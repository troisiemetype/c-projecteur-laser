#include "serialsettings.h"
#include "ui_serialsettings.h"

SerialSettings::SerialSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialSettings)
{
    ui->setupUi(this);

    serialConfig = new SerialConfig();

    ui->serialBaudrateLineEdit->addItems(serialConfig->getBaudrates());
    ui->serialDataBitsLineEdit->addItems(serialConfig->getDataBits());
    ui->serialParityLineEdit->addItems(serialConfig->getParities());
    ui->serialStopBitsLineEdit->addItems(serialConfig->getStopBits());
}

SerialSettings::~SerialSettings()
{
    delete ui;
}

void SerialSettings::on_serialUpdateList_clicked()
{

}

void SerialSettings::on_buttonBox_accepted()
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
