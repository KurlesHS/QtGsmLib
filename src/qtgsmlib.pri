##################################################
### QtSerialPort
greaterThan(QT_MAJOR_VERSION, 4) {QT += serialport}
else:
{
    system("echo QTSERIALPORT_PROJECT_ROOT = $$PWD/qtserialport > $$OUT_PWD/.qmake.cache")
    system("echo QTSERIALPORT_BUILD_ROOT = $$PWD//qtserialport >> $$OUT_PWD/.qmake.cache")
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

QMAKE_CXXFLAGS += -I$$PWD -I$$PWD/smsmessage

HEADERS += \
    $$PWD/smsmessage/qatutils.h \
    $$PWD/smsmessage/qcbsmessage.hpp \
    $$PWD/smsmessage/qsmsmessage.hpp \
    $$PWD/smsmessage/qsmsmessage_p.h \
    $$PWD/smsmessage/qgsmcodec.h \
    $$PWD/atcommand.h \
    $$PWD/atchat.h \
    $$PWD/simpleatcommand.h \
    $$PWD/atresult.h \
    $$PWD/smsreader.h

SOURCES += \
    $$PWD/smsmessage/qatutils.cpp \
    $$PWD/smsmessage/qcbsmessage.cpp \
    $$PWD/smsmessage/qsmsmsmessage.cpp \
    $$PWD/smsmessage/qgsmcodec.cpp \
    $$PWD/atcommand.cpp \
    $$PWD/atchat.cpp \
    $$PWD/simpleatcommand.cpp \
    $$PWD/atresult.cpp \
    $$PWD/smsreader.cpp
