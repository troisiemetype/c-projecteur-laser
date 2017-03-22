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

/*
 * This class is the heart of the program.
 * It transforms the image into coordinates for the laser, given original size, size wanted, distance, etc.
 */

#include "computeimage.h"

ComputeImage::ComputeImage()
{

}

//Create a new computeImage object.
//This copies the values it needs from the image that is opened.
//Each time the image is updated the computeImage object is created again.
ComputeImage::ComputeImage(Image file)
{
    //initialisation of vars and constants.
    //Positions for X and Y is coded on 16 bits.
    angleMaxValue = pow(2, 15);
    pi = atan(1) * 4;

    //The max angle in cfg file is store as degrees; converting it to radians.
    maxAngleX = 10 * pi / 180;
    maxAngleY = 10 * pi / 180;

    //Prepare the tan of the scan max angle.
    //With python it speeds up the calculus, but it seems C++ doesn't care!
    tanXScan = tan(maxAngleX);
    tanYScan = tan(maxAngleY);

    //get the values from image.
    image = file.getNegative();
    widthPix = file.getWidthPix();
    heightPix = file.getHeightPix();
    size = widthPix * heightPix;
    widthMm = file.getWidthMm();
    heightMm = file.getHeightMm();
    speed = file.getSpeed();
    mode = file.getMode();
    index = 0;
    distance = file.getDistance();

    supportWidth = file.getSupportWidth();
    supportHeight = file.getSupportHeight();

    //compute the minimum distance from laser to support,
    //given image size and angle value.
    if(widthPix > heightPix){
        minDistance = widthMm / (2 * tanXScan);
    } else {
        minDistance = heightMm / (2 * tanYScan);
    }

    //Get the ratio pix to mm, using the greatest image side to minimize errors.
    if (widthPix > heightPix)
    {
        ratioPixMm = double(widthMm) / widthPix;
    } else {
        ratioPixMm = double(heightMm) / heightPix;
    }

}

//Update the max size of the picture, given:
//The distance from laser to support, from user or project file,
//The max angles of the laser, from the general config file.
void ComputeImage::updateMaxSize()
{
    halfMaxSizeX = distance * tanXScan;
    halfMaxSizeY = distance * tanYScan;
    maxSizeX = 2 * halfMaxSizeX;
    maxSizeY = 2 * halfMaxSizeY;
//    cout << "max size X: " << maxSizeX << endl;

}

//Look at each pixel value of the image to create a string
//that will be sent to the laser.
void ComputeImage::computeCoords(Audio *buffer, QProgressBar *progress)
{

    //Get the last distance value and compute angle pos for every pixel.
    updateMaxSize();
    computeAngles();

    audio = buffer;
    audio->clear();
    progressBar = progress;

    pixelsComputed = 0;

    if (scanAngle > 45){
        if (scanAngle != 90){
            for (int i = 0; i < heightPix; i++){
                bresenham(0, i);
            }
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(i, heightPix - 1);
        }
    } else if (scanAngle >= 0){
        for (int i = 0; i < heightPix; i++){
            bresenham(0, i);
        }
        if (scanAngle != 0){
            for (int i = 0; i < widthPix; i++){
                bresenham(i, heightPix - 1);
            }
        }
    } else if (scanAngle >= -45){
        for (int i = heightPix - 1; i >= 0; i--){
            bresenham(0, i);
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(i, 0);
        }
    } else {
        if (scanAngle != -90){
            for (int i = heightPix - 1; i >= 0; i--){
                bresenham(0, i);
            }
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(i, 0);
        }
    }
}

void ComputeImage::computeSupport()
{
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int widthValue = 0;
    int heightValue = 0;

    halfSize = (supportWidth / 2);
    angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
    angleRatio = angleValue / maxAngleX;
    widthValue = angleMaxValue * angleRatio;

    halfSize = (supportHeight / 2);
    angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
    angleRatio = angleValue / maxAngleY;
    heightValue = angleMaxValue * angleRatio;

//    computeCommand((FLAG_X | FLAG_Y | FLAG_L | FLAG_SPEED | FLAG_MODE), 0, widthValue, -heightValue, 255, 10, 1);
//    serialData->push_back(_dataToSend);

}

//This function precomputes the angles  corresponding to each vertical and horizontal pixel
//That way computeCoord() can get it ready to be used in two tables, instead of asking it on each iteration
void ComputeImage::computeAngles()
{
    //Clear the two angle tables before to populate it.
    angleValueX.clear();
    angleValueY.clear();

    time_t timer;
    int debut = time(&timer);

    //Initlize some vars for computing temporary values
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int posValue = 0;


    //First get the values for width
    //
    //First is compute the distance between image center and pix pos, in mm.
    //Then this value is used to find the corresponding angle.
    //Compute a ration between this angle and the max angle.
    //Finally use this ratio to get the laser position, by multiplying it by greatest posible value.
    //Store this value into the angleValue array.
    for(int i = 0; i<widthPix; i++)
    {
        halfSize = (i - widthPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
        angleRatio = angleValue / maxAngleX;
        posValue = round(angleMaxValue * angleRatio);
        angleValueX.push_back(posValue);
    }

    //Then get the values for height
    for(int i = 0; i<heightPix; i++)
    {
        halfSize = (i - heightPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
        angleRatio = angleValue / maxAngleY;
        posValue = round(angleMaxValue * angleRatio);
        angleValueY.push_back(posValue);

    }

    int duree = time(&timer) - debut;
    QString message = "Angles computed in ";
    message += QString::number(duree);
    message += " seconds";

   // WinInfo::info(message);

}

//compute the points with Bresenham algorithm
//Each of the four octant is computed differently and handles overflows
void ComputeImage::bresenham(int x, int y){

    if (scanAngle > 45){
        double tanAngle = tan((90 - scanAngle) * pi / 180);
        double error = -0.5;

        for (; y >= 0; y--){
            error += tanAngle;
            pixelsComputed++;

            if(error > 0){
                x++;
                error--;
            }

            if(x >= widthPix){
                break;
            }

            QRgb pix = qBlue(image.pixel(x, y));

            if (pix == 0){
                continue;
            }
            //append values to audio
            audio->append(angleValueX[x], angleValueY[y], pix);
        }

    } else if (scanAngle >= 0){
        double tanAngle = tan(scanAngle * pi / 180);
        double error = -0.5;

        for (; x <= widthPix - 1; x++){
            error += tanAngle;
            pixelsComputed++;

            if (error > 0){
                y--;
                error--;
            }

            if(y < 0){
                break;
            }

            QRgb pix = qBlue(image.pixel(x, y));

            if (pix == 0){
                continue;
            }
//            cout << "x: " << x << endl;
//            cout << "y: " << y << endl;
//            cout << "l: " << (int)pix << endl;
            //append values to audio
            audio->append(angleValueX[x], angleValueY[y], pix);
        }

    } else if (scanAngle >= -45){
        double tanAngle = -tan(scanAngle * pi / 180);
        double error = -0.5;

        for (; x <= widthPix - 1; x++){
            error += tanAngle;
            pixelsComputed++;

            if (error > 0){
                y++;
                error--;
            }

            if(y >= heightPix){
                break;
            }

            QRgb pix = qBlue(image.pixel(x, y));

            if (pix == 0){
                continue;
            }
            //append values to audio
            audio->append(angleValueX[x], angleValueY[y], pix);
        }

    } else {
        double tanAngle = -tan((90 - scanAngle) * pi / 180);
        double error = -0.5;

        for (; y <= heightPix; y++){
            error += tanAngle;
            pixelsComputed++;

            if(error > 0){
                x++;
                error--;
            }

            if(x >= widthPix){
                break;
            }

            QRgb pix = qBlue(image.pixel(x, y));

            if (pix == 0){
                continue;
            }
            //append values to audio
            audio->append(angleValueX[x], angleValueY[y], pix);
        }
    }

    int progress = 100 * (float)pixelsComputed / size;
    progressBar->setValue(progress);

}

int ComputeImage::getMinDistance(){
    return minDistance;
}

void ComputeImage::setScanAngle(int angle){
    scanAngle = angle;
}

int ComputeImage::getDpi(){
    return float(25.4) / ratioPixMm;
}
