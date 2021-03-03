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

//pwm初始化
void Pwm_Init(void)
{
	pwm.status = 0;//停止
	pwm.timeCnt = 0;
	pwm.timeFlag = 0;
	pwm.curIndex = 0;
	pwm.targetIndex = 0;
	pwm.cnt = 0;

	/*******************步进电机-定时器0*********************************************/
	TMOD &= ~0x0F;
	TMOD |= 0x01;                                //模式1，不自动装载

	TL0 = 0x66;                                 //65536-11.0592M/12/1000
    TH0 = 0xfc;

	//TR0 = 1;                                    //启动定时器
    ET0 = 1;                                    //使能定时器中断

	//中断优先级，最高级（11）
	PT0 = 1;
	IPH |= PT0H;

	//创建速度表
	Pwm_SpeedTableCreate();
}

void Pwm_CMD(uint8_t targetIndex, uint32_t cnt)
{
    if(cnt)
	{
		if(!TR0)//停止状态才能启动
		{
			pwm.timeCnt = 0;
			pwm.timeFlag = 0;
			pwm.curIndex = 0;
			pwm.targetIndex = targetIndex;
			pwm.cnt = cnt;
		    pwm.status = 1;//加速
			pwm.accCnt = 0;
			pwm.decCnt = 0;
	

			TL0 = pwm.speedTable[pwm.curIndex];
			TH0 = pwm.speedTable[pwm.curIndex] >> 8;
	        TR0 = 1;//启动定时器
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
	if(pwm.status && pwm.timeFlag)//在加减速阶段才需要调节速度
	{
		pwm.timeFlag = 0;

		if(pwm.status == 2)//减速阶段
		{
			if(pwm.curIndex > pwm.targetIndex)
				pwm.curIndex--;
			else
				pwm.status = 0;//恒速
		}
		else if(pwm.status == 1)//加速阶段
		{
			if(pwm.curIndex < pwm.targetIndex)
				pwm.curIndex++;
			else
				pwm.status = 0;//恒速
		}

		if(pwm.status == 0)//恒速，计算减速点（注意，这里假设了目标脉冲数大于加减速的脉冲数）
		{
			pwm.accCnt = pwm.cnt;
			pwm.decCnt = pwm.targetCnt - pwm.accCnt;
		}
	}

	//修改定时器重装载值
	TL0 = pwm.speedTable[pwm.curIndex];
	TH0 = pwm.speedTable[pwm.curIndex] >> 8;

	pwm.cnt++;
	if(pwm.cnt >= pwm.targetCnt)
	{
		//ET0 = 0;
		TR0 = 0;//停止定时器

		pwm.status = 0;
	}
	else if(pwm.status == 0 && pwm.cnt >= pwm.decCnt)//到达减速点开始减速
	{
		pwm.status = 2;//开始减速
		pwm.targetIndex = 0;
	}

	//Y_POS
	MOUT4 = !MOUT4;

	//MBEEP = !MBEEP;                                  //测试端口
}
