#include <QApplication>
#include <QDir>
#include "Environment.h"

typedef void (Callback)();

extern "C" {
	int argumentCount;
	char **argumentValues;

	void Environment_load(int argc, char **argv) {
		argumentCount = argc;
		argumentValues = argv;
	}

	Q_DECL_EXPORT void Environment_initialize(uchar type, Callback *callback) {
		void *app = NULL;
		if (type == 0) {
			(*callback)();
		} else {
			switch (type) {
				case 1:
					app = new QCoreApplication(argumentCount, argumentValues);
					break;
				case 2:
					app = new QGuiApplication(argumentCount, argumentValues);
					break;
				case 3:
					app = new QApplication(argumentCount, argumentValues);
					break;
			}
			(*callback)();
			((QCoreApplication *)app)->exec();
		}
	}
}