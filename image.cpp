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

    ratio = (double)width / height;

    //default support size = image size.
    supportWidth = widthMm;
    supportHeight = heightMm;

    blackWhiteMode = 0;
    blackWhiteStep = 128;

    negative = setGray(image, blackWhiteMode);
    negative.invertPixels();

    if(image.height() > image.width())
    {
        thumbnail = image.scaledToHeight(500);
    } else {
        thumbnail = image.scaledToWidth(500);
    }

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
    //thumbnail = thumbnail.convertToFormat(QImage::Format_Mono);

    thumbnailBW = setGray(thumbnail, blackWhiteMode);

    QPixmap pixmap;
    pixmap.convertFromImage(thumbnailBW);

    return pixmap;
}

QImage Image::getNegative()
{
    return setGray(image, blackWhiteMode);
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

int Image::getSupportWidth()
{
    return supportWidth;
}

int Image::getSupportHeight()
{
    return supportHeight;
}

void Image::setSupportWidth(int value)
{
    supportWidth = value;
    supportHeight = value / ratio;
}

void Image::setSupportHeight(int value)
{
    supportHeight = value;
    supportWidth = value * ratio;
}

void Image::setDistance(int value)
{
    distance = value;
}

void Image::setSpeed(int value)
{
    speed = value;
}

void Image::setMode(int value)
{
    blackWhiteMode = value;
    negative = setGray(image, blackWhiteMode);
    negative.invertPixels();

}

//convert the image into black and white
//TODO: give the choice between a gray image and black and white (dotted) image.
QImage Image::setGray(QImage image, int mode)
{
    if(mode == 0)
    {
        for(int i=0; i<image.height(); i++)
        {
            //TODO: See why scanLine() gives a different color than access by pixel().
            //uchar * line = image.scanLine(i);

            for(int j=0; j<image.width(); j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = image.pixel(j, i);
                int gray = qGray(color);
                image.setPixel(j, i, qRgb(gray, gray, gray));
                //image.setPixel(j, i, qGray(image.pixel(j, i)));
            }
        }
        return image;

    } else if(mode == 1){
//        image.convertToFormat(QImage::Format_Mono);
//        return image;
        for(int i=0; i<image.height(); i++)
        {
            //TODO: See why scanLine() gives a different color than access by pixel().
            //uchar * line = image.scanLine(i);

            for(int j=0; j<image.width(); j++)
            {
                //QRgb color = (QRgb)*(line + j * 4);
                QRgb color = image.pixel(j, i);
                int gray = qGray(color);
                if(gray > blackWhiteStep){
                    image.setPixel(j, i, qRgb(255, 255, 255));
                    //image.setPixel(j, i, qGray(image.pixel(j, i)));
                } else {
                    image.setPixel(j, i, qRgb(0, 0, 0));
                }
            }
        }
        return image;
    }

}

void Image::setStep(int value){
    blackWhiteStep = value;
}
