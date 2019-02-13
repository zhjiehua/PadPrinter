#include "io.h"
#include "uart.h"
#include "misc.h"
#include "timer.h"
#include "tm770x.h"
#include "stepmotor.h"
#include "eeprom.h"

#include "misc.h"
#include "management.h"

//IO初始化
//输出口需要配置成推挽模式
void IO_Init(void)
{
#if !ARRAY_KEYBOARD

	//IO--------------------------------------------------------------------------------------
	P0M1 = 0x01;    //双双双双双双双双
	P0M0 = 0x00;
	P0 = 0xFF;    //-,-,-,-,MIN1,MIN3,MIN2,MIN6
	//----------------------
	P1M1 = 0x30;    //双双漏漏推推双双
	P1M0 = 0x3C;    //
	P1 = 0xF3;    //XTAL2,XTAL1,IIC_SCL,IIC_SDA,MOUT2,MOUT1,LCD_DIN,LCD_DOUT
	//----------------------
	P2M1 = 0xE0;    //双双双双双双双双
	P2M0 = 0x00;
	P2 = 0xFF;    //MIN9,MIN7,MIN8,MIN4,MIN5,DIS_DIO,DIS_CLK,DIS_STB
	//----------------------
	P3M1 = 0x00;    //推推推推双双双双
	P3M0 = 0xF0;
	P3 = 0x0F;    //MOUT6,MOUT5,LEDM,MBEEP,INT_SW8,INT_PD,M-TXD,M-RXD
	//----------------------
	P4M1 = 0x00;    //双双双双双双双双双
	P4M0 = 0x00;
	P4 = 0xFF;    //-,-,-,-,-,-,-,-
	
	P5M1 = 0x00;    //双双推推双双双双双
	P5M0 = 0x30;
	P5 = 0xCF;    //-,-,MOUT4,MOUT3,-,-,-,-

#else

	//IO--------------------------------------------------------------------------------------
	P0M1 = 0x01;    //双双双双双双双双
	P0M0 = 0x00;
	P0 = 0xFF;    //-,-,-,-,MIN1,MIN3,MIN2,MIN6
	//----------------------
	//P1M1 = 0x30;    //双双漏漏推推双双
	//P1M0 = 0x3C;    //
    P1M1 = 0x33;
    P1M0 = 0xFC;
	P1 = 0xF3;    //K_COL2,K_COL4,IIC_SCL,IIC_SDA,MOUT2,MOUT1,K_ROW2,K_ROW1
	//----------------------
	P2M1 = 0xE0;    //双双双双双双双双
	P2M0 = 0x00;
	P2 = 0xFF;    //MIN9,MIN7,MIN8,MIN4,MIN5,DIS_DIO,DIS_CLK,DIS_STB
	//----------------------
	P3M1 = 0x00;    //推推双推双双双双
	//P3M0 = 0xD0;
    P3M0 = 0xF4;
	P3 = 0x0F;    //MOUT6,MOUT5,K_COL1,MBEEP,INT_SW8,K_COL3,M-TXD,M-RXD
	//----------------------
	P4M1 = 0x00;    //双双双双双双双双双
	P4M0 = 0x00;
	P4 = 0xFF;    //-,-,-,-,-,-,-,-
	
	P5M1 = 0x00;    //双双推推双双双双双
	P5M0 = 0x30;
	P5 = 0xCF;    //-,-,MOUT4,MOUT3,-,-,-,-

#endif


	//SFR---------------------------------------------------------------------------
	WDT_CONTR = 0x00;         //WDT未启动
	PCON = 0x00; //bit7=0 串口波特率不加倍
	//SCON = 0x50;  MODE 1	可变波特率8位数据方式
	//TMOD = 0x00; //T0,MODE0,16位自动重装,T1 MODE1,16位自动重装
	//AUXR = 0xD5; //T0/T1/T2不分频,12T快速模式;T2为串口1的波特率发生器

	//TCON = 0x00; //

    //-------------------------------------------------------------
	//初始化IO
    //
    MOUT1 = 0;
    MOUT2 = 0;
    MOUT3 = 0;
    MOUT4 = 0;
    MOUT5 = 0;
    MOUT6 = 1;

//    //配置时钟，使用外部晶振
//    P_SW2 = 0x80;   
//    XOSCCR |= 0xC0; //启动外部晶振
//    while(XOSCCR&0x01 == 0x00); //外部晶振就绪
//    CKSEL = 0x01;                                //主时钟使用外部晶振
//    //CKSEL |= 0xA0;                               //主时钟32分频输出到P5.4口
//    CLKDIV = 0x00;                             //SYSclk = MCLK/1
//    //CLKDIV = 0x0A;                             //SYSclk = MCLK/10   //不能正常工作
//    IRC24MCR = 0x00;                            //关闭内部24MHz时钟
//    P_SW2 = 0x00;
}
