/*
	�ö�ʱ����Դ����TIMER_NUM����ÿ������1ms������ʱ���ж�ʱ�䣩Ϊ��С��λ��

	��ʱ����ID��0~TIMER_NUM-1��ע��ID��Ҫ���������Χ��

	ʹ�ö�ʱ��ʱ����Ҫ������ʱ��������Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode)��Modeģʽ����δʵ�֡�
	�����ɹ��򷵻�0������ֵ��ʾ���ɹ��������궨ʱ��Ҫ����Timer_Start(uint8_t ID)������ʱ����	
	
	ע������Timer_Delete(uint8_t ID)����Ϊɾ����ʱ����ʱ������ζ�ʱ���ж�Ƭ��ʱ�䣬���ܻᵼ����ʧһ���жϡ�
	ɾ����ʱ���Ĺ�����Ҫ�ǴӶ�ʱ���������Ƴ����������Լ��ٶ�ʱ�������ɨ��ʱ�䡣
	����ö�ʱ�����������ã���ò�Ҫʹ��ɾ���������������Timer_ResetTime(uint8_t ID, uint16_t Time)�޸Ķ�ʱʱ�������á�

	ʹ��Timer_IsTimeOut(uint8_t ID)���Լ�鶨ʱʱ���Ƿ񵽴�����һ���ԵĶ�ʱ����Ҫ�ֶ����ʱ�������־λ��
	������Timer_ResetTimeOutFlag(uint8_t ID)����Ȼʹ��Timer_Restart(uint8_t ID)Ҳ���ԣ������������������������ʱ����
	��������ʺ������Զ�ʱ�������

*/

#ifndef __SIMPLC_TIMER_H__
#define __SIMPLC_TIMER_H__

#include "common.h"

#define TIMER_NUM 15	//��ʱ��������

//#define T1MS (999)   //1ms timer calculation method in 12T mode
//#define T5MS (4999) //5ms
//
//#define TIMEOUT T1MS  //��ʱ����װ��ֵ

//��ʱ������ṹ��
typedef struct _Timer_TypeDef
{
	uint8_t Flag;//��־λ
	/*
	bit7	��ʱ��ʱ�䵽	
	bit6	��ʱ������/ֹͣ

	bit1	��ʱ����ʹ�ã�1����ʹ��	0��δʹ��
	bit0	��ʱ������ģʽ��1���Զ���װ��	0���������У�δʵ�֣�
	*/
	uint16_t Count;//��ʱ��������
	uint16_t Time;//��ʱʱ�䣨1ms��λ��

	struct _Timer_TypeDef *Next;//ָ����һ����ʱ��
}Timer_TypeDef;

void SIMPLC_Timer_Init(void);
uint8_t SIMPLC_Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode);
uint8_t SIMPLC_Timer_Delete(uint8_t ID);
void SIMPLC_Timer_Start(uint8_t ID);
void SIMPLC_Timer_Restart(uint8_t ID);
void SIMPLC_Timer_Stop(uint8_t ID);
void SIMPLC_Timer_Reset(uint8_t ID);
void SIMPLC_Timer_ResetTime(uint8_t ID, uint16_t Time);
uint16_t SIMPLC_Timer_GetTime(uint8_t ID);
uint16_t SIMPLC_Timer_GetCount(uint8_t ID);
uint8_t SIMPLC_Timer_IsTimeOut(uint8_t ID);
void SIMPLC_Timer_ResetTimeOutFlag(uint8_t ID);

#define TS(x, y)    do{SIMPLC_Timer_ResetTime(x, y);SIMPLC_Timer_Restart(x);}while(0)
#define TG(x)       SIMPLC_Timer_IsTimeOut(x)

#endif