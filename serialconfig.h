#ifndef SERIALCONFIG_H
#define SERIALCONFIG_H

#include <QSerialPort>
#include <QSettings>
#include <QStringList>

using namespace std;

class SerialConfig
{
public:
    SerialConfig();

    void save(QString, QString, QString, QString, QString, bool);

    //getters for availables values
    QStringList getBaudrates();
    QStringList getDataBits();
    QStringList getParities();
    QStringList getStopBits();

    //getters from the set values
    QString getPort();
    int getBaudrate();
    int getDataBit();
    int getParity();
    int getStopBit();
    bool getFloControlXon();

private:
    QSettings *settings;
    QStringList baudrates;
    QStringList dataBits;
    QStringList parities;
    QStringList StopBits;

};

#endif // SERIALCONFIG_H
