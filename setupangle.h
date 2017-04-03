#ifndef SETUPANGLE_H
#define SETUPANGLE_H

#include <QDialog>

namespace Ui {
class SetupAngle;
}

class SetupAngle : public QDialog
{
    Q_OBJECT

public:
    explicit SetupAngle(QWidget *parent = 0);
    ~SetupAngle();

    void setDistance(const int&);

    int getDistance()const;
    int getWidth()const;
    int getHeight()const;


private:
    Ui::SetupAngle *ui;
};

#endif // SETUPANGLE_H
