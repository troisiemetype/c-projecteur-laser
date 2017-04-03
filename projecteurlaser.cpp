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

//Constructor. Define GUI.
ProjecteurLaser::ProjecteurLaser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjecteurLaser)
{
    //Set pointers to NULL
    //Image and ComputeImage are created each time a file is opened
    //Deleted each time it's closed, or another file reopened.
    //Audio is created by Image, and pass to ProjecteurLaser, that's why it's not newed or deleted in this class.
    image = NULL;
    computeImage = NULL;
    audio = NULL;

    ui->setupUi(this);
    ui->centralWidget->hide();
    ui->progressBar->hide();
    ui->progressLabel->hide();
    ui->actionCalibrate_angles->setEnabled(false);

    ui->imageModeComboBox->addItem(tr("Grayscale"));
    ui->imageModeComboBox->addItem(tr("Floyd-Steinberg"));
    ui->imageModeComboBox->addItem(tr("Threshold"));

    repeat = 0;

    settings = new QSettings("settings.ini", QSettings::IniFormat);
    readSettings();
}

//Save settings and delete setting object before to close.
ProjecteurLaser::~ProjecteurLaser()
{
    saveSettings();
    on_actionFileClose_triggered();
//    delete audio;
    delete settings;
}

void ProjecteurLaser::newFile()
{
    //Create and init ComputeImage
    computeImage = new ComputeImage(image);
    computeImage->setDistance(ui->distanceLineEdit->text().toInt());
    computeImage->update();

    //Get pointer to Audio
    audio = image->getAudio();

    //Set connection between signals and slots.
    connect(computeImage, SIGNAL(progressing(int)), this, SLOT(handleProgress(int)));
    connect(audio, SIGNAL(progressing(int)), this, SLOT(handleProgress(int)));
    connect(audio, SIGNAL(stopped()), this, SLOT(handleAudioStopped()));

    populateGui();
    computeImage->setRepeat(ui->repeatSpinBox->value());
    computeImage->setExposure(ui->exposureSlider->value());

    ui->exposureRatioValue->setText(QString::number(computeImage->getExposureRatio()));

    //Last, enable buttons for calibrating and computing, and show the image and values area.
    ui->actionImageCompute->setEnabled(true);

    ui->centralWidget->show();
    ui->actionCalibrate_angles->setEnabled(true);

}

void ProjecteurLaser::readSettings(){
    this->move(settings->value("gui/x-pos", 50).toInt(), settings->value("gui/y-pos", 50).toInt());
    ui->distanceLineEdit->setText(QString::number(settings->value("laser/distance").toInt()));
}

void ProjecteurLaser::saveSettings(){
    settings->beginGroup("gui");
    settings->setValue("x-pos", this->pos().x());
    settings->setValue("y-pos", this->pos().y());
    settings->endGroup();

    settings->beginGroup("laser");
    settings->setValue("distance", ui->distanceLineEdit->text());
    settings->endGroup();
/*
    setting->beginGroup("audio");
    settings->setValue("samplerate");
    settings->setValue("samplesize");
    settings->endGroup();
*/
}


void ProjecteurLaser::enableSends(bool state)
{
    ui->actionSend->setEnabled(state);
    ui->actionPause->setEnabled(state);
    ui->actionStop->setEnabled(state);
//    ui->actionImageCalibrate->setEnabled(state);

}

void ProjecteurLaser::populateGui()
{
    ui->imageLabel->setPixmap(image->getPixmap());

    //And get all the values to be displayed in the GUI.
    ui->widthPixelsLabelEdit->setText(QString::number(image->getWidthPix()));
    ui->heightPixelsLabelEdit->setText(QString::number(image->getHeightPix()));

    ui->widthMmLineEdit->setText(QString::number(computeImage->getWidthMm()));
    ui->heightMmLineEdit->setText(QString::number(computeImage->getHeightMm()));
    ui->supportWidthLineEdit->setText(QString::number(computeImage->getSupportWidth()));
    ui->supportHeightLineEdit->setText(QString::number(computeImage->getSupportHeight()));

    ui->resolutionLabelEdit->setText(QString::number(computeImage->getDpi()));

    ui->stepSlider->setVisible(false);
    ui->stepLabel->setVisible(false);

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

        //Verify that image isn't yet set, else delete it
        if(image){
            on_actionFileClose_triggered();
        }
        //Create the image object
        image = new Image(file);
        newFile();
        ui->actionImageCalibrate->setEnabled(true);
    }

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

    audio->save(file);
}

//Close the current file. Hide buttons that shouldn't be clicked when no file is open.
void ProjecteurLaser::on_actionFileClose_triggered()
{
    //Hide the image and values area, hide calibrate, compute and send buttons.
    ui->centralWidget->hide();
    ui->actionCalibrate_angles->setEnabled(false);

    ui->actionImageCompute->setEnabled(false);
    enableSends(false);
    ui->actionImageCalibrate->setEnabled(false);


    ui->actionImageCalibrate->setChecked(false);
    ui->actionSend->setChecked(false);
    ui->infosWidget->setEnabled(true);


    //Unset connection between signals and slots.
    if(computeImage){
        disconnect(computeImage, SIGNAL(progressing(int)), this, SLOT(handleProgress(int)));
        delete computeImage;
        computeImage = NULL;
    }
    if(audio){
        disconnect(audio, SIGNAL(progressing(int)), this, SLOT(handleProgress(int)));
        disconnect(audio, SIGNAL(stopped()), this, SLOT(handleAudioStopped()));
        audio = NULL;
    }
    if(image){
        delete image;
        image = NULL;
    }
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

    computeImage->update();

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
    ui->actionCalibrate->setEnabled(true);
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
        computeImage->update();
        computeImage->computeSupport();
        audio->playSupport();
    } else {
        ui->infosWidget->setEnabled(true);
        enableSends(true);
        audio->stopSupport();
    }
}

//Update the width of the support.
void ProjecteurLaser::on_supportWidthLineEdit_textEdited(const QString &arg1)
{
    enableSends(false);
    //Record the new value, hide send button, update the computeImage object
    computeImage->setSupportWidth(arg1.toInt());

    computeImage->update();
}

//Update the height of the support.
void ProjecteurLaser::on_supportHeightLineEdit_textEdited(const QString &arg1)
{
    enableSends(false);
    //Record the new value, hide send button, update the computeImage object
    computeImage->setSupportHeight(arg1.toInt());

    computeImage->update();
}

//Update the distance from laser to wall.
void ProjecteurLaser::on_distanceLineEdit_textEdited(const QString &arg1)
{
    enableSends(false);

    computeImage->setDistance(arg1.toInt());
}

//Temporary: Change the image mode.
//TODO: but this settings in a dropdown menu, possibly in an image settings window.
void ProjecteurLaser::on_imageModeComboBox_currentIndexChanged(int index)
{
    if (computeImage == NULL) return;

    enableSends(false);

    if(index == Image::Threshold){
        ui->stepSlider->setVisible(true);
        ui->stepLabel->setVisible(true);
    } else {
        ui->stepSlider->setVisible(false);
        ui->stepLabel->setVisible(false);
    }

    image->setImageMode(index);
    computeImage->update();

    ui->imageLabel->setPixmap(image->getPixmap());
}

//Set the step between black and white for thresold.
void ProjecteurLaser::on_stepSlider_valueChanged(int value)
{
    enableSends(false);

    ui->stepLabel->setText(tr("Threshold: ") + QString::number(value));

    image->setStep(value);
    image->setImageMode(ui->imageModeComboBox->currentIndex());

    ui->imageLabel->setPixmap(image->getPixmap());
}

//Update the width of the picture.
void ProjecteurLaser::on_widthMmLineEdit_textEdited(const QString &arg1)
{
    enableSends(false);

    //Store new value, edit height that is linked.
    computeImage->setImageWidth(arg1.toInt());
    ui->heightMmLineEdit->setText(QString::number(computeImage->getHeightMm()));

    //Update the computeImage object.
    computeImage->update();
    ui->resolutionLabelEdit->setText(QString::number(computeImage->getDpi()));

    ui->exposureRatioValue->setText(QString::number(computeImage->getExposureRatio()));
}

//Update the height of the picture.
void ProjecteurLaser::on_heightMmLineEdit_textEdited(const QString &arg1)
{
    enableSends(false);

    //Store new value, edit width that is linked.
    computeImage->setImageHeight(arg1.toInt());
    ui->widthMmLineEdit->setText(QString::number(computeImage->getWidthMm()));

    //Update the computeImage object.
    computeImage->update();
    ui->resolutionLabelEdit->setText(QString::number(computeImage->getDpi()));

    ui->exposureRatioValue->setText(QString::number(computeImage->getExposureRatio()));

}

void ProjecteurLaser::on_actionGrayScale_triggered()
{
    bool ok = false;
    int dpi = QInputDialog::getInt(this, tr("Gray scale"), "dpi", 300, 0, 2400, 1, &ok);

    if(!ok){return;}

    if(image){
        on_actionFileClose_triggered();
    }

    //Create the image object
    image = new Image(Tools::greyChart(dpi, 80));

    newFile();
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

    audio->setExposure(position);
    computeImage->setExposure(position);

    ui->exposureRatioValue->setText(QString::number(computeImage->getExposureRatio()));
}

void ProjecteurLaser::on_repeatSpinBox_valueChanged(int arg1)
{
    repeat = arg1;
    computeImage->setRepeat(arg1);
    enableSends(false);

    ui->exposureRatioValue->setText(QString::number(computeImage->getExposureRatio()));
}

void ProjecteurLaser::on_offsetXSlider_valueChanged(int value)
{
    enableSends(false);

    QString text = (tr("Offset width: "));
    text += QString::number(value);
    text += "%";
    ui->offsetXLabel->setText(text);
    computeImage->setOffsetX(value);
}

void ProjecteurLaser::on_offsetYSlider_valueChanged(int value)
{
    enableSends(false);

    QString text = (tr("Offset heigth: "));
    text += QString::number(value);
    text += "%";
    ui->offsetYLabel->setText(text);
    computeImage->setOffsetY(value);
}

void ProjecteurLaser::on_jumpSpinBox_valueChanged(int arg1)
{
    enableSends(false);

    computeImage->setJump(arg1);
}

void ProjecteurLaser::on_resetButton_clicked()
{
    ui->repeatSpinBox->setValue(0);
    ui->angleSpinBox->setValue(0);
    ui->exposureSlider->setValue(100);
    ui->offsetXSlider->setValue(0);
    ui->offsetYSlider->setValue(0);
    ui->jumpSpinBox->setValue(0);
    ui->stepSlider->setValue(127);
}

void ProjecteurLaser::on_actionResample_triggered()
{
    bool ok = false;
    int dpi = QInputDialog::getInt(this, tr("Re-sample"), "dpi", image->getDpi(), 0, 2400, 1, &ok);

    if(!ok){return;}

    image->resample(dpi);

    computeImage->update();

    populateGui();

    ui->actionImageCompute->setEnabled(true);
    ui->centralWidget->show();
}

void ProjecteurLaser::on_actionCalibrate_triggered(bool checked)
{
    if(checked)
    {
        ui->infosWidget->setEnabled(false);
        enableSends(false);
        ui->actionImageCalibrate->setEnabled(true);
        computeImage->calibrate();
        audio->playSupport();
    } else {
        ui->infosWidget->setEnabled(true);
        enableSends(true);
        audio->stopSupport();
    }
}

void ProjecteurLaser::on_actionCalibrate_angles_triggered()
{
    SetupAngle setup(this);
    setup.setDistance(ui->distanceLineEdit->text().toInt());
    computeImage->computeMax();
    audio->playSupport();

    if(setup.exec()){
        int distance = setup.getDistance();
        int width = setup.getWidth();
        int height = setup.getHeight();

        ui->distanceLineEdit->setText(QString::number(distance));
        computeImage->setDistance(distance);
        computeImage->setRefAngle(width, ComputeImage::X);
        computeImage->setRefAngle(height, ComputeImage::Y);
    }
    audio->stopSupport();
}
