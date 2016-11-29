/*
 * This program is intended to control a laser projector
 * Copyright (C) 2016  Pierre-Loup Martin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "serial.h"

//Create a new serial object.
Serial::Serial()
{
//    serial = new QSerialPort();

//    opened = false;
}

//Create a new serial object.
Serial::Serial(QWidget *parent)
{
    serial = new QSerialPort(parent);
    QObject::connect(serial, SIGNAL(readyRead()), parent, SLOT(readData()));

    opened = false;
}

Serial::~Serial()
{

}

//Try to open a serial link.
//TODO: it should use the values from the .INI file.
bool Serial::open()
{
    serialConfig = SerialConfig();

    serial->setPortName(serialConfig.getString("port"));
    serial->setBaudRate(115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    if(serialConfig.getBool("flowcontrolxon")){
        serial->setFlowControl(QSerialPort::SoftwareControl);
    } else {
        serial->setFlowControl(QSerialPort::NoFlowControl);
    }

    if(serial->open(QIODevice::ReadWrite))
    {
        opened = true;
        return true;
    } else {
        opened = false;
        return false;
    }

}

void Serial::close()
{
    opened = false;
    serial->close();
}

//Send the data to the laser.
//TODO: see how to make this realtime (so we can see a progressbar and diplay infos at the end)
//      Maybe (probably) use some multithreading.
bool Serial::sendData(int mode)
{

    char *inData = new char();
    serial->read(inData, 1);
//    cout << bitset<8>(*inData) << endl;

    if(mode == 1 && *inData == 0){
        currentCoord++;
    }

    if(currentCoord >= dataSize){
        return false;
    }

    serial->write(dataToSend.at(currentCoord));

    return true;
}

//Send the rectangle area of the support.
void Serial::sendSupport()
{
    char *inData = new char();
    serial->read(inData, 1);
//    cout << bitset<8>(*inData) << endl;

    if(*inData == 0){
        currentCoord++;
    }

    if(currentCoord >= 4){
        currentCoord = 0;
    }
    serial->write(dataBoxSupport.at(currentCoord));
}

void Serial::addCoord(string coord)
{
//    dataToSend.push_back(coord);
}

void Serial::addBoxImage(QByteArray coord)
{
    dataBoxImage.push_back(coord);
}

void Serial::addBoxSupport(QByteArray coord)
{
    dataBoxSupport.push_back(coord);
}

void Serial::emptyCoord()
{
    dataToSend.clear();
    dataBoxImage.clear();
    dataBoxSupport.clear();
}

bool Serial::isOpen()
{
    return opened;
}

bool Serial::isCompute()
{
    if(dataToSend.size() == 0)
    {
        return 0;
    } else {
        return 1;
    }

}

vector<QByteArray>* Serial::getDataArray()
{
    return &dataToSend;
}

vector<QByteArray>* Serial::getBoxImageArray()
{
    return &dataBoxImage;
}

vector<QByteArray>* Serial::getBoxSupportArray()
{
    return &dataBoxSupport;
}

QStringList Serial::getPortNames()
{
    QList<QSerialPortInfo> available = QSerialPortInfo::availablePorts();
    QStringList portNames = QStringList();

    int listSize = available.size();

    for(int i = 0; i < listSize; i++){
        portNames << available.value(i).portName();
    }

    return portNames;
}

void Serial::initData(){
    serial->clear(QSerialPort::AllDirections);
    dataSize = dataToSend.size();
    currentCoord = 0;
//    cout << "dataSize: " << dataSize << endl;

}
