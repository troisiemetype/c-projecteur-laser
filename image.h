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

#include "audio.h"
#include "wininfo.h"


class Image
{

public:
    Image(const QString &);
    Image(const QImage &image);
    ~Image();

    void initImage();
    bool resample(int dpi);

    bool close();
    bool save();

    //getters
    QPixmap getPixmap();
    QImage *getNegative();
    Audio *getAudio();
    inline int getWidthPix() const{return width;}
    inline int getHeightPix() const{return height;}
    inline int getWidthMm() const{return widthMm;}
    inline int getHeightMm() const{return heightMm;}

    inline int getBlackWhiteStep() const{return blackWhiteStep;}
    int getDpi();

    //setters
    void setImageWidth(int);
    void setImageHeight(int);
    void setImageMode(int);

    void setStep(int);

    enum imageMode{
        Grey,
        FloydSteinberg,
        Threshold,
    };

private:
    QImage setGray(const QImage&, int);

    QImage *original;
    QImage negative;
    QImage thumbnail;
    QImage thumbnailBW;

    Audio *audio;

    int blackWhiteMode;
    int blackWhiteStep;

    int width;
    int height;
    float ratio;

    int widthMm;
    int heightMm;

    float inchesPerMeter = 39.37;
};

#endif // IMAGE_H
