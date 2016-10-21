#ifndef COMPUTEIMAGE_H
#define COMPUTEIMAGE_H

#include <iostream>
#include <cmath>
#include <ctime>

#include <QString>
#include <QVector>

#include "image.h"
#include "wininfo.h"

class ComputeImage
{

public:
    ComputeImage();
    ComputeImage(Image);

    void updateMaxSize();
    void computeCoords(QVector<QString>*);
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
    int minDistance;
    double ratioPixMm;

    //Values that store the max size of the image, given distance and scan angles.
    int maxSizeX;
    int maxSizeY;
    int halfMaxSizeX;
    int halfMaxSizeY;

    //Values that store the max angle of the scan.
    double maxAngleX;
    double maxAngleY;
    double tanXScan;
    double tanYScan;
    int angleMaxValue;
    double pi;

    //two tables to stores the angles values of each pixel.
    QVector<int> angleValueX;
    QVector<int> angleValueY;

};

#endif // COMPUTEIMAGE_H
