#-------------------------------------------------
#
# Project created by QtCreator 2017-03-18T12:31:46
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JunoEngine
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    renderwidget.cpp

HEADERS  += mainwindow.h \
    renderwidget.h

FORMS    += mainwindow.ui
