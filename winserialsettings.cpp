#include "winserialsettings.h"
#include "ui_winserialsettings.h"

WinSerialSettings::WinSerialSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WinSerialSettings)
{
    ui->setupUi(this);

    serialConfig = new SerialConfig();

    ui->serialPortLineEdit->addItem(serialConfig->getString("port"));

    ui->serialBaudrateLineEdit->addItems(serialConfig->getList("baudrates"));
    ui->serialBaudrateLineEdit->setCurrentIndex(serialConfig->getIndex("baudrate"));

    ui->serialDataBitsLineEdit->addItems(serialConfig->getList("databits"));
    ui->serialDataBitsLineEdit->setCurrentIndex(serialConfig->getIndex("databits"));

    ui->serialParityLineEdit->addItems(serialConfig->getList("parities"));
    ui->serialParityLineEdit->setCurrentIndex(serialConfig->getIndex("parity"));

    ui->serialStopBitsLineEdit->addItems(serialConfig->getList("stopbits"));
    ui->serialStopBitsLineEdit->setCurrentIndex(serialConfig->getIndex("stopbits"));

    ui->serialFlowControlXonLineEdit->setChecked(serialConfig->getBool("flowcontrolxon"));
}

WinSerialSettings::~WinSerialSettings()
{
    delete ui;
}

void WinSerialSettings::on_serialUpdateList_clicked()
{
    QStringList portNames = Serial::getPortNames();
    ui->serialPortLineEdit->clear();
    ui->serialPortLineEdit->addItems(portNames);
}

void WinSerialSettings::on_buttonBox_accepted()
{
    //Save the dilaog to the configuration file
    //port, baudrate, databits, parity, stop bits, xon
    serialConfig->save(ui->serialPortLineEdit->currentText(),
                       ui->serialBaudrateLineEdit->currentText(),
                       ui->serialDataBitsLineEdit->currentText(),
                       ui->serialParityLineEdit->currentText(),
                       ui->serialStopBitsLineEdit->currentText(),
                       ui->serialFlowControlXonLineEdit->isChecked());
}
