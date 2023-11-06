#-------------------------------------------------
#
# Project created by QtCreator 2019-02-26T19:25:46
#
#-------------------------------------------------

#QT += core gui xml
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QT -= core  # requested for QList
QT -= gui
QT += core

greaterThan(QT_MAJOR_VERSION,5): QT += core5compat

TEMPLATE = lib
CONFIG += c++17 shared dll create_prl link_prl
TARGET = zbase
DEFINES += ZBASE_LIBRARY


# mandatory : define home directory, where Development directory can be found

unix: HOME = "/home/gerard"

#------------Private to zbase libraries build and install-----------
#
DEVELOPMENT_BASE = $${HOME}/Development
TOOLSET_ROOT = $${DEVELOPMENT_BASE}/zbasetools

MODULE = zbase # name of the root include directory
INSTALL_ACTION = CLEANCOPY
PYTHON_PROC = $${TOOLSET_ROOT}/common/postlinkcopy.py

# zbase common definitions

#message(" HOME <$${HOME}>  TOOLSET_ROOT <$${TOOLSET_ROOT}> DEVELOPMENT_BASE<$${DEVELOPMENT_BASE}>")

include ($${TOOLSET_ROOT}/common/zbasecommon.pri)

#---------------------------------------------------------------------

#DEFINES += ZBASELIB_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.


#TARGET.path += $$TOOLSET_ROOT/bin/debug





#win32: QMAKE_POST_LINK += copy /y "$$shell_path($$TARGET_PATH/app_name.exe)" "$$shell_path($$PWD/../build/)"
#unix: QMAKE_POST_LINK += cp "$$shell_path($$TARGET.*)" "$$shell_path($$TOOLSET_ROOT/bin/debug)"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# CONFIG(debug, release|debug): OBJECTS_DIR = $$TOOLSET_BASE/bin/debug
# CONFIG(release, release|debug): OBJECTS_DIR = $$TOOLSET_BASE/bin/release



# QMAKE_CXXFLAGS += -fpermissive

#
# Warning :
# 1- do not confuse -lcrypto and -lcrypt both libraries are necessary
# 2- lcrypt must be the last in library list
#

#QMAKE_CXXFLAGS+= -std=c++17 -Wall -fno-implicit-templates -pthread
QMAKE_CXXFLAGS += -std=c++17 -Wall  -pthread -fPIC # -fpic for shared libraries

#if not using ldap-lssl
QMAKE_LFLAGS +=  -shared -lpthread -fPIC # -fpic for shared libraries
#if using ldap AND dynamic linkage
#QMAKE_LFLAGS +=  -lldap

#base libs dynamic
LIBS += -ldl  -lstdc++

#LIBS +=/home/gerard/Development/zbasesystem/build-zbasesystemlib-Debug/libzbasesystem.a

#           +---------------------+
#           !  shared libraries   !
#           +---------------------+

#---------static linkage (comment out if dynamic link)-----------------
#path for static libraries to link : /usr/lib/x86_64-linux-gnu/
#QMAKE_LFLAGS += -L/usr/lib/
#QMAKE_LFLAGS +=  -L/usr/lib/x86_64-linux-gnu/


# if using ldap

#libldap static
#unix:LIBS += /usr/lib/x86_64-linux-gnu/libldap.a
#libldap dynamic
#unix:LIBS += -lldap

# if using libxml++  (version 2.6)
#LIBS +=/usr/lib/libxml++-2.6.a

# if using libxml2
#for libxml2 static
#unix:LIBS += -L/usr/lib/x86_64-linux-gnu/ -llibxml2.a
#unix:LIBS += /usr/lib/x86_64-linux-gnu/libxml2.a

#for libxml2 dynamic
unix:LIBS +=  -lxml2


#for icu static
#LIBS += -L$$ICU_BASE/lib
#LIBS += $$ICU_BASE/lib/libicuuc.a
#LIBS += $$ICU_BASE/lib/libicudata.a
#LIBS += $$ICU_BASE/lib/libicui18n.a
#LIBS += $$ICU_BASE/lib/libicuio.a
#LIBS += $$ICU_BASE/lib/libicutu.a
#LIBS += $$ICU_BASE/lib/libicutest.a

#for icu dynamic

#LIBS+= -L$${ICU_LIB} -licuuc -licudata -licui18n -licuio -licutu -licutest
LIBS+=  -licuuc -licudata -licui18n -licuio -licutu -licutest

#LIBS += /usr/lib/libicuuc.so
#LIBS += /usr/lib/libicui18n.so
#LIBS += /usr/lib/libicuio.so
#LIBS += /usr/lib/libicutu.so
#LIBS += /usr/lib/libicutest.so

#for ssl and cryto static
#LIBS += /usr/lib/x86_64-linux-gnu/libssl.a
#LIBS += /usr/lib/x86_64-linux-gnu/libcrypto.a

#for ssl and crypto dynamic
LIBS += -lssl
LIBS += -lcrypto -lcrypt


#------------end static linkage ------------------

#-----------dynamic linkage ----------------------------

#if using lssl
# QMAKE_LFLAGS +=   -lssl
# LIBS += -ldl -lstdc++
#QMAKE_LFLAGS += -lxml2
#in any cases and should remain after lssl
#QMAKE_LFLAGS += -lcrypto -lcrypt



#win32:CONFIG(release, debug|release): LIBS += -L$$ICU_BASE/lib/release/ -licudata
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$ICU_BASE/lib/debug/ -licudata
#else:unix: LIBS += -L$$ICU_BASE/lib/ -licudata

#win32:CONFIG(release, debug|release): LIBS += -L$$ICU_BASE/lib/release/ -licui18n
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$ICU_BASE/lib/debug/ -licui18n
#else:unix: LIBS += -L$$ICU_BASE/lib/ -licui18n


#win32:CONFIG(release, debug|release): LIBS += -L$$ICU_BASE/lib/release/ -licuio
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$ICU_BASE/lib/debug/ -licuio
#else:unix: LIBS += -L$$ICU_BASE/lib/ -licuio


#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$ICU_BASE/lib/release/libicuio.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$ICU_BASE/lib/debug/libicuio.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$ICU_BASE/lib/release/icuio.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$ICU_BASE/lib/debug/icuio.lib
#else:unix: PRE_TARGETDEPS += $$ICU_BASE/lib/libicuio.a

#win32:CONFIG(release, debug|release): LIBS += -L$$ICU_BASE/lib/release/ -licutu
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$ICU_BASE/lib/debug/ -licutu
#else:unix: LIBS += -L$$ICU_BASE/lib/ -Bstatic -licutu -Bdynamic # tell linker to link statically libicutu


#win32:CONFIG(release, debug|release): LIBS += -L$$ICU_BASE/lib/release/ -licuuc
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$ICU_BASE/lib/debug/ -licuuc
#else:unix: LIBS += -L$$ICU_BASE/lib/ -licuuc

#win32:CONFIG(release, debug|release): LIBS += -L$$ICU_BASE/lib/release/ -licutest
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$ICU_BASE/lib/debug/ -licutest
#else:unix: LIBS += -L$$ICU_BASE/lib/ -licutest

#base libs static
#LIBS += -ldl -static-libstdc++  #do not link libstdc in static where we link icu in dynamic because icu libraries need libstdc

INCLUDEPATH +=  /usr/include/x86_64-linux-gnu \
                /usr/include

INCLUDEPATH += \
#/usr/include/c++/8 \
#                /usr/lib/x86_64-linux-gnu/glib-2.0/include \
 #               $$ICU_BASE/usr/include \
#                /usr/include \
#                /usr/include/openssl \
#                /home/gerard/Development/usr/include \
                $$TOOLSET_ROOT  \
                $$TOOLSET_BASE  \
                $$DEVELOPMENT_BASE
#                /usr/local/include/c++/8.2.0 \
#                /usr/local/include \
#                /usr/include/c++ \
#                /usr/include/c++/7 \
#                /usr/include/glib-2.0 \
#if using ICU
#INCLUDEPATH +=  $$ICU_SRC \
#                $$ICU_SRCi18n

# if using libxml2

INCLUDEPATH +=  /usr/include/libxml2

# if using libxml++ (version 2.6)  : uncomment
#-----------------------------------
#INCLUDEPATH +=  /usr/include/libxml++-2.6 \
#                /usr/include/libxml++-2.6/libxml++ \
#                /usr/lib/libxml++-2.6/include \
#                /usr/include/glibmm-2.4 \
#                /usr/lib/x86_64-linux-gnu/glibmm-2.4/include


HEADERS +=  $${TOOLSET_ROOT}/config/zconfig_general.h \
            $${TOOLSET_ROOT}/config/zconfig.h \
            $$TOOLSET_BASE/ztoolset/zarray.h \
    $$TOOLSET_BASE/ztoolset/ztoolset_common.h \
    $$TOOLSET_BASE/ztoolset/zerror.h \
    $$TOOLSET_BASE/zio/zdir.h \
    $$TOOLSET_BASE/zthread/zthread.h \
    $$TOOLSET_BASE/zthread/zarglist.h \
    $$TOOLSET_BASE/ztoolset/zexceptionmin.h \
    $$TOOLSET_BASE/ztoolset/zbasedatatypes.h \
    $$TOOLSET_BASE/ztoolset/ztoolset_doc.h \
#    $$TOOLSET_BASE/zxml/qxmlutilities.h \
    $$TOOLSET_BASE/ztoolset/zdatabuffer.h \
#    $$TOOLSET_BASE/ztoolset/zdate.h \
    $$TOOLSET_BASE/ztoolset/uristring.h \
    $$TOOLSET_BASE/ztoolset/ztime.h \
    $$TOOLSET_BASE/ztoolset/zatomicconvert.h \
    $$TOOLSET_BASE/ztoolset/zmodulestack.h \
    $$TOOLSET_BASE/ztoolset/zerror_min.h \
    $$TOOLSET_BASE/ztoolset/zerror.h \
    $$TOOLSET_BASE/zxml/zxmldoc.h \
    $$TOOLSET_BASE/zxml/zxmlerror.h \
    $$TOOLSET_BASE/zxml/zxmlnode.h \
    $$TOOLSET_BASE/zxml/zxmlattribute.h \
    $$TOOLSET_BASE/zxml/zxmlnamespace.h \
    $$TOOLSET_BASE/zxml/zxml-documentation.h \
    $$TOOLSET_BASE/zxml/zxml.h \
    $$TOOLSET_BASE/ztoolset/zlimit.h \
    $$TOOLSET_BASE/ztoolset/encodedString.h \
    $$TOOLSET_BASE/ztoolset/zcharset.h \
    $$TOOLSET_BASE/ztoolset/utftemplatestring.h \
    $$TOOLSET_BASE/ztoolset/zutfstrings.h \
    $$TOOLSET_BASE/ztoolset/zutfstrings.h \
    $$TOOLSET_BASE/ztoolset/exceptionstring.h \
    $$TOOLSET_BASE/ztoolset/utfvsprintf.h \
    $$TOOLSET_BASE/ztoolset/utfutils.h \
    $$TOOLSET_BASE/ztoolset/utfsprintf.h \
    $$TOOLSET_BASE/ztoolset/utffixedstring.h \
    $$TOOLSET_BASE/ztoolset/utfstringcommon.h \
    $$TOOLSET_BASE/ztoolset/cfixedstring.h \
    $$TOOLSET_BASE/ztoolset/utfvtemplatestring.h \
    $$TOOLSET_BASE/ztoolset/utfvaryingstring.h \
    $$TOOLSET_BASE/ztoolset/zutfstrings.h \
    $$TOOLSET_BASE/ztoolset/charfixedstring.h \
    $$TOOLSET_BASE/ztoolset/charvaryingstring.h \
    $$TOOLSET_BASE/zcrypt/zcrypt.h \
    $$TOOLSET_BASE/zcrypt/checksum.h \
    $$TOOLSET_BASE/zcrypt/md5.h \
    $$TOOLSET_BASE/ztoolset/zmem.h \
    $$TOOLSET_BASE/ztoolset/utfkey.h \
    $$TOOLSET_BASE/ztoolset/zicuconverter.h \
    $$TOOLSET_BASE/ztoolset/utfconverter.h \
    $$TOOLSET_BASE/ztoolset/utfstrctx.h \
    $$TOOLSET_BASE/ztoolset/cnvcallback.h \
    $$TOOLSET_BASE/ztoolset/zicu.h \
    $$TOOLSET_BASE/ztoolset/zlocale.h \
    $$TOOLSET_BASE/ztoolset/zsymbols.h \
    $$TOOLSET_BASE/ztoolset/ztypetype.h\
    $$TOOLSET_BASE/zthread/zmutex.h \
    $$TOOLSET_BASE/ztoolset/cescapedstring.h \
    $$TOOLSET_BASE/ztoolset/ztimer.h \
    $$TOOLSET_BASE/zthread/zthreadexithandler.h \
    $$TOOLSET_BASE/ztoolset/ztokenizer.h \
    $$TOOLSET_BASE/ztoolset/zsystemuser.h \
    $$TOOLSET_BASE/ztoolset/zbitset.h \
#    $$TOOLSET_BASE/ztoolset/templatewstring.h \
    $$TOOLSET_BASE/ztoolset/zfunctions.h \
    $$TOOLSET_BASE/ztoolset/charman.h \
    $$TOOLSET_BASE/ztoolset/userid.h \
    $$TOOLSET_BASE/ztoolset/zaierrors.h \
    $$TOOLSET_BASE/ztoolset/zhtmlreport.h \
    $$TOOLSET_BASE/ztoolset/zstatus.h \
    $$TOOLSET_BASE/zxml/zxmlprimitives.h \
    $$TOOLSET_BASE/ztoolset/utfvstrings.h \
    $$TOOLSET_BASE/ztoolset/zdatecommon.h \
    $$TOOLSET_BASE/ztoolset/zdatefull.h \
    $$TOOLSET_BASE/ztoolset/ztoolset_release.h \
    $$TOOLSET_BASE/zio/zioutils.h




SOURCES += $$TOOLSET_BASE/ztoolset/zfunctions.cpp \   #  see zlibzbasesystem.a
    $$TOOLSET_BASE/zio/zdir.cpp \
    $$TOOLSET_BASE/ztoolset/zbasedatatypes.cpp \
    $$TOOLSET_BASE/ztoolset/zexceptionmin.cpp \
#    $$TOOLSET_BASE/zxml/qxmlutilities.cpp \
    $$TOOLSET_BASE/zthread/zthread.cpp \
    $$TOOLSET_BASE/ztoolset/zdatabuffer.cpp \
#    $$TOOLSET_BASE/ztoolset/zdate.cpp \
    $$TOOLSET_BASE/ztoolset/uristring.cpp \
    $$TOOLSET_BASE/ztoolset/ztime.cpp \
#    $$TOOLSET_BASE/ztoolset/zstrings.cpp \  #renamed as zstrings_old.cpp
    $$TOOLSET_BASE/ztoolset/zaierrors.cpp \
    $$TOOLSET_BASE/ztoolset/zhtmlreport.cpp \
    $$TOOLSET_BASE/zxml/zxmlprimitives.cpp \
    $$TOOLSET_BASE/zthread/zarglist.cpp \
    $$TOOLSET_BASE/ztoolset/zarray.cpp \
    $$TOOLSET_BASE/ztoolset/zdatecommon.cpp \
    $$TOOLSET_BASE/ztoolset/zdatefull.cpp \
    ../zio/zioutils.cpp \
\
    $$TOOLSET_BASE/ztoolset/cescapedstring.cpp \
    $$TOOLSET_BASE/zthread/zmutex.cpp \
    $$TOOLSET_BASE/ztoolset/ztimer.cpp \
    $$TOOLSET_BASE/zthread/zthreadexithandler.cpp \
    $$TOOLSET_BASE/ztoolset/ztokenizer.cpp \
    $$TOOLSET_BASE/ztoolset/zsystemuser.cpp \
    $$TOOLSET_BASE/ztoolset/zbitset.cpp \
#    $$TOOLSET_BASE/ztoolset/templatewstring.cpp \
#    $$TOOLSET_BASE/ztoolset/templatestring.cpp \
#    $$TOOLSET_BASE/ztoolset/zwstrings.cpp \
    $$TOOLSET_BASE/ztoolset/zmodulestack.cpp \
#    $$TOOLSET_BASE/ztoolset/zerror_min.cpp
    $$TOOLSET_BASE/ztoolset/zerror_min.cpp \
    $$TOOLSET_BASE/zxml/zxmldoc.cpp \
    $$TOOLSET_BASE/zxml/zxmlerror.cpp \
    $$TOOLSET_BASE/zxml/zxmlnode.cpp \
    $$TOOLSET_BASE/zxml/zxmlattribute.cpp \
    $$TOOLSET_BASE/zxml/zxmlnamespace.cpp \
    $$TOOLSET_BASE/ztoolset/encodedString.cpp \
    $$TOOLSET_BASE/ztoolset/zcharset.cpp \
    $$TOOLSET_BASE/ztoolset/utftemplatestring.cpp \
    $$TOOLSET_BASE/ztoolset/zutfstrings.cpp \
    $$TOOLSET_BASE/ztoolset/utfsprintf.cpp \
    $$TOOLSET_BASE/ztoolset/zatomicconvert.cpp \
    $$TOOLSET_BASE/ztoolset/utfstringcommon.cpp \
    $$TOOLSET_BASE/ztoolset/exceptionstring.cpp \
    $$TOOLSET_BASE/ztoolset/utfvtemplatestring.cpp \
    $$TOOLSET_BASE/ztoolset/utfvaryingstring.cpp \
    $$TOOLSET_BASE/ztoolset/charvaryingstring.cpp \
    $$TOOLSET_BASE/zcrypt/zcrypt.cpp \
    $$TOOLSET_BASE/zcrypt/checksum.cpp \
    $$TOOLSET_BASE/zcrypt/md5.cpp \
    $$TOOLSET_BASE/ztoolset/zmem.cpp \
    $$TOOLSET_BASE/ztoolset/utfkey.cpp \
    $$TOOLSET_BASE/ztoolset/zicuconverter.cpp \
    $$TOOLSET_BASE/ztoolset/utfconverter.cpp \
    $$TOOLSET_BASE/ztoolset/utfstrctx.cpp \
    $$TOOLSET_BASE/ztoolset/zlocale.cpp \
    $$TOOLSET_BASE/ztoolset/ztypetype.cpp \
    $$TOOLSET_BASE/ztoolset/utfutils.cpp


#SUBDIRS += \
#    ../zbaselib_test/zbselib_test.pro

DISTFILES += \
    $$TOOLSET_ROOT/common/postlinkcopy.py \
    $$TOOLSET_ROOT/common/zbasecommon.pri \
    ../../debugger/zbasetypes.py

# need to resolve local files path : QMake runs in a different directory and needs absolute paths
HEADERS2PY=""
for(wH , HEADERS) : HEADERS2PY += $$absolute_path($${wH})


wLaunch="python "$$quote($${PYTHON_PROC})
wLaunch+=" "
wLaunch+=$$INSTALL_ACTION
wLaunch+=" "
wLaunch+=$$quote($${MODULE})
wLaunch+="'$$quote($${TOOLSET_BASE})'"
wLaunch+=" "
wLaunch+="'$$quote($${TOOLSET_INCLUDE})'"
wLaunch+=" "
wLaunch+="'$$quote($${HEADERS2PY})'"
wLaunch+=" "
wLaunch+="'$$quote($${LIBRARY_REPOSITORY})'"
wLaunch+=" "
#wLaunch+="'$$quote(lib$${TARGET}.so)'"
wLaunch+="'$$quote($${TARGET})'"

QMAKE_POST_LINK +="$$wLaunch"
#system($$wLaunch)

