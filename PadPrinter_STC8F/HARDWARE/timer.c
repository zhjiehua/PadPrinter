#include "timer.h"
#include "io.h"
#include "stddef.h"
#include "stepmotor.h"
#include "management.h"

void Timer3_Cmd(uint8_t enable)
{
    if(enable)
        T4T3M |= 0x08;                               //启动定时器
    else
    {
        T4T3M &= ~0x08;
        MBEEP = 0;
    }     
}

void Timer3_Init(void)
{
	T3L = (65536 - (FOSC/12/(BEEPER_PEROID/2)));
    T3H = (65536 - (FOSC/12/(BEEPER_PEROID/2))) >> 8;
	T4T3M &= ~0x0F; //bit4-停止定时器，bit3-定时模式，bit2-12倍速，bit0-定时时钟输出
    //T4T3M |= 0x08;                               //启动定时器
    T4T3M &= ~0x08;
    IE2 |= ET3;
}

void Timer4_Cmd(uint8_t enable)
{
    if(enable)
        T4T3M |= 0x80;                               //启动定时器
    else
        T4T3M &= ~0x80;     
}

void Timer4_Init(void)
{
	T4L = TIMEOUT;          
    T4H = TIMEOUT >> 8;
	T4T3M &= ~0xF0;
    T4T3M |= 0x80;                               //启动定时器
    //T4T3M &= ~0x80;
    IE2 |= ET4;                                //使能定时器中断
}

//-----------------------------------------------
/* Timer0 interrupt routine */
//void Timer0_Isr() interrupt 1 using 3
//{
////    TL0 = timer_TH_TL[stepMotorSpeedIndex][1];  //initial timer1 low byte
////    TH0 = timer_TH_TL[stepMotorSpeedIndex][0];  //initial timer1 high byte
//
////	if(stepMotorStep)
////		stepMotorStep--;
////	else
////	{
////		ET0 = 0;
////		TR0 = 0;
////	}
//
//	//MBEEP = !MBEEP;                                  //测试端口
//}

void TM3_Isr() interrupt 19 using 3
{
    T3L = (65536 - (FOSC/12/(BEEPER_PEROID/2)));
    T3H = (65536 - (FOSC/12/(BEEPER_PEROID/2))) >> 8;

    MBEEP = !MBEEP;                                  //测试端口

    //LEDM = !LEDM;
    AUXINTIF &= ~T3IF;                          //清中断标志
}

//void TM4_Isr() interrupt 20 using 3
//{
//    //MBEEP = !MBEEP;                                 //测试端口
//    AUXINTIF &= ~T4IF;                          //清中断标志
//}

//----------------------------------------------------------------
void Delay1us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 3;
	while (--i);
}

void Delay10us()		//@24.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 57;
	while (--i);
}

void Delay100us()		//@24.000MHz
{
	unsigned char i, j;

	i = 3;
	j = 82;
	do
	{
		while (--j);
	} while (--i);
}

void Delay500us()		//@24.000MHz
{
	unsigned char i, j;

	i = 12;
	j = 169;
	do
	{
		while (--j);
	} while (--i);
}

void Delay1ms()		//@24.000MHz
{
	unsigned char i, j;

	i = 24;
	j = 85;
	do
	{
		while (--j);
	} while (--i);
}

//void Delay10ms()		//@24.000MHz
//{
//	unsigned char i, j, k;
//
//	_nop_();
//	_nop_();
//	i = 1;
//	j = 234;
//	k = 113;
//	do
//	{
//		do
//		{
//			while (--k);
//		} while (--j);
//	} while (--i);
//}

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 56;
	k = 172;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay100ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
