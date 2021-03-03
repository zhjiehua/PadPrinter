#ifndef __PWM_H__
#define __PWM_H__

#include "common.h"

#define T100NS (65536-FOSC/12/10000) //100ns
#define T1MS (65536-FOSC/12/1000)   //1ms timer calculation method in 12T mode
#define T5MS (65536-5*FOSC/12/1000) //5ms

//#define TIMEOUT T100NS
#define TIMEOUT T1MS  //定时器重装载值
//#define TIMEOUT T5MS

#define SPEEDTABLE_SIZE 20

typedef struct
{
    uint8_t targetIndex; //pwm目标速度等级
	uint32_t targetCnt; //目标脉冲数

	//加减速使用
	uint8_t timeCnt;
	uint8_t timeFlag;
	uint8_t curIndex;  //pwm当前速度等级
	uint32_t accCnt; //加速脉冲数，即加速过程用了多少脉冲数，主要用于计算减速脉冲数(也叫减速点)
	uint32_t decCnt; //减速脉冲数，即在多少脉冲数后开始减速
	uint16_t speedTable[SPEEDTABLE_SIZE]; //速度表
	//
	uint8_t cnt; //pwm脉冲数
	uint8_t status; //0：停止/匀速  1：加速  2：减速
}PWM_TypeDef;

extern PWM_TypeDef pwm;

void Pwm_Init(void);
void Pwm_CMD(uint8_t targetIndex, uint32_t cnt);
uint8_t Pwm_IsStop(void);
void Pwm_SpeedTableCreate(void);

#endif