#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T08:28:36
#
#-------------------------------------------------

QT       -= gui

CONFIG(debug, debug|release){
    TARGET = asound_d
    DESTDIR = ../../lib
} else {
    TARGET = asound
    DESTDIR = ../../lib
}

TEMPLATE = lib

DEFINES += ASOUND_LIBRARY

INCLUDEPATH += include/
VPATH += src/

SOURCES += $$files(src/*.cpp)
HEADERS += $$files(include/*.h)

win32{

    OPENAL_LIB_DIR = $$(OPENAL_BIN)
    OPENAL_INCLUDE_BIN = $$(OPENAL_INCLUDE)

    LIBS += -L$$OPENAL_LIB_DIR -lOpenAL32
    INCLUDEPATH += $$OPENAL_INCLUDE_BIN
}

unix{

    LIBS += -lopenal
    INCLUDEPATH += /usr/include/AL
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
