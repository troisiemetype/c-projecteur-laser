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

#ifndef SERIAL_H
#define SERIAL_H

#include <QByteArray>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QStringList>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "serialconfig.h"

using namespace std;


class Serial
{

public:
    Serial();
    ~Serial();

    bool open();
    void close();

    void sendData();
    void sendSupport();

    void addCoord(QString);
    void addBoxImage(QString);
    void addBoxSupport(QString);

    void emptyCoord();

    bool isOpen();
    bool isCompute();

    //getters for used values
    vector<QString>* getDataArray();
    vector<QString>* getBoxImageArray();
    vector<QString>* getBoxSupportArray();

    //getter for port list names
    static QStringList getPortNames();

private:
    QSerialPort *serial;
    SerialConfig serialConfig;

    vector<QString> dataToSend;
    vector<QString> dataBoxImage;
    vector<QString> dataBoxSupport;

    bool opened;
    int dataSize;

};

#endif // SERIAL_H
