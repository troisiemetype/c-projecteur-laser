#ifndef PROJECTEURLASER_H
#define PROJECTEURLASER_H

#include <QMainWindow>

#include <QFileDialog>

#include "computeimage.h"
#include "image.h"
#include "serial.h"
#include "serialconfig.h"
#include "winserialsettings.h"
#include "wininfo.h"

namespace Ui {
class ProjecteurLaser;
}

class ProjecteurLaser : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjecteurLaser(QWidget *parent = 0);
    ~ProjecteurLaser();

private slots:
    void on_actionFileNew_triggered();

    void on_actionFileOpen_triggered();

    void on_actionFileSave_triggered();

    void on_actionFileClose_triggered();

    void on_actionSerialConnect_triggered();

    void on_actionSerialDisconnect_triggered();

    void on_actionSerialSettings_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_triggered();

    void on_actionImageCompute_triggered();

    void on_actionImageCalibrate_triggered();

    void on_actionSendData_triggered();

    void on_supportWidthLineEdit_returnPressed();

    void on_supportHeightLineEdit_returnPressed();

    void on_distanceLineEdit_returnPressed();

    void on_speedLineEdit_returnPressed();

    void on_blackWhiteCheckEdit_toggled(bool checked);

    void on_horizontalSlider_sliderReleased();

private:
    Ui::ProjecteurLaser *ui;

    ComputeImage computeImage;
    Image image;
    Serial serial;

    QString typeFichier;

//    Serial serial;
};

#endif // PROJECTEURLASER_H
