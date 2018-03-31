#-------------------------------------------------
#
# Project created by QtCreator 2017-11-27T19:12:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = el
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32-g*
{
    LIBS += -lopengl32
}

SOURCES += \
        main.cpp \
    baseview.cpp \
    simpleobject3d.cpp \
    group3d.cpp \
    camera3d.cpp \
    mainwindow.cpp \
    calculatespikes.cpp \
    calculatepipes.cpp

HEADERS += \
    baseview.h \
    simpleobject3d.h \
    transformational.h \
    group3d.h \
    camera3d.h \
    mainwindow.h \
    types.h \
    calculatespikes.h \
    calculatepipes.h

FORMS += \
    mainwindow.ui

DISTFILES +=

RESOURCES += \
    shaders.qrc \
    obj.qrc
