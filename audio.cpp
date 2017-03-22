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
    support = new QBuffer;
    support->open(QIODevice::ReadWrite);

    cout << "Audio instance created" << endl;

    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
/*
    buffer->reset();
    audio->start(buffer);

    QEventLoop loop;

    do{
        loop.exec();
    } while(audio->state() == QAudio::ActiveState);

    loop.exit();
    */
}

Audio::~Audio(){
    delete audio;
    delete image;
    delete support;
}

void Audio::append(int x, int y, int l){

    for(int i = 0; i < l; i++){
        image->putChar(x%256);
        image->putChar(x/256);
        image->putChar(y%256);
        image->putChar(y/256);
//        cout << "x: " << x << endl;
//        cout << "y: " << y << endl;
//        cout << "l: " << (int)l << endl;
    }
}

void Audio::clear(){
    image->buffer().clear();
    support->buffer().clear();
    cout << "buffer emptied" << endl;

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

void Audio::handleStateChanged(QAudio::State state){
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
