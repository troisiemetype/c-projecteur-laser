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
    audio = new Audio();

    initImage();
}

//Second constructor. called from the GUI for tools, construct an image from an image given to it.
Image::Image(const QImage &image){
    //Create the image from a given QImage
    original = new QImage(image);
    audio = new Audio();

    initImage();

}

//Destructor
Image::~Image()
{
    delete original;
    delete audio;
}

//Init image. Get its size, dpi, ratio. Create a negative and a thumbnail from it.
void Image::initImage()
{
    //original always contains the original file.
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

    //Sets the right image mode, and also generate the negative.
    setStep(127);
    setImageMode(Grey);

    //Create a thumbnail of 500 pix max.
    //TODO: images smaller than 500 pix should not be resized.
    if(original->height() > original->width())
    {
        thumbnail = original->scaledToHeight(500);
    } else {
        thumbnail = original->scaledToWidth(500);
    }

}

//Resample function. Used to change image size, given dpi.
//TODO: doesn't work as expected. Some work to do here again.
bool Image::resample(int dpi){
    int newWidthPix = widthMm * dpi / (float)25.4;
    int newHeightPix = heightMm * dpi / (float)25.4;

    *original = original->scaled(newWidthPix, newHeightPix);

    widthMm = 1000 * original->width() / original->dotsPerMeterX();
    heightMm = 1000 * original->height() / original->dotsPerMeterY();

    initImage();
}

//Get an updated pixmap for displaying in GUI.
QPixmap Image::getPixmap()
{
    //Create a black and white thumbnail from original one.
    //This is created as a local variable, as it's just used by the GUI, and computed on each setting change.
    QImage thumbnailBW = setGray(thumbnail, blackWhiteMode);

    //Create pixmap from it, then return it to the caller.
    QPixmap pixmap;
    pixmap.convertFromImage(thumbnailBW);

    return pixmap;
}

//Give pointer to negative. Called by ComputeImage.
QImage *Image::getNegative()
{
    return &negative;
}

//Give pointer to audio. Called by ComputeImage
Audio *Image::getAudio(){
    return audio;
}

//Get the dpi ratio.
//TODO: may probably be suppress, and implemented elsewhere.
int Image::getDpi(){
    return original->dotsPerMeterX() / inchesPerMeter;
}

//Set image mode, i.e. grey, Floyd-Steinberg, etc. Also compute a new negative.
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

    //For each column in each row, get the pixel value, convert it to gray, then set it again on pixel.
    //Floyd-Steinberg uses a dedicate method from Qt library.
    //Threshold test the pixel value against limit value, the nset pixel to white, or black.
    //mode == 0: grayscale image
    if(mode == Grey)
    {
        for(int i=0; i<result.height(); i++)
        {
            for(int j=0; j<result.width(); j++)
            {
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

//Get the tep value from the GUI.
void Image::setStep(int value){
    blackWhiteStep = value;
}
