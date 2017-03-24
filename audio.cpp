/*
 * This program is intended to control a laser projector
 * Copyright (C) 2016  Pierre-Loup Martin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This class transforms the values sent by ComputeImage to a wave sound. An plays it wen needed.
 * The X axis goes on right channel, and Y axis on left.
 */

#include "audio.h"

using namespace std;

//constructor
Audio::Audio()
{

    sampleRate = 48000;
    sampleSize = 16;
    //set audio format.
    setFormat();
//    displayDeviceInfo();

    //    cout << format.isValid() << endl;

    //set the default value for expoure and repeat
    exposure = 100;
    repeat = 0;

    //Create two buffers. One for the file to send, the other one is used to display support contour
    audio = new QAudioOutput(format);
    image = new QBuffer;
    image->open(QIODevice::ReadWrite);

    audioSupport = new QAudioOutput(format);
    support = new QBuffer;
    support->open(QIODevice::ReadWrite);

    //The timer is launch when playing and is used to update GUI progress bar.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(handleTimer()));

//    cout << "Audio instance created" << endl;
//    cout << audio << endl;

    //Connect signal for audio state change, so as GUI can update when file has been sent.
    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleAudioStateChanged(QAudio::State)));
}

//Destructor
Audio::~Audio(){
    delete audio;
    delete audioSupport;
    delete image;
    delete support;
}

//Set the format of the audio output
void Audio::setFormat(){
    format.setSampleRate(sampleRate);
    format.setChannelCount(2);
    format.setSampleSize(sampleSize);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setCodec("audio/pcm");
}

void Audio::displayDeviceInfo(){

    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();

    QStringList codecs = device.supportedCodecs();
    QList<int> channels = device.supportedChannelCounts();
    QList<int> sampleRates = device.supportedSampleRates();
    QList<int> sampleSizes = device.supportedSampleSizes();

    cout << "codecs" << endl;
    for (int i = 0; i < codecs.size(); i++){
        cout << "- " << codecs.at(i).toStdString() << endl;
    }

    cout << "channels" << endl;
    for (int i = 0; i < channels.size(); i++){
        cout << "- " << channels.at(i) << endl;
    }

    cout << "sample rates" << endl;
    for (int i = 0; i < sampleRates.size(); i++){
        cout << "- " << sampleRates.at(i) << endl;
    }

    cout << "sample sizes" << endl;
    for (int i = 0; i < sampleSizes.size(); i++){
        cout << "- " << sampleSizes.at(i) << endl;
    }



}

//Populate buffer.
//L is the laser intensity we need (it's a photographic negative).
//Each step of intenity is tranlated to a sample at the same place,
//i.e. for intenity of 35, the laser will stay at the same place for 35 frames
void Audio::append(int x, int y, int l){
    int limit = l * (float)exposure / 100;

//    cout << "appending" << endl;

    //Use >> instead of / to avoid zero doubling.
    for(int i = 0; i < limit; i++){
        image->putChar(x);
        image->putChar(x >> 8);
        image->putChar(y);
        image->putChar(y >> 8);

//        cout << "i: " << i << endl;
    }
}

void Audio::appendBresenham(int x0, int y0, int x1, int y1, vector<int> *angle, vector<int> *pix){
    if(angle->size() < 2) return;

    int max = angle->size() - 1;

    int current = 0;
    int pixValue = pix->at(current);
    int delta = (abs(angle->at(0) - angle->at(angle->size() - 1)) / angle->size() - 1);
    float durationStepPix = 256 / (float)delta;
    int limit = 1;


    cout << "size: " << angle->size() << endl;
    cout << "delta: " << delta << endl;
    cout << "step: " << durationStepPix << endl;
    cout << endl;


    int steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep){
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1){
        swap(x0, x1);
        swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);

    int error = dx/2;
    int step;

    if(y0 < y1){
        step = 1;
    } else {
        step = -1;
    }

    for(; x0 < x1; x0++){
//        cout << limit << endl;
        error -= dy;
        if(error < 0){
            y0 += step;
            error += dx;
        }
/*
        if(step == 1){
            if((current < max) && (x0 > angle->at(current))){
                pixValue = pix->at(current);
                limit = (float)pixValue / durationStepPix;
                current++;
            }
        } else {
            if((current < max) && (x0 < angle->at(current))){
                pixValue = pix->at(current);
                limit = (float)pixValue / durationStepPix;
                current++;
            }
        }
*/
        if(steep){
            for(int i = 0; i <= limit; i++){
                image->putChar(y0%256);
                image->putChar(y0 >> 8);
                image->putChar(x0%256);
                image->putChar(x0 >> 8);
            }
        } else {
            for(int i = 0; i <= limit; i++){
                image->putChar(x0%256);
                image->putChar(x0 >> 8);
                image->putChar(y0%256);
                image->putChar(y0 >> 8);
            }
        }
    }
}

//Clear the file buffer
void Audio::clearCoords(){
    image->buffer().clear();
}

//Save the buffer to a wav file.
//The file name is sent from GUI when the button "save" is pressed.
//TODO: doesn't work yet.
void Audio::save(const QString &filename){
    //Create a file, open it in read mode.
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    //Definition of the header of the file.
    //file type block ID
    file.write("RIFF");
    //file size
    file.write(wByte(36 + image->buffer().size()), 4);
    //file format id
    file.write("WAVE");

    //audio format
    //format bloc id
    file.write("fmt ");
    //block size
    file.write(wByte(16), 4);
    //audio format (1 for PCM)
    file.write(wByte(1), 2);
    //channels number
    file.write(wByte(2), 2);
    //sample rate
    file.write(wByte(sampleRate), 4);
    //byte rate
    file.write(wByte(sampleRate * sampleSize * 2 / 8), 4);
    //byte per block (nb channels * bit per sample / 8)
    file.write(wByte(2 * sampleSize / 8), 2);
    //bits per sample
    file.write(wByte(sampleSize), 2);

    //Data
    //data block id
    file.write("data");
    //data size
    file.write(wByte(image->buffer().size()), 4);
    file.write(image->buffer());
    file.close();
}

//Play the file, so that the laser can insolate it.
//Value is sent from  the GUI, it's the number of times the file must be looped.
void Audio::play(int value){
    repeat = value;
    getLength();
    elapsed = 0;
    timer->start(1000);
    audio->start(image);
}

//Pause/resume the sending.
void Audio::pause(bool value){
    if(value){
        timer->stop();
        audio->suspend();
    } else {
        timer->start(1000);
        audio->resume();
    }

}

//Stop sending file.
//Signal is emmited so that GUI can reactivate areas.
void Audio::stop(){
    timer->stop();
    audio->stop();
    image->reset();

    emit stopped();

}

//return insolation length, in seconds.
int Audio::getLength(){
    return length = (repeat + 1) * image->buffer().size() / 4 / format.sampleRate();
}

//Populate support buffer.
void Audio::appendSupport(int x, int y){
    support->putChar(x%256);
    support->putChar(x >> 8);
    support->putChar(y%256);
    support->putChar(y >> 8);
}

//Clear support buffer.
void Audio::clearSupport(){
    support->reset();
    support->buffer().clear();
}

//Send support.
//Signal is connected on each send, because the file loops as long as the GUI button is pressed.
void Audio::playSupport(){
    support->reset();
    audioSupport->start(support);
    connect(audioSupport, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleSupportStateChanged(QAudio::State)));
}

//Stop send support.
//Disconnect signal, so loop ends.
void Audio::stopSupport(){
    disconnect(audioSupport, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleSupportStateChanged(QAudio::State)));
    audio->stop();
}

//Handle file state change.
//set buffer back to beginning when finished, and repeat the whole sending if needed.
//Also emits Signal for GUI to enable back areas.
void Audio::handleAudioStateChanged(QAudio::State state){
    switch(state){
        case QAudio::ActiveState:
            break;
        case QAudio::IdleState:
            if (repeat != 0){
                image->reset();
                repeat--;
            } else {
                timer->stop();
                audio->stop();
                emit stopped();
                image->reset();
//                cout << "stop" << endl;
            }
            break;
        default:
            break;
    }
}

//handle support state change.
//Just reset the buffer to beginning, so that the sending loops as long as button is pressed.
void Audio::handleSupportStateChanged(QAudio::State state){
    switch(state){
        case QAudio::IdleState:
            support->reset();
            break;
        case QAudio::ActiveState:
            break;
        default:
            break;
    }
}

//Save the value of exposure when it's changed int othe GUI.
void Audio::handleExposureChanged(int value){
    exposure = value;
}

void Audio::handleTimer(){
    elapsed++;
    emit progressing(elapsed);
}
