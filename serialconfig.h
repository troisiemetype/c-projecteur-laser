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

#ifndef SERIALCONFIG_H
#define SERIALCONFIG_H

#include <QRegExp>
#include <QSerialPort>
#include <QSettings>
#include <QStringList>

#include <iostream>

#include "wininfo.h"

using namespace std;

class SerialConfig
{
public:
    SerialConfig();

    void read();
    void save(QString, QString, QString, QString, QString, bool);

    //getters for availables values
    QString getString(QString);
    int getIndex(QString);
    int getValue(QString);
    bool getBool(QString);

    QStringList getList(QString);

private:
    QSettings *settings;
    QStringList baudrates;
    QStringList dataBits;
    QStringList parities;
    QStringList stopBits;
    bool flowControl;

    QString activePort;
    int activeBaudrate;
    int activeDataBits;
    int activeParity;
    int activeStopBits;
    bool activeFlowControl;

};

#endif // SERIALCONFIG_H
