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

#include <QCoreApplication>
#include <QProgressBar>
#include <QString>

#include "image.h"
#include "wininfo.h"

using namespace std;

class ComputeImage
{

public:
    ComputeImage();
    ComputeImage(Image);

    void updateMaxSize();
    void computeCoords(vector<QString>*, QProgressBar*);
    void computeSupport(vector<QString>*);
    void computeAngles();

    int getMinDistance();

private:

    //Copies of the image values
    QImage image;
    int widthPix;
    int heightPix;
    int widthMm;
    int heightMm;
    int speed;
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

};

#endif // COMPUTEIMAGE_H
