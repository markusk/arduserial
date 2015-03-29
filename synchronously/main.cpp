#include "arduserial.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ArduSerial w;
	w.show();

	return a.exec();
}
