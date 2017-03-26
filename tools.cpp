#include "tools.h"

using namespace std;

Tools::Tools()
{

}

//This is a static function for generating a greychart.
//It's called by passing it a dpi value, and eventually a size in mm.
QImage Tools::greyChart(const int &dpi, const int &sizeMm)
{

    //create an empty image, of great size. Fill with white
    QImage chart = QImage(4000, 4000, QImage::Format_RGB32);

    chart.fill(Qt::white);
    //the QPainter is used to construct the image. Set it to the image.
    QPainter *painter = new QPainter();
    painter->begin(&chart);
    //Set font for values printing, set color.
    painter->setFont(QFont("courrier", 80));
    painter->setPen(Qt::black);

    //compute 64 squares
    for(int i = 0; i < 64; i++){
        int x = 500 * (i % 8);
        int y = 500 * (i / 8);
        int pix = 255 - 4*i;
        //Create a square with grey in it.
        painter->fillRect(x, y, 500, 500, QColor(pix, pix, pix));

        if(i == 32) painter->setPen(Qt::white);
        //Write current value to low right corner of the square just done.
        painter->drawText(x + 250, y + 450, QString::number(4 * i));
    }

    //Print dpi value to the top left corner of the chart.
    painter->setPen(Qt::black);
    painter->drawText(50, 150, QString::number(dpi) + " dpi");

    //Delete painter object
    delete painter;

    //resize the image to the dpi ratio needed, set the dpi ratio so it's usable after.
    int newSize = sizeMm * dpi / (float)25.4;
    chart.setDotsPerMeterX(dpi * (1000/25.4));
    chart.setDotsPerMeterY(dpi * (1000/25.4));
    //Then send it back to caller.
    return chart.scaled(newSize, newSize);
}

void Tools::sight(){

}
