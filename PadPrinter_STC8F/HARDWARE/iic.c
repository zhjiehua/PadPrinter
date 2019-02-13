#include "iic.h"

//初始化IIC
void IIC_Init(void)
{
    IIC_SCL = 1;
    IIC_SDA = 1;
    			
	P_SW2 = 0x80;
    I2CCFG = 0xe0;                              //使能I2C主机模式
    I2CMSST = 0x00;
    P_SW2 = 0x00;
}

void Wait(void)
{
    uint16_t ucErrTime=0;
    
//    while (!(I2CMSST & 0x40));
    while (!(I2CMSST & 0x40))
    {
        ucErrTime++;
		if(ucErrTime>1250)
		{
			return;
		}
    }
    I2CMSST &= ~0x40;
}

void IIC_Start(void)
{
    I2CMSCR = 0x01;                             //发送START命令
    Wait();
}

void IIC_Send_Byte(char txd)
{
    I2CTXD = txd;                               //写数据到数据缓冲区
    I2CMSCR = 0x02;                             //发送SEND命令
    Wait();
}

void IIC_Wait_Ack(void)
{
    I2CMSCR = 0x03;                             //发送读ACK命令
    Wait();
}

uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t rxd;
    I2CMSCR = 0x04;                             //发送RECV命令
    Wait();
    rxd = I2CRXD;
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return rxd;
}

void IIC_Ack(void)
{
    I2CMSST = 0x00;                             //设置ACK信号
    I2CMSCR = 0x05;                             //发送ACK命令
    Wait();
}

void IIC_NAck()
{
    I2CMSST = 0x01;                             //设置NAK信号
    I2CMSCR = 0x05;                             //发送ACK命令
    Wait();
}

void IIC_Stop(void)
{
    I2CMSCR = 0x06;                             //发送STOP命令
    Wait();
}


























