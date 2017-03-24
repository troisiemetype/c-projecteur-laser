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

#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QString>

#include <iostream>

#include "wininfo.h"


class Image
{

public:
    Image();
    Image(QString const&);
    Image(int);
    ~Image();

    void initImage();
    bool resample(int dpi);

    bool close();
    bool save();

    //getters
    QPixmap getPixmap();
    QImage getNegative();
    bool isSaved();
    int getWidthPix();
    int getHeightPix();
    int getWidthMm();
    int getHeightMm();
    int getDistance();
    int getSpeed();
    int getMode();
    int getSupportWidth();
    int getSupportHeight();
    int getBlackWhiteStep();
    int getDpi();

    //setters
    void setImageWidth(int);
    void setImageHeight(int);
    void setSupportWidth(int);
    void setSupportHeight(int);
    void setDistance(int);
    void setSpeed(int);
    void setImageMode(int);
    void setMode(int);

    void setStep(int);

private:
    QImage setGray(QImage, int);

    QImage image;
    QImage negative;
    QImage thumbnail;
    QImage thumbnailBW;

    int blackWhiteMode;
    int blackWhiteStep;

    int width;
    int height;
    float ratio;

    int widthMm;
    int heightMm;

    int distance;
    int speed;
    int mode;

    int supportWidth;
    int supportHeight;

    bool saved;


};

#endif // IMAGE_H
