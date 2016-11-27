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
    //distance = file.getDistance();
    //Temporary distance value for tests.
    distance = 650;

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
//This string is like:
//data = IidLlvalueXxvalueYyvalueSspeede.getSe.getS
void ComputeImage::computeCoords(vector<QByteArray>* serialData, QProgressBar* progressBar)
{
    _serialData = serialData;

    //Get the last distance value and compute angle pos for every pixel.
    updateMaxSize();
    computeAngles();

    time_t timer;
    int debut = time(&timer);

    if (scanAngle <= 90 && scanAngle > 45){

        if (scanAngle != 90){
            for (int i = 0; i < heightPix; i++){
                bresenham(i, 0, 0, scanAngle);
            }
        }

        for (int i = 0; i < widthPix; i++){
            bresenham(heightPix - 1, 0, i, scanAngle);
        }

    } else if (scanAngle <= 45 && scanAngle >= 0){

        for (int i = 0; i < heightPix; i++){
            bresenham(0, widthPix - 1, i, scanAngle);
        }

        if (scanAngle != 0){
            for (int i = 0; i < widthPix; i++){
                bresenham(i, widthPix - 1, heightPix - 1, scanAngle);
            }
        }

    } else if (scanAngle < 0 && scanAngle >= -45){

        for (int i = heightPix - 1; i >= 0; i--){
            bresenham(0, widthPix - 1, i, scanAngle);
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(i, widthPix - 1, 0, scanAngle);
        }


    } else if (scanAngle < -45 && scanAngle >= -90){

        if (scanAngle != -90){
            for (int i = heightPix - 1; i >= 0; i--){
                bresenham(i, heightPix - 1, 0, scanAngle);
            }
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(0, heightPix - 1, 0, scanAngle);
        }

    }

    int duree = time(&timer) - debut;
    QString message = "Position values computed in ";
    message += QString::number(duree);
    message += " seconds";

//    WinInfo::info(message);

}

void ComputeImage::computeSupport(vector<QByteArray> *serialData)
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


    computeCommand((FLAG_X | FLAG_L), widthValue, 0, 255, 0, 0);
    serialData->push_back(_dataToSend);

    computeCommand((FLAG_Y | FLAG_L), 0, heightValue, 255, 0, 0);
    serialData->push_back(_dataToSend);

    computeCommand((FLAG_X | FLAG_L), -widthValue, 0, 255, 0, 0);
    serialData->push_back(_dataToSend);

    computeCommand((FLAG_Y | FLAG_L), 0, -heightValue, 255, 0, 0);
    serialData->push_back(_dataToSend);

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
    //First is compute the distance between image centerand pix pos, in mm.
    //Then this value is used to find the corresponding angle.
    //Compute a ration between this angle and the max angle.
    //Finally use this ratio to get the laser position, by multiplying it by greatest posible value.
    //Store this value into the angleValue array.
    for(int i = 0; i<widthPix; i++)
    {
        halfSize = (i - widthPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
        angleRatio = angleValue / maxAngleX;
        posValue = angleMaxValue * angleRatio;
        angleValueX.push_back(posValue);
    }

    //The get the values for height
    for(int i = 0; i<heightPix; i++)
    {
        halfSize = (i - heightPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
        angleRatio = angleValue / maxAngleY;
        posValue = angleMaxValue * angleRatio;
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
void ComputeImage::bresenham(int start, int end, int pos, int angle){

    QByteArray dataToSend = "";

    //computing for second octant
    if (angle <= 90 && angle > 45){
        double tanAngle = tan((90 - angle) * pi / 180);
        double error = -0.5;

        for (int i = start; i >= end; i--){
            error += tanAngle;
            if (error > 0){
                pos++;
                error--;
            }

            if (pos >= widthPix){

                computeCommand(FLAG_L, 0, 0, 0, 0, 0);
                _serialData->push_back(_dataToSend);

                break;
            }

            QRgb pix = qBlue(image.pixel(pos, i));

            if(pix == 0)
            {
                continue;
            }
//            cout << angleValueX[pos] << endl;
//            cout << angleValueY[i] << endl;

            computeCommand((FLAG_X | FLAG_Y | FLAG_L),
                           angleValueX[pos],
                           angleValueY[i],
                           pix, 0, 0);
            _serialData->push_back(_dataToSend);

        }

    //Computing for first octant
    } else if (angle <=45 && angle >=0){
        double tanAngle = tan(angle * pi / 180);
        double error = 0.5;

        for (int i = start; i <= end; i++){
            error -= tanAngle;
            if (error < 0){
                pos--;
                error++;
            }

            if (pos < 0){

                computeCommand(FLAG_L, 0, 0, 0, 0, 0);
                _serialData->push_back(_dataToSend);

                break;
            }
            QRgb pix = qBlue(image.pixel(i, pos));

            if(pix == 0)
            {
                continue;
            }

            computeCommand((FLAG_X | FLAG_Y | FLAG_L),
                           angleValueX[i],
                           angleValueY[pos],
                           pix, 0, 0);
            _serialData->push_back(_dataToSend);
      }

    //Computing for height octant
    } else if (angle < 0 && angle >= -45){
        double tanAngle = tan(angle * pi / 180);
        double error = -0.5;

        for (int i = start; i <= end; i++){
            error -= tanAngle;
            if (error > 0){
                pos++;
                error--;
            }

            if (pos >= heightPix){

                computeCommand(FLAG_L, 0, 0, 0, 0, 0);
                _serialData->push_back(_dataToSend);

                break;
            }
            QRgb pix = qBlue(image.pixel(i, pos));

            if(pix == 0)
            {
                continue;
            }

            computeCommand((FLAG_X | FLAG_Y | FLAG_L),
                           angleValueX[i],
                           angleValueY[pos],
                           pix, 0, 0);
            _serialData->push_back(_dataToSend);
            char c = FLAG_X | FLAG_Y | FLAG_L;

        }
    //Computing for seventh octant
    } else if (angle < -45 && angle >= -90){
        double tanAngle = tan((-90 - angle) * pi / 180);
        double error = -0.5;

        for (int i = start; i <= end; i++){
            error -= tanAngle;
            if (error > 0){
                pos++;
                error--;
            }

            if (pos > widthPix){

                computeCommand(FLAG_L, 0, 0, 0, 0, 0);
                _serialData->push_back(_dataToSend);

                break;
            }
            QRgb pix = qBlue(image.pixel(pos, i));

            if(pix == 0)
            {
                continue;
            }
            computeCommand((FLAG_X | FLAG_Y | FLAG_L),
                           angleValueX[pos],
                           angleValueY[i],
                           pix, 0, 0);
            _serialData->push_back(_dataToSend);
        }
    }
}

void ComputeImage::computeCommand(char flags, int posX, int posY, char posL, int speed, char mode)
{
    _dataToSend.clear();
    _checksum = 0;

    computeCommandChar(flags);

    if(flags & FLAG_X){
        computeCommandInt(posX);
    }

    if(flags & FLAG_Y){
        computeCommandInt(posY);
    }

    if(flags & FLAG_L){
        computeCommandChar(posL);
    }

    if(flags & FLAG_SPEED){
        computeCommandInt(speed);
    }

    if(flags & FLAG_MODE){
        computeCommandChar(mode);
    }

    computeCommandChar(_checksum);
//    cout << endl;


}

void ComputeImage::computeCommandInt(int val)
{
    computeCommandChar(val/256);
    computeCommandChar(val%256);
}

void ComputeImage::computeCommandChar(char val)
{
    _checksum += (unsigned char)val;
    _dataToSend.append(val);
//    cout << bitset<8>(val) << endl;

}

int ComputeImage::getMinDistance()
{
    return minDistance;
}

void ComputeImage::setScanAngle(int angle){
    scanAngle = angle;
}
