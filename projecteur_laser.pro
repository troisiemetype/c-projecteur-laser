#-------------------------------------------------
#
# Project created by QtCreator 2016-10-21T13:33:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projecteur_laser
TEMPLATE = app


SOURCES += main.cpp\
           projecteurlaser.cpp \
           image.cpp \
           wininfo.cpp \
    computeimage.cpp

HEADERS  += projecteurlaser.h \
            image.h \
            wininfo.h \
    computeimage.h

FORMS    += projecteurlaser.ui
