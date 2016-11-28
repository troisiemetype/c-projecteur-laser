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

    void on_actionImageCalibrate_triggered(bool checked);

    void on_actionSendData_triggered(bool checked);

    void on_supportWidthLineEdit_editingFinished();

    void on_supportHeightLineEdit_editingFinished();

    void on_distanceLineEdit_editingFinished();

    void on_speedLineEdit_editingFinished();

    void on_blackWhiteCheckEdit_toggled(bool checked);

    void on_horizontalSlider_sliderReleased();

    void on_widthMmLineEdit_editingFinished();

    void on_heightMmLineEdit_editingFinished();

    void readData();

    void on_modeComboBox_currentIndexChanged(int index);

private:
    Ui::ProjecteurLaser *ui;

    ComputeImage computeImage;
    Image image;
    Serial serial;

    QString typeFichier;

//    Serial serial;
};

#endif // PROJECTEURLASER_H
