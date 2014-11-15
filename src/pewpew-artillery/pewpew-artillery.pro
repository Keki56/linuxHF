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
    gamewindow.cpp \
    gameengine.cpp \
    player.cpp \
    map.cpp

HEADERS  += lobbywindow.h \
    gamewindow.h \
    gameengine.h \
    player.h \
    map.h

FORMS    += lobbywindow.ui \
    gamewindow.ui
