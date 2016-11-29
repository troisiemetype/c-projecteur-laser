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

/*
 * This program is intended. This is the main program, that defines the GUI and ties its inputs and outputs
 * to the classes used.
 */

#include "projecteurlaser.h"
#include "ui_projecteurlaser.h"

//Contructor. Define GUI, give it a position. Create the serial object.
ProjecteurLaser::ProjecteurLaser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjecteurLaser)
{
    ui->setupUi(this);
    ui->centralWidget->hide();
    ui->progressBar->hide();
    ui->progressLabel->hide();

    move(50, 50);

    serial = Serial(this);

}

ProjecteurLaser::~ProjecteurLaser()
{
    delete ui;
}

//Open a new file. Init the image copies its values to the GUI fields.
void ProjecteurLaser::on_actionFileNew_triggered()
{
    //Open a file
    QString file = QFileDialog::getOpenFileName(this, tr("Open image file"),
                                                QString(),
                                                tr("Images (*.png *.jpg *.jpeg *.bmp)"));

    //Check if a file has been selected
    if(file.isNull())
    {
        return;
    }

    //Check extenion of the file. Behave differently if it's a bmp or a svg.
    //Image managment and computing are not the same for both.
    if(file.section('.', 1, 1) == "svg"){
        typeFichier = "svg";
    } else {
        typeFichier = "bmp";

        //Create the image object, get it's pixmap
        image = Image(file);
        computeImage = ComputeImage(image);

        ui->imageLabel->setPixmap(image.getPixmap());

        //And get all the values to be displayed in the GUI.
        ui->widthPixelsLabelEdit->setText(QString::number(image.getWidthPix()));
        ui->heightPixelsLabelEdit->setText(QString::number(image.getHeightPix()));
        ui->widthMmLineEdit->setText(QString::number(image.getWidthMm()));
        ui->heightMmLineEdit->setText(QString::number(image.getHeightMm()));
        ui->supportWidthLineEdit->setText(QString::number(image.getSupportWidth()));
        ui->supportHeightLineEdit->setText(QString::number(image.getSupportHeight()));
        ui->distanceLineEdit->setText(QString::number(image.getDistance()));
        ui->speedLineEdit->setText(QString::number(image.getSpeed()));

        ui->modeComboBox->addItem("Points");
        ui->modeComboBox->addItem("Points and time");
        ui->modeComboBox->addItem("Routes");

        ui->imageModeComboBox->addItem("Grayscale");
        ui->imageModeComboBox->addItem("Floyd-Steinberg");
        ui->imageModeComboBox->addItem("Threshold");

    }

    //Last, enable buttons for calibrating and computing, and show the image and values area.
    ui->actionImageCalibrate->setEnabled(true);
    ui->actionImageCompute->setEnabled(true);

    ui->centralWidget->show();
}

void ProjecteurLaser::on_actionFileOpen_triggered()
{

}

void ProjecteurLaser::on_actionFileSave_triggered()
{

}

//Close the current file. Hide buttons that shouldn't be clicked when no file is open.
void ProjecteurLaser::on_actionFileClose_triggered()
{
    //Hide the image and values area, hide calibrate, compute and send buttons.
    ui->centralWidget->hide();

    ui->actionImageCalibrate->setEnabled(false);
    ui->actionImageCompute->setEnabled(false);
    ui->actionSendData->setEnabled(false);

}

//Connect the UART.
void ProjecteurLaser::on_actionSerialConnect_triggered()
{
    if(ui->actionSerialConnect->isChecked())
    {
        int ouvert = serial.open();
        if(!ouvert){
            ui->actionSerialConnect->setChecked(false);
        }
        if(ouvert && serial.isCompute()){
            ui->actionSendData->setEnabled(true);
            ui->actionSendData->setChecked(false);
        }
    } else {
        serial.close();
        ui->actionSendData->setEnabled(false);
        ui->actionSendData->setChecked(false);
    }

}

//Disconnect the UART.
void ProjecteurLaser::on_actionSerialDisconnect_triggered()
{
    serial.close();
}

//Open the UART settings.
void ProjecteurLaser::on_actionSerialSettings_triggered()
{
    WinSerialSettings winSerialSettings(this);
    winSerialSettings.exec();
}

void ProjecteurLaser::on_actionHelp_triggered()
{

}

void ProjecteurLaser::on_actionAbout_triggered()
{

}

//Compute the image coordinates for laser.
void ProjecteurLaser::on_actionImageCompute_triggered()
{
    ui->actionImageCalibrate->setChecked(false);
    ui->actionSendData->setChecked(false);

    //Create a new image object.
    computeImage = ComputeImage(image);

    //Empty the serial buffer.
    serial.emptyCoord();

    //Show the progressbar area.
    ui->progressLabel->show();
    ui->progressLabel->setText(tr("Calcul en cours..."));
    ui->progressBar->show();

    computeImage.setScanAngle(ui->angleSpinBox->value());

    //Call the computeCoord function, give it a pointer to the progressbar.
    computeImage.computeCoords(serial.getDataArray(), ui->progressBar);

    //Hide the progressbar area.
    ui->progressLabel->hide();
    ui->progressBar->hide();

    //If serial is open, show send button.
    if(serial.isOpen()){
        ui->actionSendData->setEnabled(true);
    }
}

//compute and send a calibration rectangle.
void ProjecteurLaser::on_actionImageCalibrate_triggered(bool checked)
{
    if(checked)
    {
        ui->infosWidget->setEnabled(false);
        computeImage.computeSupport(serial.getBoxSupportArray());
        serial.initData();
        serial.sendSupport();
    } else {
        ui->infosWidget->setEnabled(true);
    }
}

//Send the data to the laser.
void ProjecteurLaser::on_actionSendData_triggered(bool checked)
{
    if(checked)
    {
        ui->infosWidget->setEnabled(false);
        serial.initData();
        serial.sendData(0);
    } else {
        ui->infosWidget->setEnabled(true);

    }
}

//Update the width of the support.
void ProjecteurLaser::on_supportWidthLineEdit_editingFinished()
{
    //Record the new value, hide send button, update the computeImage object
    image.setSupportWidth(ui->supportWidthLineEdit->text().toInt());
//    QString str = QString::number(image.getSupportHeight());
    ui->actionSendData->setEnabled(false);

    computeImage = ComputeImage(image);
}

//Update the height of the support.
void ProjecteurLaser::on_supportHeightLineEdit_editingFinished()
{
    //Record the new value, hide send button, update the computeImage object
    image.setSupportHeight(ui->supportHeightLineEdit->text().toInt());
//    QString str = QString::number(image.getSupportWidth());
    ui->actionSendData->setEnabled(false);

    computeImage = ComputeImage(image);
}

//Update the distance from laser to wall.
void ProjecteurLaser::on_distanceLineEdit_editingFinished()
{
    image.setDistance(ui->distanceLineEdit->text().toInt());
    ui->actionSendData->setEnabled(false);

}

//Update the speed.
void ProjecteurLaser::on_speedLineEdit_editingFinished()
{
    image.setSpeed(ui->speedLineEdit->text().toInt());
    ui->actionSendData->setEnabled(false);
}

//Temporary: Change the image mode.
//TODO: but this settings in a dropdown menu, possibly in an image settings window.
void ProjecteurLaser::on_imageModeComboBox_currentIndexChanged(int index)
{
    if(index == 2){
        ui->stepSlider->setVisible(true);
    } else {
        ui->stepSlider->setVisible(false);
    }

    image.setImageMode(index);

    ui->imageLabel->setPixmap(image.getPixmap());
    computeImage = ComputeImage(image);
}

//Set the step between black and white for thresold.
//TODO: see if we keep this or no.
void ProjecteurLaser::on_stepSlider_valueChanged(int value)
{
    int seuil = value;
    image.setStep(seuil);
    image.setImageMode(ui->imageModeComboBox->currentIndex());
    ui->imageLabel->setPixmap(image.getPixmap());
}

//Update the width of the picture.
void ProjecteurLaser::on_widthMmLineEdit_editingFinished()
{
    //Store new value, edit height that is linked.
    image.setImageWidth(ui->widthMmLineEdit->text().toInt());
    ui->heightMmLineEdit->setText(QString::number(image.getHeightMm()));

    //Update the computeImage object.
    computeImage = ComputeImage(image);
}

//Update the height of the picture.
void ProjecteurLaser::on_heightMmLineEdit_editingFinished()
{
    //Store new value, edit width that is linked.
    image.setImageHeight(ui->heightMmLineEdit->text().toInt());
    ui->widthMmLineEdit->setText(QString::number(image.getWidthMm()));

    //Update the computeImage object.
    computeImage = ComputeImage(image);
}

void ProjecteurLaser::readData(){
//    cout << "data received" << endl;

    if(ui->actionSendData->isChecked()){
        if(serial.sendData(1) == false){
            ui->actionSendData->setChecked(false);
        }
    } else if(ui->actionImageCalibrate->isChecked()){
        serial.sendSupport();
    }

}

void ProjecteurLaser::on_modeComboBox_currentIndexChanged(int index)
{
    image.setMode(index);
}
