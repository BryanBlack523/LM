#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T15:52:23
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LMv0001
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        activity.cpp \
        activitylistdelegate.cpp \
        activitylistmodel.cpp \
        database.cpp \
        importtool.cpp \
        main.cpp \
        mainwindow.cpp \
        menuwindow.cpp \
        utility.cpp

HEADERS += \
        activity.h \
        activitylistdelegate.h \
        activitylistmodel.h \
        database.h \
        importtool.h \
        mainwindow.h \
        menuwindow.h \
        utility.h

FORMS += \
        mainwindow.ui \
        menuwindow.ui

RESOURCES += \
        resourses.qrc
