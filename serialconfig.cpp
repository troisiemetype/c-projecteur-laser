#include "serialconfig.h"

SerialConfig::SerialConfig()
{
    //définition of the "constants" used forthe lists.
    baudrates << "115200" << "57600" << "38400" << "19200" << "9600" << "4800" << "2400" << "1200" ;
    dataBits << "5" << "6" << "7" << "8";
    parities << "0" << "2" << "3";
    StopBits << "1" << "2";

    //reading the values from the config file.
    settings = new QSettings("serialconfiguration.ini", QSettings::IniFormat);

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

QStringList SerialConfig::getBaudrates()
{
    return baudrates;
}

QStringList SerialConfig::getDataBits()
{
    return dataBits;
}

QStringList SerialConfig::getParities()
{
    return parities;
}

QStringList SerialConfig::getStopBits()
{
    return StopBits;
}

