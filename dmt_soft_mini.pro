QT -= gui
QT += sql
QT += concurrent
QT += network


CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        DatabaseUpload.cpp \
        devicedmt5.cpp \
        downloadcontrol.cpp \
        main.cpp \
        restconnector.cpp \
        restinterface.cpp \
        sqllitecontrol.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    DatabaseUpload.h \
    LibBC.h \
    common.h \
    devicedmt5.h \
    downloadcontrol.h \
    restconnector.h \
    restinterface.h \
    sqllitecontrol.h


#INCLUDEPATH += $$PWD/../libs/win32/include

LIBS += ../dmt_soft_mini/libs/win32/include/LibBodycam.dll
