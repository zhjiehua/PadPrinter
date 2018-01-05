#include "myiic.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
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

//初始化IIC
void IIC_Init(void)
{					     
    //由于STM8单片机，可以在输入和输出条件下读取IO口状态，故直接设置为输出。
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_OUT_OD_HIZ_FAST  GPIO_MODE_OUT_OD_HIZ_SLOW 
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_IN_PU_NO_IT  GPIO_MODE_IN_FL_NO_IT
    IIC_SCL_H;
    IIC_SDA_H;
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_H; 
	IIC_SDA_H;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL_L;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_H;
		else
			IIC_SDA_L;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL_H;
		delay_us(2); 
		IIC_SCL_L;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
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

//初始化IIC
void IIC_Init(void)
{                        
    //由于STM8单片机，可以在输入和输出条件下读取IO口状态，故直接设置为输出。
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_OUT_OD_HIZ_FAST  GPIO_MODE_OUT_OD_HIZ_SLOW 
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW );//GPIO_MODE_IN_PU_NO_IT  GPIO_MODE_IN_FL_NO_IT
    IIC_SCL_H;
    IIC_SDA_H;
}


//产生IIC起始信号
void IIC_Start(void)
{
    disableInterrupts();
    
    IIC_SDA_H;                    
    IIC_SCL_H;
    delay_us();
    IIC_SDA_L; //START:when CLK is high,DATA change form high to low 
    delay_us();
    IIC_SCL_L; //钳住I2C总线，准备发送或接收数据 
}


//产生IIC停止信号
void IIC_Stop(void)
{
    IIC_SCL_L;

    IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
    delay_us();
    IIC_SCL_H; 
    IIC_SDA_H;//发送I2C总线结束信号
    delay_us();
    
    enableInterrupts();
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
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
    IIC_SCL_L;//时钟输出0            
    return 0;  
}


//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL_L;
    IIC_SDA_L;
    delay_us();
    IIC_SCL_H;
    delay_us();
    IIC_SCL_L;
}

//不产生ACK应答                    
void IIC_NAck(void)
{
    IIC_SCL_L;
    IIC_SDA_H;
    delay_us();
    IIC_SCL_H;
    delay_us();
    IIC_SCL_L;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答                          
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;       
    IIC_SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7)
            IIC_SDA_H;
        else
            IIC_SDA_L;
        txd<<=1;           
        delay_us();   //对TEA5767这三个延时都是必须的
        IIC_SCL_H;
        delay_us(); 
        IIC_SCL_L;        
        delay_us();
    }         
}


//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

#endif

























