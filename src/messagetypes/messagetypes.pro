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
