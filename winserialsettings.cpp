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
