/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：tim2.c
 * 描述    ：TIM2定时、PWM功能设置函数库   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-11-12

  风驰STM8开发板硬件连接
    |-------------------------|
    |  蜂鸣器 - PD4(TIM2_CH1) |
    |-------------------------|

*******************************************************************************/

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "tim2.h"
#include "option.h"
/* 自定义新类型 */

/* 自定义宏 */
#define HSE_CLK
//#define HSI_CLK
/* 全局变量定义 */
static  u32 TimingDelay; /* 计数值 */
#ifdef HSI_CLK
u16 Beep_Frequency[7] = {3816,3400,3029,2864,2550,2272,2023};	/* 音符频率 */
#endif

#ifdef HSE_CLK
u16 Beep_Frequency[7] = {5724,5100,4543,2864,3825,3408,3034};	/* 音符频率 */
#endif
/*******************************************************************************
 * 名称: Tim2_Timer_Init
 * 功能: TIM2定时器初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: TIM2的时基定时时间计算
         使用外部24MHz:
         BaseTime_Max=1/(24000000/32768)*(65535+1)=89.478s
         BaseTime_Min=1/24000000*(1+1)=0.083us
         使用内部部16MHz
         BaseTime_Max=1/(16000000/32768)*(65535+1)=134.217s
         BaseTime_Min=1/16000000*(1+1)=0.125us
         如果使用外部24MHz晶振则添加#define HSE_CLK，把#define HSI_CLK注释起来
         如果使用外部24MHz晶振则添加#define HSI_CLK，把#define HSE_CLK注释起来
 ******************************************************************************/
void Tim2_Timer_Init(void)
{
    #ifdef HSI_CLK
    /*BaseTime=1/(16000000/16)*(999+1)=1ms*/
    TIM2_TimeBaseInit(TIM2_PRESCALER_16 ,999);
    TIM2_PrescalerConfig(TIM2_PRESCALER_16,TIM2_PSCRELOADMODE_IMMEDIATE);
    #endif

    #ifdef HSE_CLK
    /*BaseTime=1/(24000000/8)*(2999+1)=1ms*/
    TIM2_TimeBaseInit(TIM2_PRESCALER_8 ,2999);
    TIM2_PrescalerConfig(TIM2_PRESCALER_8,TIM2_PSCRELOADMODE_IMMEDIATE);
    #endif
    
    TIM2_ARRPreloadConfig(ENABLE);
    TIM2_ITConfig(TIM2_IT_UPDATE , ENABLE);
    TIM2_Cmd(ENABLE);
}

/*******************************************************************************
 * 名称: TimingDelay_Decrement
 * 功能: 延时计数变量自减操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void TimingDelay_Decrement(void)
{
	TimingDelay--;
}

/*******************************************************************************
 * 名称: Delay_ms
 * 功能: 延时函数
 * 形参: n10ms -> 以ms为间隔进行延时
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
//void Delay_ms(u16 nms)
//{
//  	
//	TimingDelay = nms;
//	
//	while(TimingDelay != 0)
//	  	;
//}

/*******************************************************************************
 * 名称: TIM2_PWM_Init
 * 功能: TIM2初始化函数
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void TIM2_PWM_Init(void)
{
     /*这里的初始化只是为了恢复TIM2_CH1通道的功能
       因为TIM2_CH1通道的重映射功能是BEEP，和PWM的初始化
       是没有任何关系的*/     
     DefaultFunctionRemapping(AFR7_PD4_TIM2_CH1);

    /*
     * TIM2 Frequency = TIM2 counter clock/(ARR + 1) 
     * 这里设置TIM2的计数频率为 24M/8/(2999+1)=1K
     */
    TIM2_TimeBaseInit(TIM2_PRESCALER_8, 2999);	
    
    /* 
     * PWM1 Mode configuration: Channel1
     * TIM2 Channel1 duty cycle = [TIM2_CCR1/(TIM2_ARR + 1)] * 100 = 50%
     * TIM2 Channel2 duty cycle = [TIM2_CCR2/(TIM2_ARR + 1)] * 100 = 50%
     */ 
    
    /* 测试通道1 */
    TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 1500, TIM2_OCPOLARITY_HIGH);
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_Cmd(DISABLE);
}

/*******************************************************************************
 * 名称: Set_TIM2_PWM_Frequency
 * 功能: 设置TIM2-PWM频率
 * 形参: TIM2_Period -> 要设置的频率
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Set_TIM2_PWM_Frequency(uint16_t TIM2_Period)
{
	/* 设置PWM频率 */
	TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
	TIM2->ARRL = (uint8_t)(TIM2_Period);
}

/*******************************************************************************
 * 名称: Set_TIM2_PWM1_DutyCycle
 * 功能: 设置TIM2-PWM通道1占空比
 * 形参: TIM2_Pulse -> 要设置的占空比
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Set_TIM2_PWM1_DutyCycle( uint16_t TIM2_Pulse)
{
    /* 设置通道1占空比 */
    TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
}

/*******************************************************************************
 * 名称: Set_TIM2_PWM2_DutyCycle
 * 功能: 设置TIM2-PWM通道2占空比
 * 形参: TIM2_Pulse -> 要设置的占空比
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Set_TIM2_PWM2_DutyCycle( uint16_t TIM2_Pulse)
{
	/* 设置通道2占空比 */
    TIM2->CCR2H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR2L = (uint8_t)(TIM2_Pulse);
}

/*******************************************************************************
 * 名称: Test_PWM_Beep
 * 功能: 测试通道1
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Test_PWM_Beep(void)
{
	u8 i;
	for(i=0;i<7;i++)
	{
		Set_TIM2_PWM_Frequency(Beep_Frequency[i]);
		Delay(0xffff);
		Delay(0xffff);
		Delay(0xffff);
		Delay(0xffff);
	}
}

static void Delay(u32 nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}

/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ********************/