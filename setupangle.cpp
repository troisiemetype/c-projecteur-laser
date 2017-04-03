#include "setupangle.h"
#include "ui_setupangle.h"

SetupAngle::SetupAngle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupAngle)
{
    ui->setupUi(this);
}

SetupAngle::~SetupAngle()
{
    delete ui;
}

void SetupAngle::setDistance(const int &value){
    ui->distanceLineEdit->setText(QString::number(value));
}

int SetupAngle::getDistance() const{
    return ui->distanceLineEdit->text().toInt();
}

int SetupAngle::getWidth() const{
    return ui->widthLineEdit->text().toInt();
}

int SetupAngle::getHeight() const{
    return ui->heightLineEdit->text().toInt();
}
