#ifndef SERIAL_H
#define SERIAL_H

//#include <QSerialPort>
#include <QString>

#include <vector>

using namespace std;


class Serial
{

public:
    Serial();
    ~Serial();
    void addCoord(QString);
    void addBoxImage();
    void addBoxSupport();

    void emptyCoord();

    vector<QString>* getDataArray();
    vector<QString>* getBoxImageArray();
    vector<QString>* getBoxSupportArray();

private:
    vector<QString> dataToSend;
    vector<QString> dataBoxImage;
    vector<QString> dataBoxSupport;


};

#endif // SERIAL_H
