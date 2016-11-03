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
    serial = new QSerialPort();
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
void Serial::sendData()
{
/*    string adresse = "sortie_serial.txt";
    ofstream sortieSerial(adresse);

    if(!sortieSerial)
    {
        WinInfo::info("ouverture impossible");
    }
*/

    dataSize = dataToSend.size();

    for(int i = 0; i < dataSize; i++){
        QByteArray strToSend = QByteArray();
        strToSend.append(dataToSend.at(i));
        serial->write(strToSend);
    }
}

//Send the rectangle area of the support.
void Serial::sendSupport()
{
    for(int j = 0; j < 200 ; j++)
    {
        for(int i = 0; i < 4; i++){
            QByteArray strToSend = QByteArray();
            strToSend.append(dataBoxSupport.at(i));
            serial->write(strToSend);
        }
    }

}

void Serial::addCoord(QString coord)
{
    dataToSend.push_back(coord);
}

void Serial::addBoxImage(QString coord)
{
    dataBoxImage.push_back(coord);
}

void Serial::addBoxSupport(QString coord)
{
    dataBoxSupport.push_back(coord);
}

void Serial::emptyCoord()
{
    dataToSend.clear();
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
