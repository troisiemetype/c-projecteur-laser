#ifndef COMPUTEIMAGE_H
#define COMPUTEIMAGE_H

#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>

#include <QCoreApplication>
#include <QProgressBar>
#include <QString>

#include "image.h"
#include "wininfo.h"

using namespace std;

class ComputeImage
{

public:
    ComputeImage();
    ComputeImage(Image);

    void updateMaxSize();
    void computeCoords(vector<QString>*, QProgressBar*);
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
    int size;

    //Values that store the max angle of the scan.
    double maxAngleX;
    double maxAngleY;
    double tanXScan;
    double tanYScan;
    int angleMaxValue;
    double pi;

    //two tables to stores the angles values of each pixel.
    vector<int> angleValueX;
    vector<int> angleValueY;

};

#endif // COMPUTEIMAGE_H
