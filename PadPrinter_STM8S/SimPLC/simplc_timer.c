#include "simplc_timer.h"
#include "simplc_io.h"
#include "stddef.h"
#include "tim1.h"
#include "uart1.h"
#include "tm1638.h"

Timer_TypeDef Timer[TIMER_NUM];//��ʱ����Դ
Timer_TypeDef *hTimer = NULL;//��ʱ������ͷָ��

//��ʱ��0��ʼ��
void SIMPLC_Timer_Init(void)
{
    Tim1_Init();
}

#if 1
//-----------------------------------------------
/**
  * @brief Timer1 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	static uint8_t IO_FreDouble = 0;
	uint8_t i;
	Timer_TypeDef *pTimer = NULL;
	
    //TIM1_SetCounter(TIMEOUT);
    //UART1_printf("TIM1_UPD_OVF_TRG_BRK_IRQHandler\r\n");
    //TM1638_SendData(0, "P111");
    if(TimingDelay > 0)
        TimingDelay--;
#if 1    
    //���������Ƿ���Ҫ��ʱ
	IO_FreDouble++;
	if(IO_FreDouble >= IO_FRE_DOUBLE)
	{
		IO_FreDouble = 0;

		for(i=0;i<IN_NUM;i++)
		{
			if(IN_State[i] & 0x40)
			{
				IN_State[i]++;
				if(IN_State[i] & 0x1F >= SHAKE_TIME)//��ʱ���ڵ�
				{
					IN_State[i] &= ~0x40;
					IN_State[i] |= 0x20;
					//IN_State[i] &= ~0x1F;
				}
			}
		}
	}

	//������ʱ���������¶�ʱ������
	pTimer = hTimer;
	while(pTimer != NULL)
	{
		if(pTimer->Flag & 0x40)//��ʱ������
		{
			pTimer->Count++;
			if(pTimer->Count == pTimer->Time)
			{
				pTimer->Flag |= 0x80;//��ʾ��ʱʱ�䵽
				pTimer->Count = 0;

//				if(pTimer->Flag & 0x01)//�Զ���װ��ģʽ
//					pTimer->Count = 0;
//				else
					pTimer->Flag &= ~0x40;//ֹͣ��ʱ��	
			}
		}

		pTimer = pTimer->Next;
	}
#endif    
	TIM1_ClearITPendingBit(TIM1_IT_UPDATE);  
}
#endif

//������ʱ��
//ID����ʱ����ţ�����Ψһ�ԣ����ж�ʱ�������Ը����Լ���ID�ҵ�
//Time����ʱʱ��
//Mode����ʱ��ģʽ��1���Զ���װ��	0���������У�δʵ�֣�
uint8_t SIMPLC_Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode)
{
	Timer_TypeDef *pTimer = NULL;

	if(Timer[ID].Flag & 0x02)//��ʱ����ʹ��
		return 1;
	
	Timer[ID].Time = Time; //��¼��ʱʱ��
	Timer[ID].Count = 0;//��0��ʼ����
	Timer[ID].Next = 0;//��־����β
	Timer[ID].Flag = 0x02;//��־�Ķ�ʱ����ʹ��

	if(Mode)
		Timer[ID].Flag |= 0x01;//�Զ���װ��ģʽ

	//�����ǰû���õ���ʱ��
	if(hTimer == NULL)
		hTimer =  &Timer[ID];
	else
	{ 	
		//���������ҵ�����β
		pTimer = hTimer;
		while(pTimer->Next != NULL)
			pTimer = pTimer->Next;

		pTimer->Next = &Timer[ID];//���뵽����β	
	}

	return 0;
}

//��ʱ��ɾ��
uint8_t SIMPLC_Timer_Delete(uint8_t ID)
{
	Timer_TypeDef *pTimer = hTimer;
	Timer_TypeDef *pTimer_Last = hTimer;

	//���������ҵ�����β
	while(pTimer != NULL)
	{
		if(pTimer == &Timer[ID])
			break;

		pTimer_Last = pTimer;
		pTimer = pTimer->Next;
	}

	if(pTimer == NULL)
		return 1;
	
	if(pTimer == hTimer)//���ɾ������ͷ���
	{
		//ET0 = 0;//���ܽ��붨ʱ�ж�
        TIM1_ITConfig(TIM1_IT_UPDATE , DISABLE);
		hTimer = pTimer->Next;
		//ET0 = 1;
        TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);
	}
	else
	{
	 	//ET0 = 0;//���ܽ��붨ʱ�ж�
        TIM1_ITConfig(TIM1_IT_UPDATE , DISABLE);
		pTimer_Last->Next = pTimer->Next;
		//ET0 = 1;
        TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);
	}

	pTimer->Flag = 0x00;//�ö�ʱ��δʹ�ñ�־

	return 0;

}

//��ʱ������
void SIMPLC_Timer_Start(uint8_t ID)
{
	Timer[ID].Flag |= 0x40;	
}

//��ʱ�������������ȸ�λ��ʱ��ʱ�䵽��־����������ʱ��
void SIMPLC_Timer_Restart(uint8_t ID)
{	
	Timer[ID].Flag &= ~0x80;
	Timer[ID].Flag |= 0x40;	
}

//��ʱ��ֹͣ
void SIMPLC_Timer_Stop(uint8_t ID)
{
	Timer[ID].Flag &= ~0x40;	
}

//��ʱ����λ
void SIMPLC_Timer_Reset(uint8_t ID)
{
	Timer[ID].Count = 0;	
}

//��ʱ����������ʱ��
void SIMPLC_Timer_ResetTime(uint8_t ID, uint16_t Time)
{
	Timer[ID].Time = Time;	
}

//�õ���ʱ����ʱʱ��
uint16_t SIMPLC_Timer_GetTime(uint8_t ID)
{
	return Timer[ID].Time;	
}

//�õ���ʱ������ʱ��
uint16_t SIMPLC_Timer_GetCount(uint8_t ID)
{
	return Timer[ID].Count;	
}

//����Ƿ�ʱʱ�䵽
uint8_t SIMPLC_Timer_IsTimeOut(uint8_t ID)
{
	return (Timer[ID].Flag & 0x80);
}

//��λ��ʱʱ�䵽��־
void SIMPLC_Timer_ResetTimeOutFlag(uint8_t ID)
{
	Timer[ID].Flag &= ~0x80;
}
