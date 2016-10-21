#include "image.h"

using namespace std;

Image::Image()
{

}

Image::Image(QString const& file)
{
    image = QImage(file);
    //image = image.convertToFormat(QImage::Format_Mono);
    width = image.width();
    height = image.height();
    widthMm = 1000 * width / image.dotsPerMeterX();
    heightMm = 1000 * height / image.dotsPerMeterY();

    setGray();
    negative = image;
    negative.invertPixels();

    saved = 0;
}

Image::~Image()
{

}

bool Image::open(QString const& file)
{
    return 0;
}

bool Image::close()
{
    return 0;
}

QPixmap Image::getPixmap()
{
    if(image.height() > image.width())
    {
        thumbnail = image.scaledToHeight(500);
    } else {
        thumbnail = image.scaledToWidth(500);
    }
    //thumbnail = thumbnail.convertToFormat(QImage::Format_Mono);

    QPixmap pixmap;
    pixmap.convertFromImage(thumbnail);

    return pixmap;
}

QImage Image::getNegative()
{
    return negative;
}

bool Image::isSaved()
{
    return saved;
}

int Image::getWidthPix()
{
    return width;
}

int Image::getHeightPix()
{
    return height;
}

int Image::getWidthMm()
{
    return widthMm;
}

int Image::getHeightMm()
{
    return heightMm;
}

int Image::getDistance()
{
    return distance;
}

int Image::getSpeed()
{
    return speed;
}

//convert the image into black and white
//TODO: give the choice between a gray image and black and white (dotted) image.
bool Image::setGray()
{
    //First test if it's not already blackand white
    if(image.allGray())
    {
        return 0;
    } else {
        //Then compute the average on each pixel

        /*
        for(int i=0; i<height; i++)
        {
            for(int j=0; j<width; j++)
            {
                QRgb color = image.pixel(j, i);
                int gray = qGray(color);
                image.setPixel(j, i, qRgb(gray, gray, gray));
//                image.setPixel(j, i, qGray(image.pixel(j, i)));
            }
        }
        */

        for(int i=0; i<height; i++)
        {
            //TODO: See why scanLine() gives a different color than access by pixel().
            //uchar * line = image.scanLine(i);

            for(int j=0; j<width; j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = image.pixel(j, i);
                int gray = qGray(color);
                image.setPixel(j, i, qRgb(gray, gray, gray));
                //image.setPixel(j, i, qGray(image.pixel(j, i)));
            }
        }


        return 1;
    }

}

