#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T18:27:57
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pewpew-artillery
TEMPLATE = app

CONFIG += debug_and_release

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD/../messagetypes/ \
               $$PWD/../../include/

CONFIG(debug, debug|release):LIBS += -L$$PWD/../../build/messagetypes/debug
CONFIG(release, debug|release):LIBS += -L$$PWD/../../build/messagetypes/release
Debug:LIBS += -L$$PWD../../build/messagetypes/debug
Release:LIBS += -L$$PWD/../../build/messagetypes/release
LIBS += -L$$PWD/../../lib/

LIBS += -lmessagetypes


SOURCES +=\
    lobbywindow.cpp \
    gameengine.cpp \
    gamewindow.cpp \
    controller.cpp \
    connecttoserverdialog.cpp \
    lobby.cpp \
    animation.cpp

HEADERS  += \
    lobbywindow.h \
    gameengine.h \
    gamewindow.h \
    controller.h \
    connecttoserverdialog.h \
    lobby.h \
    animation.h

FORMS    += lobbywindow.ui \
    gamewindow.ui \
    connecttoserverdialog.ui

CONFIG(release, debug|release):DESTDIR = $$PWD/../../build/pewpew-artillery/release
CONFIG(release, debug|release):OBJECTS_DIR = $$PWD/../../build/pewpew-artillery/release
CONFIG(release, debug|release):MOC_DIR = $$PWD/../../build/pewpew-artillery/release
CONFIG(release, debug|release):RCC_DIR = $$PWD/../../build/pewpew-artillery/release
CONFIG(release, debug|release):UI_DIR = $$PWD/../../build/pewpew-artillery/release

CONFIG(debug, debug|release):DESTDIR = $$PWD/../../build/pewpew-artillery/debug
CONFIG(debug, debug|release):OBJECTS_DIR = $$PWD/../../build/pewpew-artillery/debug
CONFIG(debug, debug|release):MOC_DIR = $$PWD/../../build/pewpew-artillery/debug
CONFIG(debug, debug|release):RCC_DIR = $$PWD/../../build/pewpew-artillery/debug
CONFIG(debug, debug|release):UI_DIR = $$PWD/../../build/pewpew-artillery/debug

RESOURCES += \
    sprites.qrc
