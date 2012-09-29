#-------------------------------------------------
#
# Project created by QtCreator 2012-09-29T11:33:36
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=gnu++0x


TARGET = autotest
TEMPLATE = app


SOURCES += main.cpp\
        auto_test.cpp \
    qoption.cpp \
    db/msg_structs.cpp

HEADERS  += auto_test.h \
    qoption.h  \
    db/msg_structs.h

FORMS    += auto_test.ui

OTHER_FILES += \
    db/msg_structs.msg


INCLUDEPATH += ../../mtk/src


LIBS =  -L../mtk/lib  -lyaml -lmtksupport -lqpidtypes

RESOURCES += \
    qrc.qrc
