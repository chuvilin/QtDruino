################################################################################
#                                                                              #
#  Serial port interface imlementation.                                        #
#                                                                              #
#  This file isn't a part of the QtDruino project.                             #
#                                                                              #
#  QSerialDevice – cross-platform library for serial devices based on Qt4.     #
#  More information you can find at http://gitorious.org/qserialdevice#more    #
#                                                                              #
################################################################################


INCLUDEPATH += $$PWD

win32 {
    DEFINES += SERIALPORT_BUILD SERIALPORT_SHARED
}
HEADERS += \
    $$PWD/serialport.h \
    $$PWD/serialportinfo.h
HEADERS += \
    $$PWD/serialport_p.h \
    $$PWD/ringbuffer_p.h \
    $$PWD/serialportengine_p.h \
    $$PWD/serialportinfo_p.h
SOURCES += \
    $$PWD/serialport.cpp \
    $$PWD/serialportinfo.cpp
win32 {
    HEADERS += \
        $$PWD/serialportengine_p_win.h
    SOURCES += \
        $$PWD/serialportengine_p_win.cpp \
        $$PWD/serialportinfo_win.cpp
    !wince*: LIBS += -lsetupapi -luuid -ladvapi32
}
symbian {
    HEADERS += \
        $$PWD/serialportengine_p_symbian.h
    SOURCES += \
        $$PWD/serialportengine_p_symbian.cpp \
        $$PWD/serialportinfo_symbian.cpp
    LIBS += -leuser -lefsrv -lc32
}
unix:!symbian {
    HEADERS += \
        $$PWD/ttylocker_p_unix.h \
        $$PWD/serialportengine_p_unix.h
    SOURCES += \
        $$PWD/ttylocker_p_unix.cpp \
        $$PWD/serialportengine_p_unix.cpp
    macx {
        SOURCES += \
            $$PWD/serialportinfo_mac.cpp
        LIBS += -framework IOKit -framework CoreFoundation
    } else {
        SOURCES += \
            $$PWD/serialportinfo_unix.cpp
        linux*:contains( DEFINES, HAVE_LIBUDEV ) {
            LIBS += -ludev
        }
    }
}
