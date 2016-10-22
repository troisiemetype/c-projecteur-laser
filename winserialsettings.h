#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QDialog>

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
