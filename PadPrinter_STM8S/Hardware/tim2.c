/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��tim2.c
 * ����    ��TIM2��ʱ��PWM�������ú�����   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 *�޸�ʱ�� ��2012-11-12

  ���STM8������Ӳ������
    |-------------------------|
    |  ������ - PD4(TIM2_CH1) |
    |-------------------------|

*******************************************************************************/

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "tim2.h"
#include "option.h"
/* �Զ��������� */

/* �Զ���� */
#define HSE_CLK
//#define HSI_CLK
/* ȫ�ֱ������� */
static  u32 TimingDelay; /* ����ֵ */
#ifdef HSI_CLK
u16 Beep_Frequency[7] = {3816,3400,3029,2864,2550,2272,2023};	/* ����Ƶ�� */
#endif

#ifdef HSE_CLK
u16 Beep_Frequency[7] = {5724,5100,4543,2864,3825,3408,3034};	/* ����Ƶ�� */
#endif
/*******************************************************************************
 * ����: Tim2_Timer_Init
 * ����: TIM2��ʱ����ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: TIM2��ʱ����ʱʱ�����
         ʹ���ⲿ24MHz:
         BaseTime_Max=1/(24000000/32768)*(65535+1)=89.478s
         BaseTime_Min=1/24000000*(1+1)=0.083us
         ʹ���ڲ���16MHz
         BaseTime_Max=1/(16000000/32768)*(65535+1)=134.217s
         BaseTime_Min=1/16000000*(1+1)=0.125us
         ���ʹ���ⲿ24MHz���������#define HSE_CLK����#define HSI_CLKע������
         ���ʹ���ⲿ24MHz���������#define HSI_CLK����#define HSE_CLKע������
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
 * ����: TimingDelay_Decrement
 * ����: ��ʱ���������Լ�����
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void TimingDelay_Decrement(void)
{
	TimingDelay--;
}

/*******************************************************************************
 * ����: Delay_ms
 * ����: ��ʱ����
 * �β�: n10ms -> ��msΪ���������ʱ
 * ����: ��
 * ˵��: �� 
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
 * ����: TIM2_PWM_Init
 * ����: TIM2��ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void TIM2_PWM_Init(void)
{
     /*����ĳ�ʼ��ֻ��Ϊ�˻ָ�TIM2_CH1ͨ���Ĺ���
       ��ΪTIM2_CH1ͨ������ӳ�书����BEEP����PWM�ĳ�ʼ��
       ��û���κι�ϵ��*/     
     DefaultFunctionRemapping(AFR7_PD4_TIM2_CH1);

    /*
     * TIM2 Frequency = TIM2 counter clock/(ARR + 1) 
     * ��������TIM2�ļ���Ƶ��Ϊ 24M/8/(2999+1)=1K
     */
    TIM2_TimeBaseInit(TIM2_PRESCALER_8, 2999);	
    
    /* 
     * PWM1 Mode configuration: Channel1
     * TIM2 Channel1 duty cycle = [TIM2_CCR1/(TIM2_ARR + 1)] * 100 = 50%
     * TIM2 Channel2 duty cycle = [TIM2_CCR2/(TIM2_ARR + 1)] * 100 = 50%
     */ 
    
    /* ����ͨ��1 */
    TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 1500, TIM2_OCPOLARITY_HIGH);
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_Cmd(DISABLE);
}

/*******************************************************************************
 * ����: Set_TIM2_PWM_Frequency
 * ����: ����TIM2-PWMƵ��
 * �β�: TIM2_Period -> Ҫ���õ�Ƶ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Set_TIM2_PWM_Frequency(uint16_t TIM2_Period)
{
	/* ����PWMƵ�� */
	TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
	TIM2->ARRL = (uint8_t)(TIM2_Period);
}

/*******************************************************************************
 * ����: Set_TIM2_PWM1_DutyCycle
 * ����: ����TIM2-PWMͨ��1ռ�ձ�
 * �β�: TIM2_Pulse -> Ҫ���õ�ռ�ձ�
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Set_TIM2_PWM1_DutyCycle( uint16_t TIM2_Pulse)
{
    /* ����ͨ��1ռ�ձ� */
    TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
}

/*******************************************************************************
 * ����: Set_TIM2_PWM2_DutyCycle
 * ����: ����TIM2-PWMͨ��2ռ�ձ�
 * �β�: TIM2_Pulse -> Ҫ���õ�ռ�ձ�
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Set_TIM2_PWM2_DutyCycle( uint16_t TIM2_Pulse)
{
	/* ����ͨ��2ռ�ձ� */
    TIM2->CCR2H = (uint8_t)(TIM2_Pulse >> 8);
    TIM2->CCR2L = (uint8_t)(TIM2_Pulse);
}

/*******************************************************************************
 * ����: Test_PWM_Beep
 * ����: ����ͨ��1
 * �β�: ��
 * ����: ��
 * ˵��: �� 
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

/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ********************/