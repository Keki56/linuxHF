#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T18:27:57
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pewpew-artillery
TEMPLATE = app

INCLUDEPATH += ../messagetypes/ \
               ../../include/

Debug:LIBS += -L../../build/messagetypes/debug
Release:LIBS += -L../../build/messagetypes/release
LIBS += -L../../lib/

LIBS += -lmessagetypes


SOURCES +=\
        lobbywindow.cpp \
    gameengine.cpp \
    map.cpp \
    controler.cpp \
    gamewindow.cpp \
    lobby.cpp \
    connecttoserverdialog.cpp

HEADERS  += lobbywindow.h \
    gameengine.h \
    map.h \
    controler.h \
    gamewindow.h \
    lobby.h \
    connecttoserverdialog.h

FORMS    += lobbywindow.ui \
    gamewindow.ui \
    connecttoserverdialog.ui

Release:DESTDIR = ../../build/pewpew-artillery/release
Release:OBJECTS_DIR = ../../build/pewpew-artillery/release
Release:MOC_DIR = ../../build/pewpew-artillery/release
Release:RCC_DIR = ../../build/pewpew-artillery/release
Release:UI_DIR = ../../build/pewpew-artillery/release

Debug:DESTDIR = ../../build/pewpew-artillery/debug
Debug:OBJECTS_DIR = ../../build/pewpew-artillery/debug
Debug:MOC_DIR = ../../build/pewpew-artillery/debug
Debug:RCC_DIR = ../../build/pewpew-artillery/debug
Debug:UI_DIR = ../../build/pewpew-artillery/debug
