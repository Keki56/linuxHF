#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T18:00:49
#
#-------------------------------------------------

QT       -= gui

TARGET = messagetypes
TEMPLATE = lib
CONFIG += staticlib

SOURCES += messagetypes.cpp

HEADERS += messagetypes.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

Release:DESTDIR = ../../build/messagetypes/release
Release:OBJECTS_DIR = ../../build/messagetypes/release
Release:MOC_DIR = ../../build/messagetypes/release
Release:RCC_DIR = ../../build/messagetypes/release
Release:UI_DIR = ../../build/messagetypes/release

Debug:DESTDIR = ../../build/messagetypes/debug
Debug:OBJECTS_DIR = ../../build/messagetypes/debug
Debug:MOC_DIR = ../../build/messagetypes/debug
Debug:RCC_DIR = ../../build/messagetypes/debug
Debug:UI_DIR = ../../build/messagetypes/debug
