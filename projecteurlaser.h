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

#include <QInputDialog>
#include <QFileDialog>

#include "audio.h"
#include "computeimage.h"
#include "image.h"
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

    void enableSends(bool);
    void populateGui();

signals:
    void exposureChanged(int);

private slots:
    void on_actionFileNew_triggered();

    void on_actionFileOpen_triggered();

    void on_actionFileSave_triggered();

    void on_actionFileClose_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_triggered();

    void on_actionImageCompute_triggered();

    void on_actionImageCalibrate_triggered(bool checked);

    void on_supportWidthLineEdit_editingFinished();

    void on_supportHeightLineEdit_editingFinished();

    void on_distanceLineEdit_editingFinished();

    void on_imageModeComboBox_currentIndexChanged(int index);

    void on_stepSlider_valueChanged(int value);

    void on_widthMmLineEdit_editingFinished();

    void on_heightMmLineEdit_editingFinished();

    void on_actionGrayScale_triggered();

    void on_actionSend_triggered(bool checked);

    void on_actionPause_triggered(bool checked);

    void on_actionStop_triggered();

    void handleAudioStopped();

    void handleProgress(int value);

    void on_angleSpinBox_valueChanged(int arg1);

    void on_exposureSlider_sliderMoved(int position);

    void on_repeatSpinBox_valueChanged(int arg1);

    void on_offsetXSlider_valueChanged(int value);

    void on_offsetYSlider_valueChanged(int value);

    void on_jumpSpinBox_valueChanged(int arg1);

    void on_resetButton_clicked();

    void on_actionResample_triggered();

private:
    Ui::ProjecteurLaser *ui;

    ComputeImage *computeImage;
    Image image;
    Audio *audio;

    int repeat;

    QString typeFichier;

};

#endif // PROJECTEURLASER_H
