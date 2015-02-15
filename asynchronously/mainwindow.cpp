#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	// show MainWindow (GUI)
	ui->setupUi(this);

	// display message in GUI
	ui->textEdit->insertHtml("Scanning for serial ports... ");

	// get a list of available serial ports.
	// this is not used in the code and only for demontration.
	QList <QextPortInfo> ports = QextSerialEnumerator::getPorts();

	ui->textEdit->insertHtml("Done.<br><br>");

	// for displaying the number of found ports
	n = 1;

	foreach (QextPortInfo portInfo, ports)
	{
		// display found ports in GUI
		ui->textEdit->insertHtml(QString("<b><u>Port %1</u></b><br>").arg(n));

		showPorts(portInfo);

		// n plus 1
		n++;
	}

	ui->textEdit->insertHtml("<br><br>");

	// if a USB device is added or removed, call the Slot onPortAddedOrRemoved
	enumerator = new QextSerialEnumerator(this);
	enumerator->setUpNotifications();

	connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAdded(QextPortInfo)));
	connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortRemoved(QextPortInfo)));

	//--------------------------------------------------------------------------------------------------
	// the settings for the serial port
	// Be aware, that the Arduino has to use the same speed (9600 Baud)
	PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10}; // 10 = timeout in ms
	//--------------------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------------------
	// the name of the serial port
	// on Windows, this would be i.e. COM5
	serialPortName = "/dev/tty.usbmodemfd1411";
	//--------------------------------------------------------------------------------------------------

	// create the serial port object.
	// we get the serial data on the port asynchronously!
	port = new QextSerialPort(serialPortName, settings);

	// try to open Arduino serial port
	initArduino();
}


MainWindow::~MainWindow()
{
	delete ui;
	delete port;
}


void MainWindow::initArduino()
{
	// initialise the serial port
	if (openSerialPort() == false)
	{
		// ERROR !!

		return;
	}

	// display message in GUI
	ui->textEdit->insertHtml("<b>Sending data to Arduino in some seconds (arduinoInit)...</b> ");

	// Special timer, needed for Arduino!
	//
	// Reason:
	// When the serial (USB) port is opened, the Arduino is not ready for serial communication immediately.
	// Therefore we start a timer. After 3000 ms (3 seconds), it will call the function arduinoInit().
	// This can then be used for a first command to the Arduino, like "Hey Arduino, Qt-Software now startet!".
	QTimer::singleShot(3000, this, SLOT(timerSlot()));
}


bool MainWindow::openSerialPort(void)
{
	// open the serial port
	port->open(QIODevice::ReadWrite | QIODevice::Unbuffered);

	// error opening port
	if (port->isOpen() == false)
	{
		// show error message
		ui->textEdit->insertHtml(QString("<b>Error opening serial port <i>%1</i>.</b><br>").arg(serialPortName));

		return false;
	}

	// success
	return true;
}


void MainWindow::sendValue(int value)
{
	QByteArray byte; // byte to sent to the port
	qint64 bw = 0;   // bytes really written


	byte.clear(); // clear buffer to be sent
	byte.append(value); // fill buffer with value to be sent

	if (port != NULL)
	{
		// write byte to serial port
		bw = port->write(byte);

		// show sent data
		ui->textEdit->insertHtml(QString("%1 byte(s) written. Written value: %2 (ASCII) / %3 (DEC) / %4 (HEX)<br>").arg(bw).arg(QChar(value)).arg(value).arg(value, 0, 16));

		// flush serial port
		port->flush();
	}
}


void MainWindow::timerSlot()
{
	QTime startTime; // For measuring elapsed time while waiting for an answer on the serial port
	qint64 ba = 0; // bytes available on the serial port
	qint64 bytesRead = 0;
	char buf[1024]; // stores all recieved data from the serial port

	// the following varaibles are not needed an only fro displaying different formats in the GUI
	QChar ch = 0; // the char of the received data
	int dec = 0;  // the int of the received data
	QString str;  // a string to show the received data


	int i = 0;

	// show message
	ui->textEdit->insertHtml(QString("<b> %1# Sending!</b><br><br>").arg(i));

	// send values to Arduino (insert your own initialisation here!)
	sendValue('*');
	sendValue('r');
	sendValue('e');
	sendValue('#');

for (i = 0; i<20; i++)
{
	// show message
	ui->textEdit->insertHtml(QString("<b> %1# Sending!</b><br><br>").arg(i));

	// send values to Arduino (insert your own initialisation here!)
	sendValue('*');
	sendValue('s');
	sendValue('7');
	sendValue('#');

	// show message
	ui->textEdit->insertHtml("<br><b>Waiting for Arduino answer...</b><br><br>");

	// just to make sure...
	if (port->isOpen() == false)
	{
		ui->textEdit->insertHtml("ERROR: serial port not opened!");

		return;
	}

	// check if the Arduino sends all data within an wanted time...
	startTime.start();

	do
	{
		// how many bytes are available?
		ba = port->bytesAvailable();

		// if data available (should _always_ be the case, since this method is called automatically by an event)
		if (ba > 0)
		{
			// show message
			ui->textEdit->insertHtml(QString("<em>%1 byte(s) available.</em>").arg(ba));

			//--------------------------------------------------------------------
			// read a maximum of 'ba' available bytes into the buffer as char *
			//--------------------------------------------------------------------
			bytesRead = port->read(buf, ba);

			// ERROR
			if (bytesRead == -1)
			{
				// show error code and message
				ui->textEdit->insertHtml(QString("ERROR %1 at readData: %2").arg(bytesRead).arg(port->errorString()));

				return;
			}

			// show message
			ui->textEdit->insertHtml(QString("<em>%1 byte(s) received.</em><br>").arg(bytesRead));

			// position in the string (index)
			n = 0;

			// show each byte
			while (n < bytesRead)
			{
				// convert char to int
				dec = (int) buf[n];

				// convert chcar to QChar
				ch = buf[n];

				// build a QString for convenience
				str.append(ch);

				// show in GUI
				ui->textEdit->insertHtml(QString("Byte No.%1: %2 (ASCII) / %3 (DEC) / %4 (HEX)<br>").arg(n+1).arg(ch).arg(dec).arg(dec, 0, 16));

				// counter +1
				n++;
			}

			// add a new line
			ui->textEdit->insertHtml("<br>");

			// scroll text edit in GUI to cursor
			ui->textEdit->ensureCursorVisible();

		} // bytes available

	} while (startTime.elapsed() < serialReadTimout);

	// show whole string in GUI
	ui->textEdit->insertHtml(QString("Complete String: %1").arg(str));

	// scroll text edit in GUI to cursor
	ui->textEdit->ensureCursorVisible();


	//
	// r e s e t
	//
	QCoreApplication::processEvents();
	ba = 0;
	bytesRead = 0;
	str.clear();

	ui->textEdit->insertHtml("<br>");
} // for
}


void MainWindow::onPortAdded(QextPortInfo newPortInfo)
{
	// get part of string
	// (i.e. looking only for the "usbmodem1451" within "/dev/tty.usbmodem1451")
	QStringRef subString = serialPortName.rightRef(serialPortName.lastIndexOf("."));


	// scroll to end
	ui->textEdit->ensureCursorVisible();
	ui->textEdit->insertHtml("<br><b><i>Serial port added!</i></b><br>");

	// show ports, with physical name
	showPorts(newPortInfo, true);

	// Wanted serial port (Arduino) found!
	if (newPortInfo.portName.contains(subString))
	{
		// show success message!
		ui->textEdit->insertHtml("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++<br>");
		ui->textEdit->insertHtml(QString("+++ Yeah, Arduino '%1' found! +++<br>").arg(subString.toString()));
		ui->textEdit->insertHtml("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++<br><br>");

		// try to open Arduino serial port
		initArduino();
	}
}


void MainWindow::onPortRemoved(QextPortInfo newPortInfo)
{
	// scroll to end
	ui->textEdit->ensureCursorVisible();
	ui->textEdit->insertHtml("<br><b><i>Serial port removed!</i></b><br>");

	// show ports
	showPorts(newPortInfo);
}


void MainWindow::showPorts(QextPortInfo portInfos, bool added)
{
	ui->textEdit->insertHtml(QString("<b>Port name:</b> %1<br>").arg(portInfos.portName));

	if (added)
	{
		ui->textEdit->insertHtml(QString("<b>Physical name:</b> %1<br>").arg(portInfos.physName));
	}

	ui->textEdit->insertHtml(QString("<b>Friendly name:</b> %1<br>").arg(portInfos.friendName));
	ui->textEdit->insertHtml(QString("<b>Enumerator name:</b> %1<br>").arg(portInfos.enumName));
	ui->textEdit->insertHtml(QString("<b>Vendor ID:</b> %1<br>").arg(portInfos.vendorID));
	ui->textEdit->insertHtml(QString("<b>Product ID:</b> %1<br>").arg(portInfos.productID));
	ui->textEdit->insertHtml("<br>");

	// scroll to end
	ui->textEdit->ensureCursorVisible();
}
