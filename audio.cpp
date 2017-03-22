#include "audio.h"

using namespace std;

Audio::Audio()
{
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setCodec("audio/pcm");

//    cout << format.isValid() << endl;

    audio = new QAudioOutput(format);
    image = new QBuffer;
    image->open(QIODevice::ReadWrite);
    audioSupport = new QAudioOutput(format);
    support = new QBuffer;
    support->open(QIODevice::ReadWrite);

    cout << "Audio instance created" << endl;
    cout << audio << endl;

    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleAudioStateChanged(QAudio::State)));
}

Audio::~Audio(){
    delete audio;
    delete audioSupport;
    delete image;
    delete support;
}

void Audio::append(int x, int y, int l){

    for(int i = 0; i < l; i++){
        image->putChar(x%256);
        image->putChar(x >> 8);
        image->putChar(y%256);
        image->putChar(y >> 8);
    }
}

void Audio::clearCoords(){
    image->buffer().clear();
}

void Audio::save(const QString &filename){
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(image->buffer());
    file.close();
}

void Audio::play(){
    audio->start(image);

}

void Audio::pause(bool value){
    if(value){
        audio->suspend();
    } else {
        audio->resume();
    }

}

void Audio::stop(){
    audio->stop();
    image->reset();

    emit stopped();

}

int Audio::getLength(){
    return image->buffer().size() / 4 / format.sampleRate();
}

void Audio::appendSupport(int x, int y){
    support->putChar(x%256);
    support->putChar(x >> 8);
    support->putChar(y%256);
    support->putChar(y >> 8);
}

void Audio::clearSupport(){
    support->reset();
    support->buffer().clear();
}

void Audio::playSupport(){
    support->reset();
    audioSupport->start(support);
    connect(audioSupport, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleSupportStateChanged(QAudio::State)));
}

void Audio::stopSupport(){
    disconnect(audioSupport, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleSupportStateChanged(QAudio::State)));
    audio->stop();
}


void Audio::handleAudioStateChanged(QAudio::State state){
    switch(state){
        case QAudio::ActiveState:
            break;
        case QAudio::IdleState:
            audio->stop();
            emit stopped();
            image->reset();
            cout << "stop" << endl;
            break;
        default:
            break;
    }
}

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
