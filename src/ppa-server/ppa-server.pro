#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T18:31:19
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = ppa-server
CONFIG   += console debug_and_release
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD/../messagetypes/ \
               $$PWD/../../include/

CONFIG(debug, debug|release):LIBS += -L$$PWD/../../build/messagetypes/debug
CONFIG(release, debug|release):LIBS += -L$$PWD/../../build/messagetypes/release
Debug:LIBS += -L$$PWD../../build/messagetypes/debug
Release:LIBS += -L$$PWD/../../build/messagetypes/release

LIBS += -lmessagetypes

SOURCES += \
    server.cpp

HEADERS += \
    server.h

CONFIG(release, debug|release):DESTDIR = $$PWD/../../build/ppa-server/release
CONFIG(release, debug|release):OBJECTS_DIR = $$PWD/../../build/ppa-server/release
CONFIG(release, debug|release):MOC_DIR = $$PWD/../../build/ppa-server/release
CONFIG(release, debug|release):RCC_DIR = $$PWD/../../build/ppa-server/release
CONFIG(release, debug|release):UI_DIR = $$PWD/../../build/ppa-server/release

CONFIG(debug, debug|release):DESTDIR = $$PWD/../../build/ppa-server/debug
CONFIG(debug, debug|release):OBJECTS_DIR = $$PWD/../../build/ppa-server/debug
CONFIG(debug, debug|release):MOC_DIR = $$PWD/../../build/ppa-server/debug
CONFIG(debug, debug|release):RCC_DIR = $$PWD/../../build/ppa-server/debug
CONFIG(debug, debug|release):UI_DIR = $$PWD/../../build/ppa-server/debug
