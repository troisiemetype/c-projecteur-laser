#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QDialog>

#include "serialconfig.h"

namespace Ui {
class SerialSettings;
}

class SerialSettings : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSettings(QWidget *parent = 0);
    ~SerialSettings();

private slots:
    void on_serialUpdateList_clicked();

    void on_buttonBox_accepted();

private:
    Ui::SerialSettings *ui;

    SerialConfig *serialConfig;
};

#endif // SERIALSETTINGS_H
