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
    void clearCoords();
    void save(const QString &filename);
    void play();
    void pause(bool value);
    void stop();
    int getLength();

    void appendSupport(int x, int y);
    void clearSupport();
    void playSupport();
    void stopSupport();

protected:
    QAudioFormat format;
    QAudioOutput *audio;
    QAudioOutput *audioSupport;
    QBuffer *image;
    QBuffer *support;

public slots:
    void handleAudioStateChanged(QAudio::State);
    void handleSupportStateChanged(QAudio::State);

signals:
    void stopped();
};

#endif // AUDIO_H
