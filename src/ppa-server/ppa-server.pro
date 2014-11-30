#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T18:31:19
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = ppa-server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../messagetypes/ \
               ../../include/

Debug:LIBS += -L../../build/messagetypes/debug
Release:LIBS += -L../../build/messagetypes/release
LIBS += -L../../lib/

LIBS += -lmessagetypes

SOURCES += \
    server.cpp

HEADERS += \
    server.h

Release:DESTDIR = ../../build/ppa-server/release
Release:OBJECTS_DIR = ../../build/ppa-server/release
Release:MOC_DIR = ../../build/ppa-server/release
Release:RCC_DIR = ../../build/ppa-server/release
Release:UI_DIR = ../../build/ppa-server/release

Debug:DESTDIR = ../../build/ppa-server/debug
Debug:OBJECTS_DIR = ../../build/ppa-server/debug
Debug:MOC_DIR = ../../build/ppa-server/debug
Debug:RCC_DIR = ../../build/ppa-server/debug
Debug:UI_DIR = ../../build/ppa-server/debug
