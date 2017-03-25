#-------------------------------------------------
#
# Project created by QtCreator 2016-10-21T13:33:40
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projecteur_laser
TEMPLATE = app


SOURCES += main.cpp\
           projecteurlaser.cpp \
           image.cpp \
           wininfo.cpp \
           computeimage.cpp \
           audio.cpp \
    tools.cpp

HEADERS  += projecteurlaser.h \
            image.h \
            wininfo.h \
            computeimage.h \
            audio.h \
    tools.h

FORMS    += projecteurlaser.ui

TRANSLATIONS = projecteurlaser_en.ts \
               projecteurlaser_fr.ts
