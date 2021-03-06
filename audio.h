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

#ifndef AUDIO_H
#define AUDIO_H

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#include <QEventLoop>
#include <QFile>
#include <QIODevice>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QTimer>


#include <QDebug>

#include <cmath>
#include <iostream>
#include <vector>


#include "wininfo.h"

using namespace std;

class Audio : public QObject
{
    Q_OBJECT

public:
    Audio();
    ~Audio();
    void append(int x, int y, int lValue);
    void appendBresenham(int x0, int y0, int x1, int y1, vector<int>*, vector<int>*);
    void clearCoords();
    void save(const QString &filename);
    void play(int value);
    void pause(bool value);
    void stop();

    int getLength();

    void setRepeat(const int & value);

    void appendSupport(const int &x, const int &y);
    void clearSupport();
    void playSupport();
    void stopSupport();

    void displayDeviceInfo();
    void setFormat();

    inline void swap(int a, int b){
        int t = a;
        a = b;
        b = t;
    }

    inline char* wByte(int a){
        char *data = new char();
        for (int i = 0; i<4 ; i++){
            data[i] = a >> (i * 8);
        }
        return data;
    }

    //Setters

    void setExposure(const int&);
    inline int getExposure() const{return exposure;}
    inline int getRepeat() const{return repeat;}

protected:
    QAudioFormat format;
    QAudioOutput *audio;
    QAudioOutput *audioSupport;
    QBuffer *image;
    QBuffer *support;
    QTimer *timer;
    QSettings *settings;

    int sampleRate;
    int sampleSize;

    int exposure;
    int repeat;
    int repeatCount;
    int length;
    int elapsed;

public slots:
    void handleAudioStateChanged(QAudio::State);
    void handleSupportStateChanged(QAudio::State);
    void handleTimer();

signals:
    void stopped();
    void progressing(int);

};

#endif // AUDIO_H
