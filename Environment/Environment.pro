TARGET = Environment
TEMPLATE = lib
QT += widgets
DEFINES += LIBRARY_ENVIRONMENT

windows {
	LIBS += -ldelayimp
	QMAKE_LFLAGS += /DELAYLOAD:Qt5Core.dll /DELAYLOAD:Qt5Gui.dll /DELAYLOAD:Qt5Widgets.dll
}

HEADERS += Environment.h
SOURCES += Environment.cpp