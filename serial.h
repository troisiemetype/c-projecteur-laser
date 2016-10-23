#ifndef SERIAL_H
#define SERIAL_H

#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QStringList>

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
