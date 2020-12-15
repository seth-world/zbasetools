#QT -= gui
QT += core xml network printsupport
CONFIG += c++17 console

TARGET = zbasedbgtest

PYTHON = python3.7


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
                                                                                                              ...
TEMPLATE = qt console


# mandatory : define home directory, where Development directory can be found

HOME = /home/gerard

#------------Private to libraries build and install  (python utility <postlinkcopy.py>)-----------
#

# --------ztoolset common definitions------------


DEVELOPMENT_BASE = $${HOME}/Development
TOOLSET_ROOT = $${DEVELOPMENT_BASE}/zbasetools

MODULE = zflow # name of the root include directory
INSTALL_ACTION = CLEANCOPY
PYTHON_PROC = $${TOOLSET_ROOT}/common/postlinkcopy.py

#defines DEVELOPMENT_BASE, TOOLSET symbols and ICU symbols

include ($${TOOLSET_ROOT}/common/zbasecommon.pri)



#-------------Windows-------------------------------

win32:ZBASESYSTEM_BASE = D:\Development\zbasesystem
win32:DEVELOPMENT_BASE = D:\Development
win32:ZTOOLSET_BASE = D:\Development\zbasesystem\ztoolset
win32:OPENSSL_BASE=$$DEVELOPEMENT_BASE\openssl
win32:OPENSSL_LIBPATH=$$OPENSSL_BASE\lib
win32:LIBXML_BASE=$$DEVELOPMENT_BASE\libxml2




INCLUDEPATH += \
                $${TOOLSET_INC_ZBASE}     \
                $${TOOLSET_INC_ZNET}     \
                $${TOOLSET_INC_ZCONTENT}  \
                $${TOOLSET_INC_ZAM}       \
                $${OPENSSL_INCLUDEPATH}

INCLUDEPATH +=    $$OPENSSL_INCLUDEPATH \
                        $$LIBXML_BASE


QMAKE_CXXFLAGS+= -Wall -std=c++17  -pthread
#QMAKE_CXXFLAGS+= -std=c++14  -pthread -I/home/gerard/Development/zbasesystem  -I/home/gerard/Development/zbasesystem/ztoolset

#QMAKE_LFLAGS +=  -std=c++14 -lpthread
#QMAKE_LFLAGS +=  -std=c++14 -lpthread -lcrypto
#QMAKE_LFLAGS +=  -std=c++14 -lpthread -lcrypto -L"/home/gerard/Development/zbasesystem/libzbs_debug" -lzbasesystem
#QMAKE_CXXFLAGS+= -std=c++14 -Wall -pthread

#if not using ldap-lssl
#QMAKE_LFLAGS +=  -std=c++14 -lpthread
#if using ldap
QMAKE_LFLAGS+= -lldap
#if using lssl
QMAKE_LFLAGS+= -lssl

# if using libxml2
INCLUDEPATH +=/usr/include/libxml2

#
#   with QT5.6 do not put "" around libraries when using LIBS (generates a error message on build)
#
LIBS += -ldl -static-libstdc++
LIBS += -L$${LIBRARY_REPOSITORY} -lzbase -lzcontent -lzam -lznet -lcrypto  -lcrypt


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    zconfig.h

