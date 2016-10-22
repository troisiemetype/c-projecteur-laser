#include "serial.h"

Serial::Serial()
{
    serial = new QSerialPort();
}

Serial::~Serial()
{

}

bool Serial::open()
{
    serialConfig = SerialConfig();

    serial->setPortName(serialConfig.getString("port"));
    serial->setBaudRate(115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    if(serialConfig.getBool("flowcontrolxon")){
        serial->setFlowControl(QSerialPort::SoftwareControl);
    }

    if(serial->open(QIODevice::ReadWrite))
    {
        return true;
    } else {
        return false;
    }

}

void Serial::close()
{
    serial->close();
}

void Serial::sendData()
{

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
