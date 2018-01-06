TARGET = Run
TEMPLATE = app
QT -= gui

windows {
	# Remove this line when compile as Windows gui application
	# 编译成 Windows Gui 应用程序时，移除这行
	CONFIG += console
	INCLUDEPATH += ../Environment
	LIBS += -L../Environment/Release
	LIBS += -lEnvironment
}

SOURCES += Run.cpp