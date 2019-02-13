#include "iic.h"

//��ʼ��IIC
void IIC_Init(void)
{
    IIC_SCL = 1;
    IIC_SDA = 1;
    			
	P_SW2 = 0x80;
    I2CCFG = 0xe0;                              //ʹ��I2C����ģʽ
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
    I2CMSCR = 0x01;                             //����START����
    Wait();
}

void IIC_Send_Byte(char txd)
{
    I2CTXD = txd;                               //д���ݵ����ݻ�����
    I2CMSCR = 0x02;                             //����SEND����
    Wait();
}

void IIC_Wait_Ack(void)
{
    I2CMSCR = 0x03;                             //���Ͷ�ACK����
    Wait();
}

uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t rxd;
    I2CMSCR = 0x04;                             //����RECV����
    Wait();
    rxd = I2CRXD;
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return rxd;
}

void IIC_Ack(void)
{
    I2CMSST = 0x00;                             //����ACK�ź�
    I2CMSCR = 0x05;                             //����ACK����
    Wait();
}

void IIC_NAck()
{
    I2CMSST = 0x01;                             //����NAK�ź�
    I2CMSCR = 0x05;                             //����ACK����
    Wait();
}

void IIC_Stop(void)
{
    I2CMSCR = 0x06;                             //����STOP����
    Wait();
}


























