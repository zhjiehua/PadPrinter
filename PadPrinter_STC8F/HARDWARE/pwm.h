#ifndef __PWM_H__
#define __PWM_H__

#include "common.h"

#define T100NS (65536-FOSC/12/10000) //100ns
#define T1MS (65536-FOSC/12/1000)   //1ms timer calculation method in 12T mode
#define T5MS (65536-5*FOSC/12/1000) //5ms

//#define TIMEOUT T100NS
#define TIMEOUT T1MS  //��ʱ����װ��ֵ
//#define TIMEOUT T5MS

#define SPEEDTABLE_SIZE 20

typedef struct
{
    uint8_t targetIndex; //pwmĿ���ٶȵȼ�
	uint32_t targetCnt; //Ŀ��������

	//�Ӽ���ʹ��
	uint8_t timeCnt;
	uint8_t timeFlag;
	uint8_t curIndex;  //pwm��ǰ�ٶȵȼ�
	uint32_t accCnt; //�����������������ٹ������˶�������������Ҫ���ڼ������������(Ҳ�м��ٵ�)
	uint32_t decCnt; //���������������ڶ�����������ʼ����
	uint16_t speedTable[SPEEDTABLE_SIZE]; //�ٶȱ�
	//
	uint8_t cnt; //pwm������
	uint8_t status; //0��ֹͣ/����  1������  2������
}PWM_TypeDef;

extern PWM_TypeDef pwm;

void Pwm_Init(void);
void Pwm_CMD(uint8_t targetIndex, uint32_t cnt);
uint8_t Pwm_IsStop(void);
void Pwm_SpeedTableCreate(void);

#endif