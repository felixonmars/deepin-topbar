#-------------------------------------------------
#
# Project created by QtCreator 2017-06-27T13:52:24
#
#-------------------------------------------------

include(../../interfaces/interfaces.pri)

QT       += core gui widgets dbus x11extras
PKGCONFIG += dtkwidget dtkbase dtkutil dframeworkdbus
TEMPLATE = lib
CONFIG    += plugin c++11 link_pkgconfig
TARGET     = $$qtLibraryTarget(notify)
DESTDIR    = $$_PRO_FILE_PWD_/../
DISTFILES += notify.json

DEFINES += NOTIFY_LIBARAY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += notifyplugin.cpp \
    notifywidget.cpp \
    notifypopupwidget.cpp

HEADERS += notifyplugin.h \
    notifywidget.h \
    notifypopupwidget.h

target.path = $${PREFIX}/lib/dde-topbar/plugins/
INSTALLS   += target
