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
    detectionwidget.cpp \
    transform3d.cpp \
    slamwidget.cpp

HEADERS  += mainwindow.h \
    detectionwidget.h \
    transform3d.h \
    vertex.h \
    slamwidget.h \
    vertexuv.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
