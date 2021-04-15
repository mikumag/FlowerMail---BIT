TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += \
    -L/usr/lib -lmysqlclient -lpthread -lz -lzstd -lm -lrt -lssl -lcrypto -ldl \

SOURCES += \
        Main.cpp \


HEADERS += \
    Classes.h \
    global_args.h \
    global_requirements.h \
    temp_class.h
