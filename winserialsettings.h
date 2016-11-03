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

#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QDialog>
#include <QStringList>

#include "serial.h"
#include "serialconfig.h"

namespace Ui {
class WinSerialSettings;
}

class WinSerialSettings : public QDialog
{
    Q_OBJECT

public:
    explicit WinSerialSettings(QWidget *parent = 0);
    ~WinSerialSettings();

private slots:
    void on_serialUpdateList_clicked();

    void on_buttonBox_accepted();

private:
    Ui::WinSerialSettings *ui;

    SerialConfig *serialConfig;
};

#endif // SERIALSETTINGS_H
