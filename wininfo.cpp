#include "wininfo.h"

/*
 * Definition for the class that handles message boxes.
 */

WinInfo::WinInfo()
{

}

int WinInfo::info(QString const& message, QString const& title)
{
    return QMessageBox::information(0, title, message);
}

int WinInfo::question(QString const& message, QString const& title)
{
    return QMessageBox::question(0, title, message, (QMessageBox::Yes | QMessageBox::No));

}

int WinInfo::error(QString const& message, QString const& title)
{
    return QMessageBox::warning(0, title, message);
}
