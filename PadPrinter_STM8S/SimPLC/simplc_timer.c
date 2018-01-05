#include "simplc_timer.h"
#include "simplc_io.h"
#include "stddef.h"
#include "tim1.h"
#include "uart1.h"
#include "tm1638.h"

Timer_TypeDef Timer[TIMER_NUM];//定时器资源
Timer_TypeDef *hTimer = NULL;//定时器链表头指针

//定时器0初始化
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
    //检查输入点是否需要延时
	IO_FreDouble++;
	if(IO_FreDouble >= IO_FRE_DOUBLE)
	{
		IO_FreDouble = 0;

		for(i=0;i<IN_NUM;i++)
		{
			if(IN_State[i] & 0x40)
			{
				IN_State[i]++;
				if(IN_State[i] & 0x1F >= SHAKE_TIME)//延时周期到
				{
					IN_State[i] &= ~0x40;
					IN_State[i] |= 0x20;
					//IN_State[i] &= ~0x1F;
				}
			}
		}
	}

	//遍历定时器链表，更新定时器数据
	pTimer = hTimer;
	while(pTimer != NULL)
	{
		if(pTimer->Flag & 0x40)//定时器启动
		{
			pTimer->Count++;
			if(pTimer->Count == pTimer->Time)
			{
				pTimer->Flag |= 0x80;//表示定时时间到
				pTimer->Count = 0;

//				if(pTimer->Flag & 0x01)//自动重装载模式
//					pTimer->Count = 0;
//				else
					pTimer->Flag &= ~0x40;//停止定时器	
			}
		}

		pTimer = pTimer->Next;
	}
#endif    
	TIM1_ClearITPendingBit(TIM1_IT_UPDATE);  
}
#endif

//创建定时器
//ID：定时器编号，具有唯一性，所有定时器都可以根据自己的ID找到
//Time：定时时间
//Mode：定时器模式，1：自动重装载	0：单次运行（未实现）
uint8_t SIMPLC_Timer_Create(uint8_t ID, uint16_t Time, uint8_t Mode)
{
	Timer_TypeDef *pTimer = NULL;

	if(Timer[ID].Flag & 0x02)//定时器已使用
		return 1;
	
	Timer[ID].Time = Time; //记录定时时间
	Timer[ID].Count = 0;//从0开始计数
	Timer[ID].Next = 0;//标志链表尾
	Timer[ID].Flag = 0x02;//标志改定时器已使用

	if(Mode)
		Timer[ID].Flag |= 0x01;//自动重装载模式

	//如果当前没有用到定时器
	if(hTimer == NULL)
		hTimer =  &Timer[ID];
	else
	{ 	
		//遍历链表，找到链表尾
		pTimer = hTimer;
		while(pTimer->Next != NULL)
			pTimer = pTimer->Next;

		pTimer->Next = &Timer[ID];//加入到链表尾	
	}

	return 0;
}

//定时器删除
uint8_t SIMPLC_Timer_Delete(uint8_t ID)
{
	Timer_TypeDef *pTimer = hTimer;
	Timer_TypeDef *pTimer_Last = hTimer;

	//遍历链表，找到链表尾
	while(pTimer != NULL)
	{
		if(pTimer == &Timer[ID])
			break;

		pTimer_Last = pTimer;
		pTimer = pTimer->Next;
	}

	if(pTimer == NULL)
		return 1;
	
	if(pTimer == hTimer)//如果删除的是头结点
	{
		//ET0 = 0;//不能进入定时中断
        TIM1_ITConfig(TIM1_IT_UPDATE , DISABLE);
		hTimer = pTimer->Next;
		//ET0 = 1;
        TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);
	}
	else
	{
	 	//ET0 = 0;//不能进入定时中断
        TIM1_ITConfig(TIM1_IT_UPDATE , DISABLE);
		pTimer_Last->Next = pTimer->Next;
		//ET0 = 1;
        TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);
	}

	pTimer->Flag = 0x00;//置定时器未使用标志

	return 0;

}

//定时器启动
void SIMPLC_Timer_Start(uint8_t ID)
{
	Timer[ID].Flag |= 0x40;	
}

//定时器重新启动，先复位定时器时间到标志，再启动定时器
void SIMPLC_Timer_Restart(uint8_t ID)
{	
	Timer[ID].Flag &= ~0x80;
	Timer[ID].Flag |= 0x40;	
}

//定时器停止
void SIMPLC_Timer_Stop(uint8_t ID)
{
	Timer[ID].Flag &= ~0x40;	
}

//定时器复位
void SIMPLC_Timer_Reset(uint8_t ID)
{
	Timer[ID].Count = 0;	
}

//定时器重新设置时间
void SIMPLC_Timer_ResetTime(uint8_t ID, uint16_t Time)
{
	Timer[ID].Time = Time;	
}

//得到定时器定时时间
uint16_t SIMPLC_Timer_GetTime(uint8_t ID)
{
	return Timer[ID].Time;	
}

//得到定时器计数时间
uint16_t SIMPLC_Timer_GetCount(uint8_t ID)
{
	return Timer[ID].Count;	
}

//检查是否定时时间到
uint8_t SIMPLC_Timer_IsTimeOut(uint8_t ID)
{
	return (Timer[ID].Flag & 0x80);
}

//复位定时时间到标志
void SIMPLC_Timer_ResetTimeOutFlag(uint8_t ID)
{
	Timer[ID].Flag &= ~0x80;
}
