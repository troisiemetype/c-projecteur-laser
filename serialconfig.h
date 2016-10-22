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
