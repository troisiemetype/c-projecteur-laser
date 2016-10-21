#include "serial.h"

Serial::Serial()
{

}

Serial::~Serial()
{

}

void Serial::addCoord(QString coord)
{
    dataToSend.push_back(coord);
}

void addBoxImage()
{

}

void addBoxSupport()
{

}

void Serial::emptyCoord()
{
    dataToSend.clear();
}

vector<QString>* Serial::getDataArray()
{
    return &dataToSend;
}

vector<QString>* Serial::getBoxImageArray()
{
    return &dataBoxImage;
}

vector<QString>* Serial::getBoxSupportArray()
{
    return &dataBoxSupport;
}

