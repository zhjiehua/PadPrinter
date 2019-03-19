#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FATool.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class FATool : public QMainWindow
{
	Q_OBJECT

public:
	FATool(QWidget *parent = Q_NULLPTR);

	void InitPort(void);
	void SetNonSelectable(void);
	void SetSelectable(void);

	quint32 BufferFindCMD(void);
	void ProcessMessage(void);

	//void FFT_Test(void);

public slots:
	void on_btnOpen_clicked();   //
	void on_btnSend_clicked();
	void on_btnClrRcv_clicked();

	void on_btnResetDefault_clicked();
	void on_btnInitBoardID_clicked();
	void on_btnRestart_clicked();
	void on_btnClsBoardID_clicked();

	void bgRcvButtonsClicked(int id);
	void bgSendButtonsClicked(int id);
	void bgMachineTypeButtonsClicked(int id);

	void ReadComDataSlot();   //
	void UartDataParse();

	/*********************************************************************/

	/*********************************************************************/

private:
	Ui::FAToolClass ui;

	QButtonGroup *bgRcv;
	QButtonGroup *bgSend;
	QButtonGroup *bgMachineType;
	
	QSerialPort *serialPort;
	QTimer *timer;
	QTimer *mainTimer;
	QByteArray dataBuffer;
	QByteArray uartBuffer;
	QByteArray sendBufferHex;
	QString sendBufferASCII;

	//HMI_Printer_Driver *thermalPrinter;
};
