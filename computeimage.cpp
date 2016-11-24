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
    maxAngleX = 20 * pi / 180;
    maxAngleY = 20 * pi / 180;

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
    distance = 400;

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
/*
    cout << "ratio pix/mm: " << ratioPixMm << endl;
    cout << "width mm: " << widthMm << endl;
    cout << "distance: " << distance << endl;
    cout << "width pix: " << widthPix << endl;
    cout << "max angle X: " << maxAngleX << endl;
    cout << "max pos: " << angleMaxValue << endl;
*/

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
void ComputeImage::computeCoords(vector<string>* serialData, QProgressBar* progressBar)
{
    _serialData = serialData;

    //Get the last distance value and compute angle pos for every pixel.
    updateMaxSize();
    computeAngles();

    time_t timer;
    int debut = time(&timer);

//    int index = 0;
//    double percent = 0;

//    string dataToSend = "";

    int angle = 50;

//    int mode = 1;

    if (angle <= 90 && angle > 45){

        if (angle != 90){
            for (int i = 0; i < heightPix; i++){
                bresenham(i, 0, 0, angle);
            }
        }

        for (int i = 0; i < widthPix; i++){
            bresenham(heightPix - 1, 0, i, angle);
        }

    } else if (angle <= 45 && angle >= 0){

        for (int i = 0; i < heightPix; i++){
            bresenham(0, widthPix - 1, i, angle);
        }

        if (angle != 0){
            for (int i = 0; i < widthPix; i++){
                bresenham(i, widthPix - 1, heightPix - 1, angle);
            }
        }

    } else if (angle < 0 && angle >= -45){

        for (int i = heightPix - 1; i >= 0; i--){
            bresenham(0, widthPix - 1, i, angle);
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(i, widthPix - 1, 0, angle);
        }


    } else if (angle < -45 && angle >= -90){

        if (angle != -90){
            for (int i = heightPix - 1; i >= 0; i--){
                bresenham(i, heightPix - 1, 0, angle);
            }
        }
        for (int i = 0; i < widthPix; i++){
            bresenham(0, heightPix - 1, 0, angle);
        }

    }

    int duree = time(&timer) - debut;
    QString message = "Position values computed in ";
    message += QString::number(duree);
    message += " seconds";

//    WinInfo::info(message);

}

void ComputeImage::computeSupport(vector<QString> *serialData)
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

    QString dataToSend = "L255";
    dataToSend += "X";
    dataToSend += QString::number(widthValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

    dataToSend = "Y";
    dataToSend += QString::number(heightValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

    dataToSend = "X";
    dataToSend += QString::number(-widthValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

    dataToSend = "Y";
    dataToSend += QString::number(-heightValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

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

    string dataToSend = "";

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
                break;
            }

            QRgb pix = qBlue(image.pixel(pos, i));

            if(pix == 0)
            {
                continue;
            }

            unsigned char c = FLAG_X | FLAG_Y | FLAG_L;
//            cout << bitset<8>(c) << endl;

            unsigned char checksum = c;
            dataToSend = c;
            c = angleValueX[pos]/256;
            checksum += c;
            dataToSend += c;
//            cout << bitset<8>(c) << endl;

            c = angleValueX[pos]%256;
            checksum += c;
            dataToSend += c;
//            cout << bitset<8>(c) << endl;

            c = angleValueY[i]/256;
            checksum += c;
            dataToSend += c;
//            cout << bitset<8>(c) << endl;

            c = angleValueY[i]%256;
            checksum += c;
            dataToSend += c;
//            cout << bitset<8>(c) << endl;

            c = pix;
            checksum += c;
            dataToSend += c;
//            cout << bitset<8>(c) << endl;

            dataToSend += checksum;
//            cout << bitset<8>(checksum) << endl;

            _serialData->push_back(dataToSend);
//            cout << angleValueX[pos] << endl;
//            cout << angleValueY[i] << endl;

            dataToSend = FLAG_L;
            dataToSend += (char)0;
            dataToSend += FLAG_L;
            _serialData->push_back(dataToSend);
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
                break;
            }
            QRgb pix = qBlue(image.pixel(i, pos));

            if(pix == 0)
            {
                continue;
            }
/*            "X";
            QString::number(angleValueX[i]);
            "Y";
            QString::number(angleValueY[pos]);
            QString::number(pix);
//            dataToSend += "M1";
//            dataToSend += "S3000";
            dataToSend += '\n';
            _serialData->push_back(dataToSend);
            _serialData->push_back("L0M0\n");
            */
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
                break;
            }
            QRgb pix = qBlue(image.pixel(i, pos));

            if(pix == 0)
            {
                continue;
            }

 /*           dataToSend = "X";
            dataToSend += QString::number(angleValueX[i]);
            dataToSend += "Y";
            dataToSend += QString::number(angleValueY[pos]);
            dataToSend += "L";
            dataToSend += QString::number(pix);
            dataToSend += '\n';
            _serialData->push_back(dataToSend);
            _serialData->push_back("L0\n");
 */       }
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
                break;
            }
            QRgb pix = qBlue(image.pixel(pos, i));

            if(pix == 0)
            {
                continue;
            }

 /*           dataToSend = "X";
            dataToSend += QString::number(angleValueX[pos]);
            dataToSend += "Y";
            dataToSend += QString::number(angleValueY[i]);
            dataToSend += "L";
            dataToSend += QString::number(pix);
            dataToSend += '\n';
            _serialData->push_back(dataToSend);
            _serialData->push_back("L0\n");
    */    }
    }

}

int ComputeImage::getMinDistance()
{
    return minDistance;
}
