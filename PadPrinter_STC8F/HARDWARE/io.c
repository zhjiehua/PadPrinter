#include "io.h"
#include "uart.h"
#include "misc.h"
#include "timer.h"
#include "tm770x.h"
#include "stepmotor.h"
#include "eeprom.h"

#include "misc.h"
#include "management.h"

//IO��ʼ��
//�������Ҫ���ó�����ģʽ
void IO_Init(void)
{
#if !ARRAY_KEYBOARD

	//IO--------------------------------------------------------------------------------------
	P0M1 = 0x01;    //˫˫˫˫˫˫˫˫
	P0M0 = 0x00;
	P0 = 0xFF;    //-,-,-,-,MIN1,MIN3,MIN2,MIN6
	//----------------------
	P1M1 = 0x30;    //˫˫©©����˫˫
	P1M0 = 0x3C;    //
	P1 = 0xF3;    //XTAL2,XTAL1,IIC_SCL,IIC_SDA,MOUT2,MOUT1,LCD_DIN,LCD_DOUT
	//----------------------
	P2M1 = 0xE0;    //˫˫˫˫˫˫˫˫
	P2M0 = 0x00;
	P2 = 0xFF;    //MIN9,MIN7,MIN8,MIN4,MIN5,DIS_DIO,DIS_CLK,DIS_STB
	//----------------------
	P3M1 = 0x00;    //��������˫˫˫˫
	P3M0 = 0xF0;
	P3 = 0x0F;    //MOUT6,MOUT5,LEDM,MBEEP,INT_SW8,INT_PD,M-TXD,M-RXD
	//----------------------
	P4M1 = 0x00;    //˫˫˫˫˫˫˫˫˫
	P4M0 = 0x00;
	P4 = 0xFF;    //-,-,-,-,-,-,-,-
	
	P5M1 = 0x00;    //˫˫����˫˫˫˫˫
	P5M0 = 0x30;
	P5 = 0xCF;    //-,-,MOUT4,MOUT3,-,-,-,-

#else

	//IO--------------------------------------------------------------------------------------
	P0M1 = 0x01;    //˫˫˫˫˫˫˫˫
	P0M0 = 0x00;
	P0 = 0xFF;    //-,-,-,-,MIN1,MIN3,MIN2,MIN6
	//----------------------
	//P1M1 = 0x30;    //˫˫©©����˫˫
	//P1M0 = 0x3C;    //
    P1M1 = 0x33;
    P1M0 = 0xFC;
	P1 = 0xF3;    //K_COL2,K_COL4,IIC_SCL,IIC_SDA,MOUT2,MOUT1,K_ROW2,K_ROW1
	//----------------------
	P2M1 = 0xE0;    //˫˫˫˫˫˫˫˫
	P2M0 = 0x00;
	P2 = 0xFF;    //MIN9,MIN7,MIN8,MIN4,MIN5,DIS_DIO,DIS_CLK,DIS_STB
	//----------------------
	P3M1 = 0x00;    //����˫��˫˫˫˫
	//P3M0 = 0xD0;
    P3M0 = 0xF4;
	P3 = 0x0F;    //MOUT6,MOUT5,K_COL1,MBEEP,INT_SW8,K_COL3,M-TXD,M-RXD
	//----------------------
	P4M1 = 0x00;    //˫˫˫˫˫˫˫˫˫
	P4M0 = 0x00;
	P4 = 0xFF;    //-,-,-,-,-,-,-,-
	
	P5M1 = 0x00;    //˫˫����˫˫˫˫˫
	P5M0 = 0x30;
	P5 = 0xCF;    //-,-,MOUT4,MOUT3,-,-,-,-

#endif


	//SFR---------------------------------------------------------------------------
	WDT_CONTR = 0x00;         //WDTδ����
	PCON = 0x00; //bit7=0 ���ڲ����ʲ��ӱ�
	//SCON = 0x50;  MODE 1	�ɱ䲨����8λ���ݷ�ʽ
	//TMOD = 0x00; //T0,MODE0,16λ�Զ���װ,T1 MODE1,16λ�Զ���װ
	//AUXR = 0xD5; //T0/T1/T2����Ƶ,12T����ģʽ;T2Ϊ����1�Ĳ����ʷ�����

	//TCON = 0x00; //

    //-------------------------------------------------------------
	//��ʼ��IO
    //
    MOUT1 = 0;
    MOUT2 = 0;
    MOUT3 = 0;
    MOUT4 = 0;
    MOUT5 = 0;
    MOUT6 = 1;

//    //����ʱ�ӣ�ʹ���ⲿ����
//    P_SW2 = 0x80;   
//    XOSCCR |= 0xC0; //�����ⲿ����
//    while(XOSCCR&0x01 == 0x00); //�ⲿ�������
//    CKSEL = 0x01;                                //��ʱ��ʹ���ⲿ����
//    //CKSEL |= 0xA0;                               //��ʱ��32��Ƶ�����P5.4��
//    CLKDIV = 0x00;                             //SYSclk = MCLK/1
//    //CLKDIV = 0x0A;                             //SYSclk = MCLK/10   //������������
//    IRC24MCR = 0x00;                            //�ر��ڲ�24MHzʱ��
//    P_SW2 = 0x00;
}
