#include "serial.h"

Serial::Serial()
{
    serial = new QSerialPort();
    opened = false;
}

Serial::~Serial()
{

}

bool Serial::open()
{
    serialConfig = SerialConfig();

    serial->setPortName(serialConfig.getString("port"));
    serial->setBaudRate(57600);
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

void Serial::sendData()
{
/*    string adresse = "sortie_serial.txt";
    ofstream sortieSerial(adresse);

    if(!sortieSerial)
    {
        WinInfo::info("ouverture impossible");
    }
*/

 /*   dataSize = dataToSend.size();

    for(int i = 0; i < dataSize; i++){
        string strToSend = dataToSend.at(i).toStdString();
        char *charToSend = new char[strToSend.size() + 1];
        strcpy(charToSend, strToSend.c_str());
        serial->write(charToSend);

//        sortieSerial << charToSend;

    }
    */
    for(int i = 0; i < dataSize; i++){
        QByteArray strToSend = QByteArray();
        strToSend.append(dataToSend.at(i));
        serial->write(strToSend);
    }

//    WinInfo::info("Image sent...");
}

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
