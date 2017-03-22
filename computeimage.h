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

#ifndef COMPUTEIMAGE_H
#define COMPUTEIMAGE_H

#include <bitset>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

#include <QByteArray>
#include <QCoreApplication>
#include <QObject>
#include <QProgressBar>
#include <QString>

#include "audio.h"
#include "image.h"
#include "wininfo.h"

#define FLAG_I			1 << 5
#define FLAG_X			1 << 4
#define FLAG_Y			1 << 3
#define FLAG_L			1 << 2
#define FLAG_SPEED		1 << 1
#define FLAG_MODE		1 << 0

using namespace std;

class ComputeImage : public QObject
{
    Q_OBJECT

public:
    ComputeImage();
    void init(Image);

    void updateMaxSize();
    void computeCoords(Audio *buffer);
    void computeSupport();

    int getMinDistance();
    void setScanAngle(int);
    int getDpi();

signals:
    void progressing(int value);

private:
    void computeAngles();
    void bresenham(int start, int end);
    QByteArray computeCommand(char, char, int, int, char, int, char);
    void computeCommandInt(int);
    void computeCommandChar(char);

    void inline swap(int a, int b){
        int t = a;
        a = b;
        b = t;
    }

    //pointers to external objects
    Audio *audio;
    QProgressBar *progressBar;

    //Copies of the image values
    QImage image;
    int widthPix;
    int heightPix;
    int widthMm;
    int heightMm;
    char index;
    int speed;
    int mode;
    int distance;
    int supportWidth;
    int supportHeight;
    int minDistance;
    double ratioPixMm;

    //Values that store the max size of the image, given distance and scan angles.
    int maxSizeX;
    int maxSizeY;
    int halfMaxSizeX;
    int halfMaxSizeY;
    int size;
    int scanAngle;

    //Values that store the max angle of the scan.
    double maxAngleX;
    double maxAngleY;
    double tanXScan;
    double tanYScan;
    int angleMaxValue;
    double pi;

    //two tables to stores the angles values of each pixel.
    vector<int> angleValueX;
    vector<int> angleValueY;

    //progress of computing
    int pixelsComputed;

};

#endif // COMPUTEIMAGE_H
