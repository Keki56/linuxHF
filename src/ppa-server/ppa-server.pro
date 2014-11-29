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
LIBS += ../../lib/

LIBS += -lmessagetypes

SOURCES += \
    server.cpp

HEADERS += \
    server.h
