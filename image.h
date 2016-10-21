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

private:
    bool setGray();

    QImage image;
    QImage negative;
    QImage thumbnail;

    int width;
    int height;

    int widthMm;
    int heightMm;

    int distance;
    int speed;

    bool saved;


};

#endif // IMAGE_H
