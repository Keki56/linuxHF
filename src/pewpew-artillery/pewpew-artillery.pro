#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T18:27:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pewpew-artillery
TEMPLATE = app

INCLUDEPATH += ../common/

SOURCES += main.cpp\
        lobbywindow.cpp \
    gameengine.cpp \
    map.cpp \
    controler.cpp \
    gamewindow.cpp

HEADERS  += lobbywindow.h \
    gameengine.h \
    map.h \
    controler.h \
    gamewindow.h

FORMS    += lobbywindow.ui \
    gamewindow.ui
