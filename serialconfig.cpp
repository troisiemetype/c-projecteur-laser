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

#include "serialconfig.h"

SerialConfig::SerialConfig()
{
    //définition of the "constants" used for the lists.
    baudrates << "115200" << "57600" << "38400" << "19200" << "9600" << "4800" << "2400" << "1200" ;
    dataBits << "5" << "6" << "7" << "8";
    parities << "0" << "2" << "3";
    stopBits << "1" << "2";

    //reading the values from the config file.
    settings = new QSettings("serialconfiguration.ini", QSettings::IniFormat);

    read();

}

void SerialConfig::read()
{
    activePort = settings->value("port").toString();
    activeBaudrate = baudrates.indexOf(settings->value("baudrate").toString());
    activeDataBits = dataBits.indexOf(settings->value("databits").toString());
    activeParity = parities.indexOf(settings->value("parity").toString());
    activeStopBits = stopBits.indexOf(settings->value("stopbits").toString());
    activeFlowControl = settings->value("xon").toBool();

}

void SerialConfig::save(QString port, QString baudrate, QString dataBit,
                        QString parity, QString stopBit, bool flowControlXon)
{
    settings->setValue("port", port);
    settings->setValue("baudrate", baudrate);
    settings->setValue("databits", dataBit);
    settings->setValue("parity", parity);
    settings->setValue("stopbits", stopBit);
    settings->setValue("xon", flowControlXon);
}

QString SerialConfig::getString(QString value)
{
    if(value == "port"){
        return settings->value("port").toString();
    } else {
        return "";
    }
}

int SerialConfig::getIndex(QString value)
{
    if(value == "baudrate")
    {
         return baudrates.indexOf(settings->value("baudrate").toString());

    } else if(value == "databits"){
        return dataBits.indexOf(settings->value("databits").toString());

    } else if(value == "parity"){
        return parities.indexOf(settings->value("parity").toString());

    } else if(value == "stopbits"){
        return stopBits.indexOf(settings->value("stopbits").toString());

    } else {
        return 0;
    }

}

int SerialConfig::getValue(QString value)
{
    if(value == "baudrate")
    {
         return 0;

    } else if(value == "databits"){
        return 0;

    } else if(value == "parity"){
        return 0;

    } else if(value == "stopbits"){
        return 0;

    } else {
        return 0;
    }

}

bool SerialConfig::getBool(QString value)
{
    if(value == "flowcontrolxon"){
        return settings->value("xon").toBool();
    } else {
        return false;
    }
}

QStringList SerialConfig::getList(QString value)
{
    if(value == "baudrates")
    {
         return baudrates;

    } else if(value == "databits"){
        return dataBits;

    } else if(value == "parities"){
        return parities;

    } else if(value == "stopbits"){
        return stopBits;

    } else {
        return QStringList();
    }

}
