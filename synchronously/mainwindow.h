#ifndef ARDUSERIAL_H
#define ARDUSERIAL_H

#include <QMainWindow>
#include <QTimer>
#include <QTime> /// For measuring elapsed time while waiting for an answer on the serial port
#include <qextserialport.h> /// This is for serial port communication
#include <qextserialenumerator.h> /// This is for getting a list of serial ports

namespace Ui
{
	class ArduSerial;
}

class ArduSerial : public QMainWindow
{
	Q_OBJECT

public:
	explicit ArduSerial(QWidget *parent = 0);
	~ArduSerial();

	/**
	 * @brief initSerialPort Initialisation of the serial port
	 * @return true on success
	 */
	bool openSerialPort();

	/**
	 * @brief sendValue sends a value to the Arduino.
	 * @param value which will be sent to the Arduino
	 */
	void sendValue(int value);


private slots:
	/**
	 * @brief timerSlot Sends init data to the Arduino.
	 * Please note, that this is a Slot, called by the @sa timerEvent
	 */
	void timerSlot();

	/**
	 * @brief onPortAdded is called, if a USB device is added.
	 */
	void onPortAdded(QextPortInfo newPortInfo);

	/**
	 * @brief onPortRemoved is called, if a USB device is removed.
	 */
	void onPortRemoved(QextPortInfo newPortInfo);

	/**
	 * @brief showPorts shows the ports found by event.
	 * @param portInfos contains the found port to be shown
	 * @param added can be set to true. Only then the physical name will be shown.
	 */
	void showPorts(QextPortInfo portInfos, bool added = false);


private:
	/**
	 * @brief initArduino initialises my Arduino with my own instructions. It also waits for an answer and reads this!
	 */
	void initArduino();
	QString timestamp(QString text);


	Ui::MainWindow *ui; /// The main window (GUI)
	QextSerialPort *port; /// The serial port
	QextSerialEnumerator *enumerator; /// This is for getting a list of serial ports (filenames like /dev/ttyUSB0)
	QString serialPortName; /// for the (file)name of the serial port, like /dev/ttyUSB0 or COM1
	int n;
	static const int serialReadTimout = 500; // time in ms for waiting for an answer for all bytes
};

#endif // ARDUSERIAL_H
