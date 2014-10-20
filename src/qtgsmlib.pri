#include($$PWD/smspdu-cpp/pdu.pri)
#include($$PWD/libpdu/libpdu.pri)

QMAKE_CXXFLAGS += -I$$PWD

HEADERS += \
    $$PWD/smsmessage/qatresult.h \
    $$PWD/smsmessage/qatresultparser.h \
    $$PWD/smsmessage/qatutils.h \
    $$PWD/smsmessage/qcbsmessage.hpp \
    $$PWD/smsmessage/qsmsmessage.hpp \
    $$PWD/smsmessage/qsmsmessage_p.h \
    $$PWD/smsmessage/qgsmcodec.h

SOURCES += \
    $$PWD/smsmessage/qatresult.cpp \
    $$PWD/smsmessage/qatresultparser.cpp \
    $$PWD/smsmessage/qatutils.cpp \
    $$PWD/smsmessage/qcbsmessage.cpp \
    $$PWD/smsmessage/qsmsmsmessage.cpp \
    $$PWD/smsmessage/qgsmcodec.cpp
