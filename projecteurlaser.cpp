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
 * This is the main program, that defines the GUI and ties its inputs and outputs
 * to the classes used.
 * It contains all the slots for GUI management
 */

#include "projecteurlaser.h"
#include "ui_projecteurlaser.h"

//Contructor. Define GUI, give it a position.
ProjecteurLaser::ProjecteurLaser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjecteurLaser)
{

    computeImage = NULL;
    audio = NULL;

    ui->setupUi(this);
    ui->centralWidget->hide();
    ui->progressBar->hide();
    ui->progressLabel->hide();

    ui->imageModeComboBox->addItem(tr("Grayscale"));
    ui->imageModeComboBox->addItem(tr("Floyd-Steinberg"));
    ui->imageModeComboBox->addItem(tr("Threshold"));

    repeat = 0;

    move(50, 50);

    computeImage = new ComputeImage();
    audio = new Audio();

    connect(audio, SIGNAL(stopped()), this, SLOT(handleAudioStopped()));
    connect(this, SIGNAL(exposureChanged(int)), audio, SLOT(handleExposureChanged(int)));
    connect(computeImage, SIGNAL(progressing(int)), this, SLOT(handleProgress(int)));
    connect(audio, SIGNAL(progressing(int)), this, SLOT(handleProgress(int)));

}

ProjecteurLaser::~ProjecteurLaser()
{
    delete ui;
}

void ProjecteurLaser::enableSends(bool state)
{
    ui->actionSend->setEnabled(state);
    ui->actionPause->setEnabled(state);
    ui->actionStop->setEnabled(state);
    ui->actionImageCalibrate->setEnabled(state);

}

void ProjecteurLaser::populateGui()
{
    ui->imageLabel->setPixmap(image.getPixmap());

    //And get all the values to be displayed in the GUI.
    ui->widthPixelsLabelEdit->setText(QString::number(image.getWidthPix()));
    ui->heightPixelsLabelEdit->setText(QString::number(image.getHeightPix()));
    ui->widthMmLineEdit->setText(QString::number(image.getWidthMm()));
    ui->heightMmLineEdit->setText(QString::number(image.getHeightMm()));
    ui->supportWidthLineEdit->setText(QString::number(image.getSupportWidth()));
    ui->supportHeightLineEdit->setText(QString::number(image.getSupportHeight()));
    ui->distanceLineEdit->setText(QString::number(image.getDistance()));
    ui->resolutionLabelEdit->setText(QString::number(computeImage->getDpi()));

    ui->stepSlider->setVisible(false);

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
    if(file.section('.', 1) == "svg"){
        typeFichier = "svg";
    } else {
        typeFichier = "bmp";

        //Create the image object, get it's pixmap
        image = Image(file);
        computeImage->init(image);

        populateGui();
    }

    //Last, enable buttons for calibrating and computing, and show the image and values area.
    ui->actionImageCompute->setEnabled(true);

    ui->centralWidget->show();

}

void ProjecteurLaser::on_actionFileOpen_triggered()
{

}

void ProjecteurLaser::on_actionFileSave_triggered()
{
    QString file = QFileDialog::getSaveFileName(this, tr("save as"),
                                                QString(), tr("sound (*.wav)"));
    QString fileExt = file.section('.', 1);

    if (fileExt.isEmpty()){
        file.append(".wav");
    } else if (fileExt != "wav" && fileExt != "wave"){
        WinInfo::error(tr("Wrong file format."));
        return;
    }

//    cout << file.toStdString() << endl;

    audio->save(file);
}

//Close the current file. Hide buttons that shouldn't be clicked when no file is open.
void ProjecteurLaser::on_actionFileClose_triggered()
{
    //Hide the image and values area, hide calibrate, compute and send buttons.
    ui->centralWidget->hide();

    ui->actionImageCompute->setEnabled(false);
    enableSends(false);
    audio->clearCoords();
    audio->clearSupport();
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
    ui->actionSend->setChecked(false);
    ui->infosWidget->setEnabled(true);

    //Create a new image object.
    computeImage->init(image);

    //Show the progressbar area.
    ui->progressLabel->show();
    ui->progressLabel->setText(tr("Processing..."));
    ui->progressBar->setMaximum(100);
    ui->progressBar->setFormat("%p%");
    ui->progressBar->show();

    computeImage->setScanAngle(ui->angleSpinBox->value());

    //Call the computeCoord function, give it a pointer to the audio processor.
    computeImage->computeCoords(audio);
    audio->stop();

    //Hide the progressbar area.
    ui->progressLabel->hide();
    ui->progressBar->hide();
    ui->actionImageCalibrate->setEnabled(true);
    enableSends(true);
    ui->actionSend->setEnabled(true);
    QString text = tr("Insolation time: ");
    text += QString::number(audio->getLength());
    text += tr(" seconds.");
    this->statusBar()->showMessage(text);
//    cout << "durée insolation: " << audio->getLength() << endl;
}

//compute and send a calibration rectangle.
void ProjecteurLaser::on_actionImageCalibrate_triggered(bool checked)
{
    if(checked)
    {
        ui->infosWidget->setEnabled(false);
        enableSends(false);
        ui->actionImageCalibrate->setEnabled(true);
        computeImage->computeSupport();
        audio->playSupport();
    } else {
        ui->infosWidget->setEnabled(true);
        enableSends(true);
        audio->stopSupport();
    }
}

//Update the width of the support.
void ProjecteurLaser::on_supportWidthLineEdit_editingFinished()
{
    //Record the new value, hide send button, update the computeImage object
    image.setSupportWidth(ui->supportWidthLineEdit->text().toInt());
    enableSends(false);

    computeImage->init(image);
}

//Update the height of the support.
void ProjecteurLaser::on_supportHeightLineEdit_editingFinished()
{
    //Record the new value, hide send button, update the computeImage object
    image.setSupportHeight(ui->supportHeightLineEdit->text().toInt());
    enableSends(false);

    computeImage->init(image);
}

//Update the distance from laser to wall.
void ProjecteurLaser::on_distanceLineEdit_editingFinished()
{
    image.setDistance(ui->distanceLineEdit->text().toInt());
    enableSends(false);

}

//Temporary: Change the image mode.
//TODO: but this settings in a dropdown menu, possibly in an image settings window.
void ProjecteurLaser::on_imageModeComboBox_currentIndexChanged(int index)
{
    if (computeImage == NULL) return;
    if(index == 2){
        ui->stepSlider->setVisible(true);
    } else {
        ui->stepSlider->setVisible(false);
    }

    image.setImageMode(index);
    computeImage->init(image);

    ui->imageLabel->setPixmap(image.getPixmap());
    enableSends(false);
}

//Set the step between black and white for thresold.
void ProjecteurLaser::on_stepSlider_valueChanged(int value)
{
    int seuil = value;
    image.setStep(seuil);
    image.setImageMode(ui->imageModeComboBox->currentIndex());
    ui->imageLabel->setPixmap(image.getPixmap());
    enableSends(false);
}

//Update the width of the picture.
void ProjecteurLaser::on_widthMmLineEdit_editingFinished()
{
    //Store new value, edit height that is linked.
    image.setImageWidth(ui->widthMmLineEdit->text().toInt());
    ui->heightMmLineEdit->setText(QString::number(image.getHeightMm()));

    //Update the computeImage object.
    computeImage->init(image);
    ui->resolutionLabelEdit->setText(QString::number(computeImage->getDpi()));
    enableSends(false);
}

//Update the height of the picture.
void ProjecteurLaser::on_heightMmLineEdit_editingFinished()
{
    //Store new value, edit width that is linked.
    image.setImageHeight(ui->heightMmLineEdit->text().toInt());
    ui->widthMmLineEdit->setText(QString::number(image.getWidthMm()));

    //Update the computeImage object.
    computeImage->init(image);
    ui->resolutionLabelEdit->setText(QString::number(computeImage->getDpi()));
    enableSends(false);
}

void ProjecteurLaser::on_actionGrayScale_triggered()
{
    bool ok = false;
    int dpi = QInputDialog::getInt(this, tr("Gray scale"), "dpi", 300, 0, 2400, 1, &ok);

    if(!ok){return;}

    image = Image(dpi);
    computeImage->init(image);

    populateGui();

    //Last, enable buttons for calibrating and computing, and show the image and values area.
    ui->actionImageCompute->setEnabled(true);
    ui->centralWidget->show();

}

void ProjecteurLaser::on_actionSend_triggered(bool checked)
{
    if(checked)
    {
        //Show the progressbar area.
        ui->progressLabel->setText(tr("Insolating..."));
        ui->progressLabel->show();
        ui->progressBar->setMaximum(audio->getLength());
        ui->progressBar->setValue(0);
        ui->progressBar->setFormat(tr("%v/%m seconds"));
        ui->progressBar->show();

        ui->infosWidget->setEnabled(false);
        enableSends(true);
        audio->play(repeat);
    } else {
        //hide the progressbar area.
        ui->progressLabel->hide();
        ui->progressBar->hide();
        audio->stop();
    }
}

void ProjecteurLaser::on_actionPause_triggered(bool checked)
{
    if(!ui->actionSend->isChecked()){
        ui->actionPause->setChecked(false);
        return;
    }

    if(checked)
    {
        ui->actionPause->setText(tr("resume"));
        audio->pause(true);
    } else {
        ui->actionPause->setText(tr("pause"));
        audio->pause(false);
    }

}

void ProjecteurLaser::on_actionStop_triggered()
{
    //hide the progressbar area.
    ui->progressLabel->hide();
    ui->progressBar->hide();

    audio->stop();
}

void ProjecteurLaser::handleAudioStopped(){
    ui->infosWidget->setEnabled(true);
    ui->actionSend->setChecked(false);
    ui->actionPause->setChecked(false);

    //hide the progressbar area.
    ui->progressLabel->hide();
    ui->progressBar->hide();

}

void ProjecteurLaser::handleProgress(int value){
    ui->progressBar->setValue(value);
}

void ProjecteurLaser::on_angleSpinBox_valueChanged(int arg1)
{
    enableSends(false);
}

void ProjecteurLaser::on_exposureSlider_sliderMoved(int position)
{
    QString text = (tr("Exposure: "));
    text += QString::number(position);
    text += "%";
    ui->speedLabel->setText(text);
    enableSends(false);

    emit exposureChanged(position);
}

void ProjecteurLaser::on_repeatSpinBox_valueChanged(int arg1)
{
    repeat = arg1;
    enableSends(false);
}

void ProjecteurLaser::on_offsetXSlider_valueChanged(int value)
{
    QString text = (tr("Offset width: "));
    text += QString::number(value);
    text += "%";
    ui->offsetXLabel->setText(text);
    computeImage->setOffsetX(value);
    enableSends(false);
}

void ProjecteurLaser::on_offsetYSlider_valueChanged(int value)
{
    QString text = (tr("Offset heigth: "));
    text += QString::number(value);
    text += "%";
    ui->offsetYLabel->setText(text);
    computeImage->setOffsetY(value);
    enableSends(false);
}

void ProjecteurLaser::on_jumpSpinBox_valueChanged(int arg1)
{
    computeImage->setJump(arg1);
    enableSends(false);
}

void ProjecteurLaser::on_resetButton_clicked()
{
    ui->repeatSpinBox->setValue(0);
    ui->angleSpinBox->setValue(0);
    ui->exposureSlider->setValue(100);
    ui->offsetXSlider->setValue(0);
    ui->offsetYSlider->setValue(0);
    ui->jumpSpinBox->setValue(0);
}

void ProjecteurLaser::on_actionResample_triggered()
{
    bool ok = false;
    int dpi = QInputDialog::getInt(this, tr("Re-sample"), "dpi", image.getDpi(), 0, 2400, 1, &ok);

    if(!ok){return;}

    image.resample(dpi);

    computeImage->init(image);

    populateGui();

    ui->actionImageCompute->setEnabled(true);
    ui->centralWidget->show();
}
