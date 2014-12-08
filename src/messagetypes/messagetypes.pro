#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T18:00:49
#
#-------------------------------------------------

QT       -= gui

TARGET = messagetypes
TEMPLATE = lib
CONFIG += staticlib debug_and_release

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

CONFIG(release, debug|release):DESTDIR = $$PWD/../../build/messagetypes/release
CONFIG(release, debug|release):OBJECTS_DIR = $$PWD/../../build/messagetypes/release
CONFIG(release, debug|release):MOC_DIR = $$PWD/../../build/messagetypes/release
CONFIG(release, debug|release):RCC_DIR = $$PWD/../../build/messagetypes/release
CONFIG(release, debug|release):UI_DIR = $$PWD/../../build/messagetypes/release

CONFIG(debug, debug|release):DESTDIR = $$PWD/../../build/messagetypes/debug
CONFIG(debug, debug|release):OBJECTS_DIR = $$PWD/../../build/messagetypes/debug
CONFIG(debug, debug|release):MOC_DIR = $$PWD/../../build/messagetypes/debug
CONFIG(debug, debug|release):RCC_DIR = $$PWD/../../build/messagetypes/debug
CONFIG(debug, debug|release):UI_DIR = $$PWD/../../build/messagetypes/debug
