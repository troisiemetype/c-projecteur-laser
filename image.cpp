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

#include "image.h"

using namespace std;

Image::Image()
{

}

//The Image constructor. Called by the GUI when a file is opened.
Image::Image(QString const& file)
{
    //image always contains the original file.
    //There are other QImage object used, i.e.:
    //negative is the negative version of this image. Computed again on blackWitheMode change.
    //thumbnail is the thumbnail version of the original.
    //thumbnailBW, is computed from the above, each time needed.
    image = QImage(file);
    //image = image.convertToFormat(QImage::Format_Mono);
    width = image.width();
    height = image.height();
    widthMm = 1000 * width / image.dotsPerMeterX();
    heightMm = 1000 * height / image.dotsPerMeterY();

    ratio = (float)width / (float)height;

    //default support size = image size.
    supportWidth = widthMm;
    supportHeight = heightMm;

    //The type of image we use. grayscale, thresold, etc.
    blackWhiteMode = 0;
    blackWhiteStep = 128;

    //The negative object, whom computings are made on.
    negative = setGray(image, blackWhiteMode);
    negative.invertPixels();

    if(image.height() > image.width())
    {
        thumbnail = image.scaledToHeight(500);
    } else {
        thumbnail = image.scaledToWidth(500);
    }

    saved = 0;
}

Image::~Image()
{

}

bool Image::open(QString const& file)
{
    return 0;
}

bool Image::close()
{
    return 0;
}

//Get an updated pixmap for displaying in GUI.
QPixmap Image::getPixmap()
{
    //thumbnail = thumbnail.convertToFormat(QImage::Format_Mono);

    thumbnailBW = setGray(thumbnail, blackWhiteMode);

    QPixmap pixmap;
    pixmap.convertFromImage(thumbnailBW);

    return pixmap;
}

QImage Image::getNegative()
{
    return setGray(negative, blackWhiteMode);
}

bool Image::isSaved()
{
    return saved;
}

int Image::getWidthPix()
{
    return width;
}

int Image::getHeightPix()
{
    return height;
}

int Image::getWidthMm()
{
    return widthMm;
}

int Image::getHeightMm()
{
    return heightMm;
}

int Image::getDistance()
{
    return distance;
}

int Image::getSpeed()
{
    return speed;
}

int Image::getSupportWidth()
{
    return supportWidth;
}

int Image::getSupportHeight()
{
    return supportHeight;
}

//Update the width from GUI, adapt height.
void Image::setImageWidth(int value)
{
    widthMm = value;
    heightMm = (float)value / (float)ratio;
}

//Update the height from GUI, adapt width.
void Image::setImageHeight(int value)
{
    heightMm = value;
    widthMm = (float)value * (float)ratio;
}

void Image::setSupportWidth(int value)
{
    supportWidth = value;
}

void Image::setSupportHeight(int value)
{
    supportHeight = value;
}

void Image::setDistance(int value)
{
    distance = value;
}

void Image::setSpeed(int value)
{
    speed = value;
}

void Image::setMode(int value)
{
    blackWhiteMode = value;
    negative = setGray(image, blackWhiteMode);
    negative.invertPixels();

}

//convert the image into black and white
//TODO: give the choice between a gray image and black and white thresold image.
QImage Image::setGray(QImage image, int mode)
{
    if(mode == 0)
    {
        for(int i=0; i<image.height(); i++)
        {
            //TODO: See why scanLine() gives a different color than access by pixel().
            //uchar * line = image.scanLine(i);

            for(int j=0; j<image.width(); j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = image.pixel(j, i);
                int gray = qGray(color);
                image.setPixel(j, i, qRgb(gray, gray, gray));
                //image.setPixel(j, i, qGray(image.pixel(j, i)));
            }
        }
        return image;

    } else if(mode == 1){
        image = image.convertToFormat(QImage::Format_Mono);
/*        for(int i=0; i<image.height(); i++)
        {
            //TODO: See why scanLine() gives a different color than access by pixel().
            //uchar * line = image.scanLine(i);

            for(int j=0; j<image.width(); j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = image.pixel(j, i);
                int gray = qGray(color);
                if(gray > blackWhiteStep){
                    image.setPixel(j, i, qRgb(255, 255, 255));
                    //image.setPixel(j, i, qGray(image.pixel(j, i)));
                } else {
                    image.setPixel(j, i, qRgb(0, 0, 0));
                }
            }
        }*/
        return image;
    }

}

void Image::setStep(int value){
    blackWhiteStep = value;
}
