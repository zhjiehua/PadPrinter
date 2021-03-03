#include "pwm.h"
#include "io.h"
#include "stddef.h"
#include "management.h"

//const uint8_t timer_TH_TL[SPEEDTABLE_SIZE][2] = 
//{
//    {0x00, 0x00},
//    {0x00, 0x80},
//
//	{0x44, 0x80}, //250HZ
//	{0xA2, 0x40}, //500HZ
//	{0xD1, 0x20}, //1000HZ
//	{0xED, 0x40}, //2500HZ
//	{0xF6, 0xA0}, //5000HZ
//	//{0xFB, 0x50}, //10000HZ
//};

PWM_TypeDef pwm;

//pwm��ʼ��
void Pwm_Init(void)
{
	pwm.status = 0;//ֹͣ
	pwm.timeCnt = 0;
	pwm.timeFlag = 0;
	pwm.curIndex = 0;
	pwm.targetIndex = 0;
	pwm.cnt = 0;

	/*******************�������-��ʱ��0*********************************************/
	TMOD &= ~0x0F;
	TMOD |= 0x01;                                //ģʽ1�����Զ�װ��

	TL0 = 0x66;                                 //65536-11.0592M/12/1000
    TH0 = 0xfc;

	//TR0 = 1;                                    //������ʱ��
    ET0 = 1;                                    //ʹ�ܶ�ʱ���ж�

	//�ж����ȼ�����߼���11��
	PT0 = 1;
	IPH |= PT0H;

	//�����ٶȱ�
	Pwm_SpeedTableCreate();
}

void Pwm_CMD(uint8_t targetIndex, uint32_t cnt)
{
    if(cnt)
	{
		if(!TR0)//ֹͣ״̬��������
		{
			pwm.timeCnt = 0;
			pwm.timeFlag = 0;
			pwm.curIndex = 0;
			pwm.targetIndex = targetIndex;
			pwm.cnt = cnt;
		    pwm.status = 1;//����
			pwm.accCnt = 0;
			pwm.decCnt = 0;
	

			TL0 = pwm.speedTable[pwm.curIndex];
			TH0 = pwm.speedTable[pwm.curIndex] >> 8;
	        TR0 = 1;//������ʱ��
		}
	}
    else
        TR0 = 0; 
}

uint8_t Pwm_IsStop(void)
{
	return TR0 ? 0 : 1;
}

void Pwm_SpeedTableCreate(void)
{
	uint8_t i;
	for(i=0;i<SPEEDTABLE_SIZE;i++)
	{
		pwm.speedTable[i] = i * 2500;
	}
}

//-----------------------------------------------
/* Timer0 interrupt routine */
void Timer0_Isr() interrupt 1 using 3
{
	if(pwm.status && pwm.timeFlag)//�ڼӼ��ٽ׶β���Ҫ�����ٶ�
	{
		pwm.timeFlag = 0;

		if(pwm.status == 2)//���ٽ׶�
		{
			if(pwm.curIndex > pwm.targetIndex)
				pwm.curIndex--;
			else
				pwm.status = 0;//����
		}
		else if(pwm.status == 1)//���ٽ׶�
		{
			if(pwm.curIndex < pwm.targetIndex)
				pwm.curIndex++;
			else
				pwm.status = 0;//����
		}

		if(pwm.status == 0)//���٣�������ٵ㣨ע�⣬���������Ŀ�����������ڼӼ��ٵ���������
		{
			pwm.accCnt = pwm.cnt;
			pwm.decCnt = pwm.targetCnt - pwm.accCnt;
		}
	}

	//�޸Ķ�ʱ����װ��ֵ
	TL0 = pwm.speedTable[pwm.curIndex];
	TH0 = pwm.speedTable[pwm.curIndex] >> 8;

	pwm.cnt++;
	if(pwm.cnt >= pwm.targetCnt)
	{
		//ET0 = 0;
		TR0 = 0;//ֹͣ��ʱ��

		pwm.status = 0;
	}
	else if(pwm.status == 0 && pwm.cnt >= pwm.decCnt)//������ٵ㿪ʼ����
	{
		pwm.status = 2;//��ʼ����
		pwm.targetIndex = 0;
	}

	//Y_POS
	MOUT4 = !MOUT4;

	//MBEEP = !MBEEP;                                  //���Զ˿�
}
