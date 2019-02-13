#ifndef __IO_H__
#define __IO_H__

#include "common.h"

//misc
//sbit LEDM = P3^5;
sbit MBEEP = P3^4;
//sbit INT_PD = P3^2;
sbit INT_SW8 = P3^3;

//KEY
sbit K_COL1 = P3^5;
sbit K_COL2 = P1^7;
sbit K_COL3 = P3^2;
sbit K_COL4 = P1^6;
sbit K_ROW1 = P1^0;
sbit K_ROW2 = P1^1;

//IO
sbit MIN1 = P0^3;
sbit MIN2 = P0^1;
sbit MIN3 = P0^2;
sbit MIN4 = P2^4;
sbit MIN5 = P2^3;
sbit MIN6 = P0^0;
sbit MIN7 = P2^6;
sbit MIN8 = P2^5;
sbit MIN9 = P2^7;
sbit MOUT1 = P1^2;
sbit MOUT2 = P1^3;
sbit MOUT3 = P5^4;
sbit MOUT4 = P5^5;
sbit MOUT5 = P3^6;
sbit MOUT6 = P3^7;

//TM1638
sbit DIS_DIO = P2^2;
sbit DIS_CLK = P2^1;
sbit DIS_STB = P2^0;

////EEPROM
////sbit IIC_WP = P2^7;
//sbit IIC_SCL = P1^5;
//sbit IIC_SDA = P1^4;

//LCD
//sbit LCD_BUSY = P2^0;
sbit LCD_DOUT = P1^0;
sbit LCD_DIN = P1^1;

//´®¿Ú
sbit M_RXD = P3^0;
sbit M_TXD = P3^1;

void IO_Init(void);

#endif