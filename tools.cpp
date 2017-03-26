#include "tools.h"

using namespace std;

Tools::Tools()
{

}

QImage Tools::greyChart(const int &dpi)
{

    //create an empty image, of great size. Fill with white
    QImage chart = QImage(4000, 4000, QImage::Format_RGB32);

    //This is the contructor for a gray scale image, which goal is to set the speed needed.
    int sizeMm = 80;

    chart.fill(Qt::white);
    //the QPainter is used to fill the image with grey squares. Set it to the image.
    QPainter *painter = new QPainter();
//    cout << painter->font().family().toStdString() << endl;

    painter->begin(&chart);
    painter->setFont(QFont("courrier", 80));
    painter->setPen(Qt::black);
    //compute 64 squares
    for(int i = 0; i < 64; i++){
        int x = 500 * (i % 8);
        int y = 500 * (i / 8);
        int pix = 255 - 4*i;

        painter->fillRect(x, y, 500, 500, QColor(pix, pix, pix));
        if(i == 32) painter->setPen(Qt::white);
        painter->drawText(x + 250, y + 450, QString::number(4 * i));
    }

    painter->setPen(Qt::black);
    painter->drawText(50, 150, QString::number(dpi) + " dpi");

    delete painter;

    //resize the image to the dpi ratio needed.
    int newSize = sizeMm * dpi / (float)25.4;
    chart.setDotsPerMeterX(dpi * (1000/25.4));
    chart.setDotsPerMeterY(dpi * (1000/25.4));
    return chart.scaled(newSize, newSize);
}

void Tools::sight(){

}
