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

#ifndef WININFO_H
#define WININFO_H

/*
 * This class handles the message boxes
 * Static methods are defined, that may be called by
 * whichever part of the program needs it.
 * These methods just take title and message as parameters
 * They return an int that represent the way they were closed
 * (i.e. by clicking ok, cancel, quit, escape, etc.)
 */

#include <QMessageBox>
#include <QString>


class WinInfo
{

public:
   WinInfo();
   static int info(QString const&, QString const& = "");
   static int question(QString const&, QString const& = "");
   static int error(QString const&, QString const& = "");

};

#endif // WININFO_H
