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
#include <QSettings>
#include <QString>

#include "audio.h"
#include "image.h"
#include "wininfo.h"

using namespace std;

class ComputeImage : public QObject
{
    Q_OBJECT

public:
//    ComputeImage();
    ComputeImage(Image *);
    void update();

    void updateMaxSize();
    void computeCoords(Audio *buffer);
    void computeSupport();

    void calibrate();

    //getters
    inline int getMinDistance() const{return minDistance;}
    inline int getDpi() const{return float(25.4) / ratioPixMm;}
    inline int getWidthMm() const{return widthMm;}
    inline int getHeightMm() const{return heightMm;}
    inline int getSupportWidth() const{return supportWidth;}
    inline int getSupportHeight() const{return supportHeight;}

    //setters
    void setDistance(const int &value);

    void setImageWidth(const int &value);
    void setImageHeight(const int &value);
    void setSupportWidth(const int &value);
    void setSupportHeight(const int &value);

    void setScanAngle(const int &value);
    void setJump(const int &value);
    void setOffsetX(const int &value);
    void setOffsetY(const int &value);

signals:
    void progressing(int value);

private:

    void readSettings();
    void computeAngles();
    void bresenham(int start, int end);

    void inline swap(int a, int b){
        int t = a;
        a = b;
        b = t;
    }

    //pointers to external objects
    Audio *audio;
    Image *image;
    QImage *negative;

    QSettings *settings;

    //copies of the image values
    int widthPix;
    int heightPix;
    int widthMm;
    int heightMm;
    float ratio;
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

    int jump;
    int offsetX;
    int offsetY;
    int offsetValueX;
    int offsetValueY;

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

    //these two tables store the coordinate and pixel value
    vector<int> angleValue;
    vector<int> pixValue;

    //progress of computing
    int pixelsComputed;

};

#endif // COMPUTEIMAGE_H
