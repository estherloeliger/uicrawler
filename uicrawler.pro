#-------------------------------------------------
#
# Project created by QtCreator 2011-04-22T08:32:34
#
#-------------------------------------------------

QT       += webkit network core gui

#test
CONFIG  += webkit phonon
#end test

TARGET = uicrawler
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logwidget.cpp \
    graphwidget.cpp \
    profilewidget.cpp \
    filterwidget.cpp \
    webpage.cpp \
    modeler.cpp \
    mystring.cpp \
    browser.cpp \
    arrow.cpp \
    mappingwidget.cpp \
    popuplistwidget.cpp \
    data.cpp

HEADERS  += mainwindow.h \
    logwidget.h \
    graphwidget.h \
    data.h \
    arrow.h \
    profilewidget.h \
    state.h \
    filterwidget.h \
    webpage.h \
    modeler.h \
    mystring.h \
    constants.h \
    browser.h \
    mappingwidget.h \
    popuplistwidget.h
RESOURCES = uicrawler.qrc

FORMS +=

OTHER_FILES += \
    uicrawler.js \
    sha1.js \
    jquery-latest.js
