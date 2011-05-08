#-------------------------------------------------
#
# Project created by QtCreator 2011-04-22T08:32:34
#
#-------------------------------------------------

QT       += webkit network core gui

TARGET = uicrawler
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logwidget.cpp \
    graphwidget.cpp \
    profilewidget.cpp

HEADERS  += mainwindow.h \
    logwidget.h \
    graphwidget.h \
    data.h \
    arrow.h \
    profilewidget.h
RESOURCES = uicrawler.qrc

FORMS +=

OTHER_FILES += \
    uicrawler.js
