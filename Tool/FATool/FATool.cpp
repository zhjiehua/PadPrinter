#include "FATool.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include "Misc.h"


#define CMD_HEAD	0xA55A


FATool::FATool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	bgRcv = new QButtonGroup(this);
	bgSend = new QButtonGroup(this);
	bgMachineType = new QButtonGroup(this);
	bgRcv->addButton(ui.rdbRcvHex, 0);
	bgRcv->addButton(ui.rdbRcvASCII, 1);
	bgSend->addButton(ui.rdbSendHex, 0);
	bgSend->addButton(ui.rdbSendASCII, 1);
	bgMachineType->addButton(ui.rb2Sensors5V, 0);
	bgMachineType->addButton(ui.rb3Sensors12V, 1);
	bgMachineType->addButton(ui.rb2Sensors12V, 2);
	ui.rdbRcvASCII->setChecked(true);
	//ui.rdbSendASCII->setChecked(true);
	ui.rdbSendHex->setChecked(true);
	ui.rb2Sensors5V->setChecked(true);
	connect(bgRcv, SIGNAL(buttonClicked(int)), this, SLOT(bgRcvButtonsClicked(int)));
	connect(bgSend, SIGNAL(buttonClicked(int)), this, SLOT(bgSendButtonsClicked(int)));
	connect(bgMachineType, SIGNAL(buttonClicked(int)), this, SLOT(bgMachineTypeButtonsClicked(int)));

	InitPort();//初始化串口

	//定时读取串口数据
	mainTimer = new QTimer(this);
	connect(mainTimer, SIGNAL(timeout()), this, SLOT(UartDataParse()));
	mainTimer->start(5);

	
}


/*********************************************************************************
*
*
*
*
**********************************************************************************/


/****************************串口初始化*****************************/
void FATool::InitPort(void)
{
	qDebug() << "InitPort\r\n";
	qDebug() << "InitPort\r\n";
	qDebug() << "InitPort\r\n";

	//读取串口信息  
	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		qDebug() << "Name:" << info.portName();
		qDebug() << "Description:" << info.description();
		qDebug() << "Manufacturer:" << info.manufacturer();

		//这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去  
		QSerialPort serial;
		serial.setPort(info);
		if (serial.open(QIODevice::ReadWrite))
		{
			//将串口号添加到cmb  
			ui.cmbPortName->addItem(info.portName());
			//关闭串口等待人为(打开串口按钮)打开  
			serial.close();
		}
	}

	QStringList baudList;//波特率  
	QStringList parityList;//校验位  
	QStringList dataBitsList;//数据位  
	QStringList stopBitsList;//停止位  

	baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300"
		<< "600" << "1200" << "1800" << "2400" << "4800" << "9600"
		<< "14400" << "19200" << "38400" << "56000" << "57600"
		<< "76800" << "115200" << "128000" << "256000";

	ui.cmbBaudRate->addItems(baudList);
	ui.cmbBaudRate->setCurrentIndex(19); //115200
										 //ui.cmbBaudRate->setCurrentIndex(12); //9600

	parityList << QString::fromLocal8Bit("无") << QString::fromLocal8Bit("奇") << QString::fromLocal8Bit("偶");
	parityList << QString::fromLocal8Bit("标志");
	parityList << QString::fromLocal8Bit("空格");

	ui.cmbParity->addItems(parityList);
	ui.cmbParity->setCurrentIndex(0);

	dataBitsList << "5" << "6" << "7" << "8";
	ui.cmbDataBits->addItems(dataBitsList);
	ui.cmbDataBits->setCurrentIndex(3);

	stopBitsList << "1";
	stopBitsList << "1.5";
	stopBitsList << "2";

	ui.cmbStopBits->addItems(stopBitsList);
	ui.cmbStopBits->setCurrentIndex(0);

	//设置按钮可以被按下  
	ui.btnOpen->setCheckable(true);

	ui.teReceiveData->setReadOnly(true);
}

/****************************串口设置******************************/
void FATool::on_btnOpen_clicked()
{
	if (ui.btnOpen->text() == QString::fromLocal8Bit("打开串口"))
	{
		serialPort = new QSerialPort(this);

		//设置串口号  
		serialPort->setPortName(ui.cmbPortName->currentText());
		//以读写方式打开串口  
		if (serialPort->open(QIODevice::ReadWrite))
		{
			//设置波特率  
			serialPort->setBaudRate(ui.cmbBaudRate->currentText().toInt());
			//设置数据位  
			serialPort->setDataBits(QSerialPort::Data8);
			//设置校验位  
			serialPort->setParity(QSerialPort::NoParity);
			//设置流控制  
			serialPort->setFlowControl(QSerialPort::NoFlowControl);
			//设置停止位  
			serialPort->setStopBits(QSerialPort::OneStop);

			//每秒读一次  
			timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(ReadComDataSlot()));
			timer->start(100);

			SetNonSelectable();
		}
		else
		{
			QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
			return;
		}
	}
	else
	{
		timer->stop();
		SetSelectable();
		serialPort->close();
	}

	//测试FFT
	//FFT_Test();

}

void FATool::SetNonSelectable(void)
{
	ui.btnOpen->setText(QString::fromLocal8Bit("关闭串口"));

	ui.cmbPortName->setEditable(false);
	ui.cmbBaudRate->setEditable(false);
	ui.cmbDataBits->setEditable(false);
	ui.cmbParity->setEditable(false);
	ui.cmbStopBits->setEditable(false);
	ui.cmbFlowControl->setEditable(false);

	ui.cmbPortName->hidePopup();
}

void FATool::SetSelectable(void)
{
	ui.btnOpen->setText(QString::fromLocal8Bit("打开串口"));

	ui.cmbPortName->setEditable(true);
	ui.cmbBaudRate->setEditable(true);
	ui.cmbDataBits->setEditable(true);
	ui.cmbParity->setEditable(true);
	ui.cmbStopBits->setEditable(true);
	ui.cmbFlowControl->setEditable(true);

	ui.cmbPortName->showPopup();
}

/****************************数据传输******************************/
void FATool::ReadComDataSlot()
{
	//读取串口数据  
	QByteArray readComData = serialPort->readAll();

	//将读到的数据显示到数据接收区的te中  
	if (!readComData.isNull())
	{
		if (bgRcv->checkedId() == 0)//hex
		{
			ui.teReceiveData->append(readComData.toHex());
		}
		else//ascii
		{
			ui.teReceiveData->append(readComData.data());
		}


		uartBuffer.append(readComData);

		qDebug() << readComData;
	}

	//清除缓冲区  
	readComData.clear();
}

void FATool::on_btnSend_clicked()
{
	if (bgSend->checkedId() == 0)//hex
	{
		QString sendData = ui.teSendData->toPlainText();
		sendBufferHex = Misc::QString2Hex(sendData);

		//写入缓冲区
		if (serialPort)
			serialPort->write(sendBufferHex);
		else
			QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
	}
	else//ascii
	{
		sendBufferASCII = ui.teSendData->toPlainText();

		//写入缓冲区 
		if (serialPort)
			serialPort->write(sendBufferASCII.toLatin1());
		else
			QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
	}
}

void FATool::on_btnClrRcv_clicked()
{
	ui.teReceiveData->clear();
	uartBuffer.clear();
}


void FATool::on_btnResetDefault_clicked()
{
	QByteArray sendData;
	sendData.append('R');
	sendData.append('1');

	sendData.append('\r');
	sendData.append('\n');

	//写入缓冲区
	if (serialPort)
		serialPort->write(sendData);
	else
		QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
}

void FATool::on_btnInitBoardID_clicked()
{
	QByteArray sendData;
	sendData.append('E');
	sendData.append('4');

	sendData.append('\r');
	sendData.append('\n');

	//写入缓冲区
	if (serialPort)
		serialPort->write(sendData);
	else
		QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
}

void FATool::on_btnRestart_clicked()
{
	QByteArray sendData;
	sendData.append('R');
	sendData.append('s');

	sendData.append('\r');
	sendData.append('\n');

	//写入缓冲区
	if (serialPort)
		serialPort->write(sendData);
	else
		QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
}

void FATool::on_btnClsBoardID_clicked()
{
	quint8 i;
	QByteArray sendData;
	sendData.append('E');
	sendData.append('3');

	for (i = 0; i < 7; i++)
	{
		sendData.append(0xff);
	}

	sendData.append('\r');
	sendData.append('\n');

	//写入缓冲区
	if (serialPort)
		serialPort->write(sendData);
	else
		QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
}



void FATool::bgRcvButtonsClicked(int id)
{
	if (bgRcv->checkedId() == 0)//hex
	{
		ui.teReceiveData->clear();
		ui.teReceiveData->append(uartBuffer.toHex());
	}
	else//ascii
	{
		ui.teReceiveData->clear();
		ui.teReceiveData->append(uartBuffer.data());
	}
}

void FATool::bgSendButtonsClicked(int id)
{
	if (bgSend->checkedId() == 0)//hex
	{
		sendBufferASCII = ui.teSendData->toPlainText();

		ui.teSendData->clear();
		ui.teSendData->append(sendBufferHex.toHex());
	}
	else//ascii
	{
		QString sendData = ui.teSendData->toPlainText();
		sendBufferHex = Misc::QString2Hex(sendData);

		ui.teSendData->clear();
		ui.teSendData->append(sendBufferASCII);
	}
}

void FATool::bgMachineTypeButtonsClicked(int id)
{
	QByteArray sendData;
	sendData.append('S');

	if (bgMachineType->checkedId() == 0)//2Sensors5V
	{
		sendData.append('0');
	}
	else if(bgMachineType->checkedId() == 1)//3Sensors12V
	{
		sendData.append('1');
	}
	else //2Sensors12V
	{
		sendData.append('2');
	}

	sendData.append('\r');
	sendData.append('\n');

	//写入缓冲区
	if (serialPort)
		serialPort->write(sendData);
	else
		QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));
}


quint32 FATool::BufferFindCMD(void)
{
	quint32 cmdSize = 0;
	quint32 size;
	char data;
	static quint32 cmdPos = 0;
	static quint16 cmdHead = 0;
	static quint8 cmdLen = 3;

	while (uartBuffer.size())
	{
		data = uartBuffer.at(0);
		uartBuffer.remove(0, 1);

		cmdHead = (cmdHead << 8) | data;

		if (cmdPos == 0)
		{
			if (cmdHead != CMD_HEAD)//指令第一个字节必须是帧头，否则跳过
				continue;
			else
			{
				dataBuffer.append((char)(CMD_HEAD >> 8));
				cmdPos++;
			}
		}

		dataBuffer.append((char)data);
		cmdPos++;

		if (cmdPos == 3) cmdLen = data + 3;

		//凑够5字节得到完整帧
		if (cmdPos >= cmdLen)
		{
			cmdLen = 3;
			cmdSize = cmdPos; //指令字节长度
			cmdPos = 0; //复位指令指针
			cmdHead = 0;

			return cmdSize;
		}

	}
	return 0;//没有形成完整的一帧
}

void FATool::ProcessMessage(void)
{
	quint8 cmd = dataBuffer.at(3);
	quint8 temp[7], i, j;
	quint32 index;
	char *ch = dataBuffer.data();

	double a = 0;
	double b = 0;

	uint8_t maxNum;
	int32_t maxPos[3];

	switch (cmd)
	{
	case 0xE4: //系统变量
	{
		qDebug("MCUID = 0x%x%x%x%x%x%x%x\r\n", dataBuffer.at(4), dataBuffer.at(5), dataBuffer.at(6), 
			dataBuffer.at(7), dataBuffer.at(8), dataBuffer.at(9), dataBuffer.at(10));

		for (i = 0; i<7; i++)
		{
			temp[i] = 0;

			for (j = 0; j<8; j++)
			{
				if (dataBuffer.at(4+i) & (0x01 << j))
					temp[i] |= (0x80 >> j);
			}
		}

#define ACTIVEMASK 0x4A

		for (i = 0; i < 7; i++)
		{
			temp[i] = temp[i] ^ ACTIVEMASK;
		}

		//发送MCUID数据
		QByteArray sendData;
		sendData.append('E');
		sendData.append('3');

		for (i = 0; i < 7; i++)
		{
			sendData.append(temp[i]);
		}

		sendData.append('\r');
		sendData.append('\n');

		//写入缓冲区
		if (serialPort)
			serialPort->write(sendData);
		else
			QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), QStringLiteral("串口没有打开！"));

	}
	break;

	default:
		break;
	}

	dataBuffer.clear();
}

void FATool::UartDataParse()
{
	qint32 size;

	//解析串口数据
	size = BufferFindCMD(); //从缓冲区中获取一条指令        
	if (size > 0)//接收到指令
	{
		ProcessMessage();//指令处理
	}
}