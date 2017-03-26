#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>

#include <QFont>
#include <QImage>
#include <QPainter>
#include <QStaticText>


class Tools
{
public:
    Tools();

    static QImage greyChart(const int& dpi, const int& sizeMm);
    void sight();
};

#endif // TOOLS_H
