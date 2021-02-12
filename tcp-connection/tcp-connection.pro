TEMPLATE = lib

DEFINES += TCPCONNECTION_LIB

QT -= gui
QT += network

CONFIG(debug, debug|release) {

        TARGET = TcpConnection_d
	DESTDIR = ../../lib

} else {

        TARGET = TcpConnection
        DESTDIR = ../../lib
}

INCLUDEPATH += ./include


HEADERS += $$files(./include/*.h)
SOURCES += $$files(./src/*.cpp)
