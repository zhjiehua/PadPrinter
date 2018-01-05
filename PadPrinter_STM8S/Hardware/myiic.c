#include "myiic.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#if 1

void delay_us(uint8_t n)
{
    uint8_t i;
    for(i=0;i<n;i++)
    {
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
        //nop();
    }
}

//��ʼ��IIC
void IIC_Init(void)
{					     
    //����STM8��Ƭ�����������������������¶�ȡIO��״̬����ֱ������Ϊ�����
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_OUT_OD_HIZ_FAST  GPIO_MODE_OUT_OD_HIZ_SLOW 
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_IN_PU_NO_IT  GPIO_MODE_IN_FL_NO_IT
    IIC_SCL_H;
    IIC_SDA_H;
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_H; 
	IIC_SDA_H;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_H;delay_us(1);	   
	IIC_SCL_H;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_H;
		else
			IIC_SDA_L;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H;
		delay_us(2); 
		IIC_SCL_L;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL_L; 
        delay_us(2);
		IIC_SCL_H;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

#else

void delay_us()
{
    nop();
    nop();
    nop();
    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
//    nop();
}

//��ʼ��IIC
void IIC_Init(void)
{                        
    //����STM8��Ƭ�����������������������¶�ȡIO��״̬����ֱ������Ϊ�����
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_OUT_OD_HIZ_FAST  GPIO_MODE_OUT_OD_HIZ_SLOW 
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_IN_PU_NO_IT  GPIO_MODE_IN_FL_NO_IT
    IIC_SCL_H;
    IIC_SDA_H;
}


//����IIC��ʼ�ź�
void IIC_Start(void)
{
    disableInterrupts();
    
    IIC_SDA_H;                    
    IIC_SCL_H;
    delay_us();
    IIC_SDA_L; //START:when CLK is high,DATA change form high to low 
    delay_us();
    IIC_SCL_L; //ǯסI2C���ߣ�׼�����ͻ�������� 
}


//����IICֹͣ�ź�
void IIC_Stop(void)
{
    IIC_SCL_L;

    IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
    delay_us();
    IIC_SCL_H; 
    IIC_SDA_H;//����I2C���߽����ź�
    delay_us();
    
    enableInterrupts();
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;

    IIC_SDA_H;nop();           
    IIC_SCL_H;nop();
    while(IIC_SDA_R)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_L;//ʱ�����0            
    return 0;  
}


//����ACKӦ��
void IIC_Ack(void)
{
    IIC_SCL_L;
    IIC_SDA_L;
    delay_us();
    IIC_SCL_H;
    delay_us();
    IIC_SCL_L;
}

//������ACKӦ��                    
void IIC_NAck(void)
{
    IIC_SCL_L;
    IIC_SDA_H;
    delay_us();
    IIC_SCL_H;
    delay_us();
    IIC_SCL_L;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��                          
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;       
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7)
            IIC_SDA_H;
        else
            IIC_SDA_L;
        txd<<=1;           
        delay_us();   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL_H;
        delay_us(); 
        IIC_SCL_L;        
        delay_us();
    }         
}


//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
     unsigned char i,receive=0;
     for(i=0;i<8;i++ )

    {
        IIC_SCL_L; 
        delay_us();
        IIC_SCL_H;
        receive<<=1;
        if(IIC_SDA_R)receive++;   
        nop(); 
    }                                         
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

#endif

























