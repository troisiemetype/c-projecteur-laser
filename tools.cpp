#include "tools.h"

Tools::Tools()
{

}

void Tools::greyChart(int dpi){
/*
    //This is the contructor for a gray scale image, which goal is to set the speed needed.
    int sizeMm = 80;

    //create an empty image, of great size. Fill with white
    image = QImage(4000, 4000, QImage::Format_RGB32);
    image.fill(Qt::white);
    //the QPainter is used to fill the image with grey squares. Set it to the image.
    QPainter *painter = new QPainter();
    painter->begin(&image);
    int newWidth = widthMm;
    int newHeight;
    //compute 64 squares
    for(int i = 0; i < 64; i++){
        int x = 500 * (i % 8);
        int y = 500 * (i / 8);
        int pix = 255 - 4*i;

        painter->fillRect(x, y, 500, 500, QColor(pix, pix, pix));
    }

    delete painter;

    //resize the image to the dpi ratio needed.
    int newSize = sizeMm * dpi / (float)25.4;
    image = image.scaled(newSize, newSize);

    widthMm = sizeMm;
    heightMm = sizeMm;

    initImage();
*/
}

void Tools::sight(){

}
