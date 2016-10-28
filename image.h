#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QPixmap>
#include <QString>

#include <iostream>

#include "wininfo.h"


class Image
{

public:
    Image();
    Image(QString const&);
    ~Image();

    bool open(QString const&);
    bool close();
    bool save();

    //getters
    QPixmap getPixmap();
    QImage getNegative();
    bool isSaved();
    int getWidthPix();
    int getHeightPix();
    int getWidthMm();
    int getHeightMm();
    int getDistance();
    int getSpeed();
    int getSupportWidth();
    int getSupportHeight();

    //setters
    void setSupportWidth(int);
    void setSupportHeight(int);
    void setDistance(int);
    void setSpeed(int);
    void setMode(int);

    void setStep(int);

private:
    QImage setGray(QImage, int);

    QImage image;
    QImage negative;
    QImage thumbnail;
    QImage thumbnailBW;

    int blackWhiteMode;
    int blackWhiteStep;

    int width;
    int height;
    double ratio;

    int widthMm;
    int heightMm;

    int distance;
    int speed;

    int supportWidth;
    int supportHeight;

    bool saved;


};

#endif // IMAGE_H
