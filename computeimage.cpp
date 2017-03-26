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

/*
ComputeImage::ComputeImage()
{
    audio = NULL;
}
*/

//Create a compute object for an image.
//On each image openning, a compute object is created, and deleted when image is closed.
ComputeImage::ComputeImage(Image *file)
{
    //set a pointer to the image; get its negative. Set pointer to audio
    //TODO: link audio to the image.
    image = file;
    negative = image->getNegative();
    audio = image->getAudio();

    //Get laser settings
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    readSettings();

    //get the values from image.
    widthPix = image->getWidthPix();
    heightPix = image->getHeightPix();
    size = widthPix * heightPix;
    ratio = (float)widthPix / (float)heightPix;
    widthMm = image->getWidthMm();
    heightMm = image->getHeightMm();

    supportWidth = widthMm;
    supportHeight = heightMm;

    //initialisation of vars and constants.
    //Positions for X and Y is coded on 16 bits.
    angleMaxValue = pow(2, 15);
    pi = atan(1) * 4;

    //The max angle in cfg file is store as degrees; converting it to radians.
    maxAngleX *= pi / 180;
    maxAngleY *= pi / 180;

    //Prepare the tan of the scan max angle.
    //With python it speeds up the calculus, but it seems C++ doesn't care!
    tanXScan = tan(maxAngleX);
    tanYScan = tan(maxAngleY);

    //Initialize value for jump and offset.
    jump = 0;
    offsetX = 0;
    offsetY = 0;
}

//read the settings from the setting file.
void ComputeImage::readSettings(){
    maxAngleX = maxAngleY = settings->value("laser/maxangle").toInt();
}

//Create a new computeImage object.
void ComputeImage::update()
{
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
//This method will compute a start point for each line composing the image, using the scan angle set by user.
void ComputeImage::computeCoords(Audio *buffer)
{

    //Get the last distance value and compute angle pos for every pixel.
    updateMaxSize();
    computeAngles();

    //Display a message box if the intended size is beyond the limits of the laser, asking to increase size.
    if(widthMm > maxSizeX || heightMm > maxSizeY){
//        QString text = tr("Max size for this distance is %n x %nmm. Please decrease size or increase distance.");
        QString text = tr("Max size for this distance is ");
        text += QString::number(maxSizeX);
        text += "x";
        text += QString::number(maxSizeY);
        text += tr("mm. Please decrease size or increase distance.");
        WinInfo::info(text, tr("Size error"));
        return;
    }

   // Clear coords in buffer before to populate it
    audio = buffer;
    audio->clearCoords();

    //This is used to update progress bar.
    pixelsComputed = 0;

    //precompute tangente to speed up computing.
    float tanAngle = 0;

    //According to scan angle, find the start point for each scna line, then call bresenham from it.
    //Starting point may be out of the image for every scan angle that is not horizontal or vertical.
    //It's needed to have consistency spacing between lines while insolating.
    //Starting from the border of the picture induce different exposure values
    //when passing from a vertical to a horizontal border.
    //Bresenham method simply discards points that are outside image.
    if (scanAngle > 45){
        tanAngle = tan((90 - scanAngle) * pi / 180);
        for (int i = -tanAngle * heightPix; i < widthPix; i++){
            if((jump != 0) && (i%(jump + 1)) != 0){
                continue;
            }
            bresenham(i, heightPix - 1);
        }

    } else if (scanAngle >= 0){
        tanAngle = tan(scanAngle * pi / 180);
        int limit = heightPix + tanAngle * widthPix;
        for (int i = 0; i < limit; i++){
            if((jump != 0) && (i%(jump + 1)) != 0){
                continue;
            }
            bresenham(0, i);
        }

    } else if (scanAngle >= -45){
        tanAngle = tan(scanAngle * pi / 180);
        int limit = tanAngle * widthPix;
        for (int i = heightPix - 1; i >= limit; i--){
            if((jump != 0) && (i%(jump + 1)) != 0){
                continue;
            }
            bresenham(0, i);
        }

    } else {
        if (scanAngle != -90){
            tanAngle = -tan((90 + scanAngle) * pi / 180);
        }
        for (int i = tanAngle * heightPix; i < widthPix; i++){
            if((jump != 0) && (i%(jump + 1)) != 0){
                continue;
            }
            bresenham(i, 0);
        }
    }
}

//generate four lines according to support size
void ComputeImage::computeSupport()
{
    //Somes values used t ocompute the support border.
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int widthValue = 0;
    int heightValue = 0;

    //Compute the values of extremum.
    //TODO: this is grossly the same function as in computeAngles.
    //This two methods should be refactor to use one another.
    halfSize = (supportWidth / 2);
    angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
    angleRatio = angleValue / maxAngleX;
    widthValue = angleMaxValue * angleRatio;

    halfSize = (supportHeight / 2);
    angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
    angleRatio = angleValue / maxAngleY;
    heightValue = angleMaxValue * angleRatio;

    int x = -widthValue;
    int y = -heightValue;

    //Clear support audio buffer
    audio->clearSupport();

    //Draw each border line.
    for(;x<widthValue; x+=32){
        audio->appendSupport(x, y);
    }
    for(;y<heightValue; y+=32){
        audio->appendSupport(x, y);
    }
    for(;x>-widthValue; x-=32){
        audio->appendSupport(x, y);
    }
    for(;y>-heightValue; y-=32){
        audio->appendSupport(x, y);
    }

}

//This is a function that displays a sight, to verify that the system is correctly setup.
//TODO: probably this should be taken out of ComputeImage, and implement into Tools.
//This should also benefit from computeAngles instead of using the same amlgorithms.
void ComputeImage::calibrate(){
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int widthValue = 0;
    int heightValue = 0;

    //compute extremums.
    halfSize = 50;
    angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
    angleRatio = angleValue / maxAngleX;
    widthValue = angleMaxValue * angleRatio;

    halfSize = 50;
    angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
    angleRatio = angleValue / maxAngleY;
    heightValue = angleMaxValue * angleRatio;

    audio->clearSupport();

    //Draw the two axis.
    for(int i = -widthValue; i < widthValue; i+=32){
        audio->appendSupport(i, 0);
    }

    for(int i = -heightValue; i < heightValue; i+=32){
        audio->appendSupport(0, i);
    }

    //Draw a square
    widthValue *= 0.8;
    heightValue *= 0.8;
    int x = -widthValue;
    int y = -heightValue;

    for(; x<widthValue; x+=32){
        audio->appendSupport(x, y);
    }
    for(;y<heightValue; y+=32){
        audio->appendSupport(x, y);
    }
    for(;x>-widthValue; x-=32){
        audio->appendSupport(x, y);
    }
    for(;y>-heightValue; y-=32){
        audio->appendSupport(x, y);
    }

    //Draw a circle
    for(int i = 0; i < 720; i++){
        audio->appendSupport(widthValue * cos(i * pi / 360), heightValue * sin(i * pi / 360));
    }

}

//This function precomputes the angles  corresponding to each vertical and horizontal pixel
//That way computeCoord() can get it ready to be used in two tables, instead of asking it on each iteration
void ComputeImage::computeAngles()
{
    //Clear the two angle tables before to populate it.
    angleValueX.clear();
    angleValueY.clear();

    //Initlize some vars for computing temporary values
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int posValue = 0;

    //First get the values for width
    //
    //First is computed the distance between image center and pix pos, in mm.
    //Then this value is used to find the corresponding angle.
    //Compute a ratio between this angle and the max angle.
    //Finally use this ratio to get the laser position, by multiplying it by greatest possible value.

    //Store this value into the angleValue array.
    for(int i = 0; i<widthPix; i++)
    {
        halfSize = (i - widthPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
        angleRatio = angleValue / maxAngleX;
//        posValue = round(angleMaxValue * angleRatio);
        posValue = angleMaxValue * angleRatio;
        angleValueX.push_back(posValue);
    }

    //Then get the values for height
    for(int i = 0; i<heightPix; i++)
    {
        halfSize = (i - heightPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
        angleRatio = angleValue / maxAngleY;
//        posValue = round(angleMaxValue * angleRatio);
        posValue = angleMaxValue * angleRatio;
        angleValueY.push_back(posValue);

    }

    //get the number of points for a pixel increment.
    //That's used to offset each point from its theorical position when insolating, and is set in the GUI.
    offsetValueX = abs((float)angleValueX[0] / (widthPix / 2));
    offsetValueX *= (float)offsetX / 100;
    offsetValueY = abs((float)angleValueY[0] / (heightPix / 2));
    offsetValueY *= (float)offsetY / 100;

}

//compute the points with Bresenham algorithm
//Each of the four octant is computed differently and handles overflows
//TODO: these four cases should be condensed into one.
void ComputeImage::bresenham(int x, int y){

/*
    int previousX = x;
    int previousY = y;
    angleValue.clear();
    pixValue.clear();
*/

    if (scanAngle > 45){
        double tanAngle = tan((90 - scanAngle) * pi / 180);
        double error = -0.5;

        for (; y >= 0; y--){
            error += tanAngle;

            if(error > 0){
                x++;
                error--;
            }

            if(x < 0){
                continue;
            }

            pixelsComputed++;

            if(x >= widthPix){
                break;
            }

            QRgb pix = qBlue(negative->pixel(x, y));

            /*
            if (pix == 0){
                continue;
            }
            */
            //append values to audio
            audio->append(angleValueX[x] + offsetValueX, angleValueY[y] + offsetValueY, pix);
        }

    } else if (scanAngle >= 0){
        double tanAngle = tan(scanAngle * pi / 180);
        double error = -0.5;

        for (; x < widthPix; x++){
            error += tanAngle;

            if (error > 0){
                y--;
                error--;
            }

            if (y >= heightPix - 1){
//                previousX = x;
//                previousY = y;
                continue;
            }

            pixelsComputed++;

            if(y <= 0){
//                cout << "pos 0: " << previousX << " * " << previousY << endl;
//                cout << "pos 1: " << x << " * " << y << endl;
//                cout << endl;
                break;
            }

            QRgb pix = qBlue(negative->pixel(x, y));
//            angleValue.push_back(angleValueX[x]);
//            pixValue.push_back(pix);

            /*
            if (pix == 0){
                continue;
            }
            */
//            cout << "x: " << x << endl;
//            cout << "y: " << y << endl;
//            cout << "l: " << (int)pix << endl;
            //append values to audio
            audio->append(angleValueX[x] + offsetValueX, angleValueY[y] + offsetValueY, pix);

//            previousX = x;
//            previousY = y;
        }
//        cout << "pos 0: " << previousX << " * " << previousY << endl;
//        cout << "pos 1: " << x << " * " << y << endl;
//        cout << endl;
/*
        audio->appendBresenham(angleValueX[previousX], angleValueY[previousY],
                               angleValueX[x - 1], angleValueY[y - 1],
                               &angleValue, &pixValue);

*/
    } else if (scanAngle >= -45){
        double tanAngle = -tan(scanAngle * pi / 180);
        double error = -0.5;

        for (; x <= widthPix - 1; x++){
            error += tanAngle;

            if (error > 0){
                y++;
                error--;
            }

            if (y < 0){
                continue;
            }

            pixelsComputed++;

            if(y >= heightPix){
                break;
            }

            QRgb pix = qBlue(negative->pixel(x, y));

            /*
            if (pix == 0){
                continue;
            }
            */
            //append values to audio
            audio->append(angleValueX[x] + offsetValueX, angleValueY[y] + offsetValueY, pix);
        }

    } else {
        double tanAngle = -tan((90 - scanAngle) * pi / 180);
        double error = -0.5;

        for (; y <= heightPix - 1; y++){
            error += tanAngle;

            if(error > 0){
                x++;
                error--;
            }

            if (x < 0){
                continue;
            }

            pixelsComputed++;

            if(x >= widthPix){
                break;
            }

            QRgb pix = qBlue(negative->pixel(x, y));

            /*
            if (pix == 0){
                continue;
            }
            */
            //append values to audio
            audio->append(angleValueX[x] + offsetValueX, angleValueY[y] + offsetValueY, pix);
        }
    }

    int progress = 100 * (float)pixelsComputed / size;
    emit progressing(progress);
}

//Update the distance from GUI
void ComputeImage::setDistance(const int &value){
    distance = value;
}

//Update the width from GUI, adapt height.
void ComputeImage::setImageWidth(const int &value)
{
    widthMm = value;
    heightMm = (float)value / (float)ratio;
}

//Update the height from GUI, adapt width.
void ComputeImage::setImageHeight(const int &value)
{
    heightMm = value;
    widthMm = (float)value * (float)ratio;
}

//Update the support width from GUI
void ComputeImage::setSupportWidth(const int &value)
{
    supportWidth = value;
}

//Update the support height from GUI
void ComputeImage::setSupportHeight(const int &value)
{
    supportHeight = value;
}

//Update the scan angle from the GUI
void ComputeImage::setScanAngle(const int &value){
    scanAngle = value;
}

//Update the jump value from the GUI. This is used to jump a scanning line when insolating.
void ComputeImage::setJump(const int &value){
    jump = value;
}

//Set offset from the GUI. Used to move point position from its theorical value
void ComputeImage::setOffsetX(const int &value){
    offsetX = value;
}

//Set offset from the GUI. Used to move point position from its theorical value
void ComputeImage::setOffsetY(const int &value){
    offsetY = value;
}

