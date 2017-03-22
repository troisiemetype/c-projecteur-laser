#ifndef AUDIO_H
#define AUDIO_H

#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#include <QEventLoop>
#include <QFile>
#include <QIODevice>
#include <QObject>
#include <QString>


#include <QDebug>

#include <cmath>
#include <iostream>


class Audio : public QObject
{
    Q_OBJECT

public:
    Audio();
    ~Audio();
    void append(int x, int y, int lValue);
    void clear();
    void save(const QString &filename);
    void play();
    void pause(bool value);
    void stop();
    int getLength();

protected:
    QAudioFormat format;
    QAudioOutput *audio;
    QBuffer *image;
    QBuffer *support;

public slots:
    void handleStateChanged(QAudio::State);

signals:
    void stopped();
};

#endif // AUDIO_H
