TARGET = Run
TEMPLATE = app
QT -= gui

windows {
	CONFIG += console
	INCLUDEPATH += ../Environment
	#LIBS += -LD:/Work/Build/gperftools/x64/Release
	LIBS += -L../Environment/Release
	LIBS += -llibtcmalloc_minimal -lEnvironment
	QMAKE_LFLAGS += /INCLUDE:"__tcmalloc"
}

SOURCES += Run.cpp