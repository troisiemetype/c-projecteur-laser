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

/* This class is used for image management.
 * It opens and close the image when asking from GUI.
 * It can also construct a gray chart with 64 squares going from white to black
 *
 * It inits image size, speed and distance from laser to support
 * It computes the negative image needed for projection, and it alos provides thumbnail for GUI
 *
 * It has a function that transforms the originale image to greyscale, floyd-steinberg compression, or treshold.
 *
 * Finally, it has getters a and setters for the GUI to display and eventually modify values.
 */

#include "image.h"

using namespace std;

//The Image constructor. Called by the GUI when a file is opened.
Image::Image(const QString& file)
{
    //Create the image from an image file
    original = new QImage(file);

    initImage();
}

Image::Image(QImage *image){
    //Create the image from an image file
    original = image;

    initImage();

}

//This is the contructor for a gray scale image, which goal is to set the speed needed.
Image::Image(int dpi)
{
    int sizeMm = 80;

    //create an empty image, of great size. Fill with white
    original = new QImage(4000, 4000, QImage::Format_RGB32);
    original->fill(Qt::white);
    //the QPainter is used to fill the image with grey squares. Set it to the image.
    QPainter *painter = new QPainter();
    painter->begin(original);
    int newWidth = widthMm;
    int newHeight;
    //compute 64 squares
    for(int i = 0; i < 64; i++){
        int x = 500 * (i % 8);
        int y = 500 * (i / 8);
        int pix = 255 - 4*i;

        painter->fillRect(x, y, 500, 500, QColor(pix, pix, pix));
    }

    delete painter;

    //resize the image to the dpi ratio needed.
    int newSize = sizeMm * dpi / (float)25.4;
    *original = original->scaled(newSize, newSize);

    widthMm = sizeMm;
    heightMm = sizeMm;

    initImage();
}

Image::~Image()
{
    //delete original;
}

void Image::initImage()
{
    //image always contains the original file.
    //There are other QImage object used, i.e.:
    //negative is the negative version of this image. Computed again on blackWitheMode change.
    //thumbnail is the thumbnail version of the original.
    //thumbnailBW, is computed from the above, each time needed.

    width = original->width();
    height = original->height();

    //Get the size in mm from the size in pixels and the dpi ratio.
    widthMm = 1000 * width / original->dotsPerMeterX();
    heightMm = 1000 * height / original->dotsPerMeterY();

    ratio = (float)width / (float)height;

    //The type of image we use. grayscale, thresold, etc.
    blackWhiteMode = Grey;
    blackWhiteStep = 127;

    //The negative object, whom computings are made on.
    negative = setGray(*original, blackWhiteMode);
    negative.invertPixels();

    if(original->height() > original->width())
    {
        thumbnail = original->scaledToHeight(500);
    } else {
        thumbnail = original->scaledToWidth(500);
    }

}

bool Image::resample(int dpi){
    int newWidthPix = widthMm * dpi / (float)25.4;
    int newHeightPix = heightMm * dpi / (float)25.4;

    *original = original->scaled(newWidthPix, newHeightPix);

    widthMm = 1000 * original->width() / original->dotsPerMeterX();
    heightMm = 1000 * original->height() / original->dotsPerMeterY();

    initImage();
}

bool Image::close()
{
    return 0;
}

//Get an updated pixmap for displaying in GUI.
QPixmap Image::getPixmap()
{
    thumbnailBW = setGray(thumbnail, blackWhiteMode);

    QPixmap pixmap;
    pixmap.convertFromImage(thumbnailBW);

    return pixmap;
}

QImage *Image::getNegative()
{
    setGray(negative, blackWhiteMode);
    return &negative;
}

Audio *Image::getAudio(){
    return audio;
}

int Image::getDpi(){
    //39.3 is the quantity of inches in a meter
    return original->dotsPerMeterX() / inchesPerMeter;
}

void Image::setImageMode(int value)
{
    blackWhiteMode = value;
    negative = setGray(*original, blackWhiteMode);
    negative.invertPixels();

}

//convert the image into black and white, in the mode wanted,
//i.e. grayscale, floyd-teinberg or threshold
QImage Image::setGray(const QImage &image, int mode)
{
    QImage result = QImage(image);
    //mode == 0: grayscale image
    if(mode == Grey)
    {
        for(int i=0; i<result.height(); i++)
        {
            //TODO: See why scanLine() gives a different color than access by pixel().
            //uchar * line = result.scanLine(i);

            for(int j=0; j<result.width(); j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = result.pixel(j, i);
                int gray = qGray(color);
                result.setPixel(j, i, qRgb(gray, gray, gray));
                //result.setPixel(j, i, qGray(result.pixel(j, i)));
            }
        }
    //mode == 1: Floyd-Steinberg image
    } else if(mode == FloydSteinberg){
        result = result.convertToFormat(QImage::Format_Mono);

    //mode == 2: Threshold image
    } else if(mode == Threshold){
        for(int i=0; i<result.height(); i++)
        {
            for(int j=0; j<result.width(); j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = result.pixel(j, i);
                int gray = qGray(color);
                if(gray > blackWhiteStep){
                    result.setPixel(j, i, qRgb(255, 255, 255));
                } else {
                    result.setPixel(j, i, qRgb(0, 0, 0));
                }
                //result.setPixel(j, i, qGray(result.pixel(j, i)));
            }
        }
    }
    return result;
}

void Image::setStep(int value){
    blackWhiteStep = value;
}
