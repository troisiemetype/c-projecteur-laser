#include "projecteurlaser.h"
#include "ui_projecteurlaser.h"

ProjecteurLaser::ProjecteurLaser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjecteurLaser)
{
    ui->setupUi(this);
    ui->centralWidget->hide();
    ui->progressBar->hide();
    ui->progressLabel->hide();

    serial = Serial();

}

ProjecteurLaser::~ProjecteurLaser()
{
    delete ui;
}

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

        ui->imageLabel->setPixmap(image.getPixmap());

        //And get all the values to be displayed in the GUI.
        ui->widthPixelsLineEdit->setText(QString::number(image.getWidthPix()));
        ui->heightPixelsLineEdit->setText(QString::number(image.getHeightPix()));
        ui->widthMmLineEdit->setText(QString::number(image.getWidthMm()));
        ui->heightMmLineEdit->setText(QString::number(image.getHeightMm()));
        ui->distanceLineEdit->setText(QString::number(image.getDistance()));
        ui->speedLineEdit->setText(QString::number(image.getSpeed()));

        computeImage = ComputeImage(image);
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

void ProjecteurLaser::on_actionFileClose_triggered()
{
    //Hide the image and values area, hide calibrate, compute and send buttons.
    ui->centralWidget->hide();

    ui->actionImageCalibrate->setEnabled(false);
    ui->actionImageCompute->setEnabled(false);
    ui->actionSendData->setEnabled(false);

}

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
        }
    } else {
        serial.close();
        ui->actionSendData->setEnabled(false);
    }

}

void ProjecteurLaser::on_actionSerialDisconnect_triggered()
{
    serial.close();
}

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

void ProjecteurLaser::on_actionImageCompute_triggered()
{
    serial.emptyCoord();

    ui->progressLabel->show();
    ui->progressLabel->setText(tr("Calcul en cours..."));
    ui->progressBar->show();

    computeImage.computeCoords(serial.getDataArray(), ui->progressBar);

    ui->progressLabel->hide();
    ui->progressBar->hide();

    if(serial.isOpen()){
        ui->actionSendData->setEnabled(true);
    }
}

void ProjecteurLaser::on_actionImageCalibrate_triggered()
{
    computeImage.computeSupport(serial.getBoxSupportArray());
    serial.sendSupport();
}

void ProjecteurLaser::on_actionSendData_triggered()
{
    serial.sendData();
}

void ProjecteurLaser::on_supportWidthLineEdit_returnPressed()
{
    image.setSupportWidth(ui->supportWidthLineEdit->text().toInt());
}

void ProjecteurLaser::on_supportHeightLineEdit_returnPressed()
{
    image.setSupportHeight(ui->supportHeightLineEdit->text().toInt());

}

void ProjecteurLaser::on_distanceLineEdit_returnPressed()
{
    image.setDistance(ui->distanceLineEdit->text().toInt());

}

void ProjecteurLaser::on_speedLineEdit_returnPressed()
{
    image.setSpeed(ui->speedLineEdit->text().toInt());
}
