#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T15:52:23
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LMv0001
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    activitylistdelegate.cpp \
    activitylistmodel.cpp \
    activity.cpp \
    menuwindow.cpp \
    database.cpp \
    importtool.cpp \
    importprogressdialog.cpp

HEADERS += \
        mainwindow.h \
    activitylistdelegate.h \
    activitylistmodel.h \
    activity.h \
    menuwindow.h \
    database.h \
    importtool.h \
    importprogressdialog.h

FORMS += \
        mainwindow.ui \
    menuwindow.ui \
    importprogressdialog.ui

RESOURCES += \
    resourses.qrc
