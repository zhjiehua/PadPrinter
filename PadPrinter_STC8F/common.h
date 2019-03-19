#ifndef __COMMON_H__
#define __COMMON_H__

#include "stc8.h"
#include "intrins.h"
//#include "RTX51TNY.H"

typedef unsigned char uint8_t;
typedef          char int8_t;
typedef unsigned int uint16_t;
typedef          int int16_t;
typedef unsigned long uint32_t;
typedef          long int32_t;

typedef unsigned char uint8;
typedef          char int8;
typedef unsigned int uint16;
typedef          int int16;
typedef unsigned long uint32;
typedef          long int32;

//----------------------------------------------- 
/* define constants */
//#define FOSC 11059200L      //System frequency
#define FOSC 24000000L      //System frequency

sfr IP0			= 0xB8; //�ж����ȼ���λ PPCA  PLVD  PADC  PS   PT1  PX1  PT0  PX0   0000,0000
sfr WAKE_CLKO	= 0x8F; //PCAWAKEUP  RXD_PIN_IE  T1_PIN_IE  T0_PIN_IE  LVD_WAKE  _  T1CLKO  T0CLKO  0000,0000B
    //b7 - PCAWAKEUP : PCA �жϿɻ��� powerdown��
    //b6 - RXD_PIN_IE : �� P3.0(RXD) �½�����λ RI ʱ�ɻ��� powerdown(�������Ӧ�ж�)��
    //b5 - T1_PIN_IE : �� T1 ���½�����λ T1 �жϱ�־ʱ�ɻ��� powerdown(�������Ӧ�ж�)��
    //b4 - T0_PIN_IE : �� T0 ���½�����λ T0 �жϱ�־ʱ�ɻ��� powerdown(�������Ӧ�ж�)��
    //b3 - LVD_WAKE : �� CMPIN �ŵ͵�ƽ��λ LVD �жϱ�־ʱ�ɻ��� powerdown(�������Ӧ�ж�)��
    //b2 - 
    //b1 - T1CLKO : ���� T1CKO(P3.5) ����� T1 ������壬Fck1 = 1/2 T1 �����
    //b0 - T0CLKO : ���� T0CKO(P3.4) ����� T0 ������壬Fck0 = 1/2 T1 �����
sfr P4SW 		= 0xBB; //Port-4 switch	  -  LVD_P4.6 ALE_P4.5 NA_P4.4  -   -   -   -	  x000,xxxx

#endif