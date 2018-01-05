#ifndef __MYIIC_H
#define __MYIIC_H

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

#include "stm8s.h"
//#include "stm8s_gpio.h"

//IO��������
#define SDA_IN()  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT )
#define SDA_OUT() GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW )

//#define SDA_IN()     do{GPIOB->DDR|=0xdf;GPIOB->CR1|=0x20;GPIOB->CR2&=0xdf;}while(0)
//#define SDA_OUT()    do{GPIOB->DDR|=0x20;GPIOB->CR1|=0x20;GPIOB->CR2&=0xdf;}while(0)

//SCL-PB4
#define IIC_SCL_H     (GPIOB->ODR |=  (1 << 4))
#define IIC_SCL_L     (GPIOB->ODR &= ~(1 << 4))

//SDA-PB5
#define IIC_SDA_H     (GPIOB->ODR |=  (1 << 5))
#define IIC_SDA_L     (GPIOB->ODR &= ~(1 << 5))
#define READ_SDA      ((GPIOB->IDR &  (1 << 5))>>5)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	

#else

#include "stm8s.h"

//SCL-PB4
#define IIC_SCL_H     (GPIOB->ODR |=  (1 << 4))
#define IIC_SCL_L     (GPIOB->ODR &= ~(1 << 4))

//SDA-PB5
#define IIC_SDA_H     (GPIOB->ODR |=  (1 << 5))
#define IIC_SDA_L     (GPIOB->ODR &= ~(1 << 5))
#define IIC_SDA_R     ((GPIOB->IDR &  (1 << 5))>>5)


//IIC??????
void IIC_Init(void);                //???IIC?IO?                                 
void IIC_Start(void);                                //??IIC????
void IIC_Stop(void);                                  //??IIC????
void IIC_Send_Byte(u8 txd);                        //IIC??????
u8 IIC_Read_Byte(unsigned char ack);//IIC??????
u8 IIC_Wait_Ack(void);                                 //IIC??ACK??
void IIC_Ack(void);                                        //IIC??ACK??
void IIC_NAck(void);                                //IIC???ACK??

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

#endif

#endif
















