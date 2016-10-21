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
