QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYShutdown
TEMPLATE = app

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    blurimage.cpp

HEADERS += \
        mainwindow.h \
    blurimage.h

RESOURCES += \
    res.qrc