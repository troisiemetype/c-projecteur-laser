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

#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "serialconfig.h"

using namespace std;


class Serial
{

public:
    Serial();
    Serial(QWidget *parent);
    ~Serial();

    bool open();
    void close();

    bool sendData(int);
    void sendSupport();

    void addCoord(string);
    void addBoxImage(QString);
    void addBoxSupport(QString);

    void emptyCoord();

    bool isOpen();
    bool isCompute();

    void initData();

    //getters for used values
    vector<string>* getDataArray();
    vector<QString>* getBoxImageArray();
    vector<QString>* getBoxSupportArray();

    //getter for port list names
    static QStringList getPortNames();

public slots:
//    void readData();

private:
    QSerialPort *serial;
    SerialConfig serialConfig;

    vector<string> dataToSend;
    vector<QString> dataBoxImage;
    vector<QString> dataBoxSupport;

    bool opened;
    int dataSize;
    int currentCoord;

};

#endif // SERIAL_H
