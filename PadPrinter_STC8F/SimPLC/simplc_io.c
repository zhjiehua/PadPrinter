#include "simplc_io.h"
#include "io.h"
#include "project.h"
#include "management.h"
#include "common.h"
#include "uart.h"

uint8_t IN_State[IN_NUM];//输入点滤波用
/*
	bit8 记录输入电平
	bit7 开启延时的开关
	bit6 延时时间到标志
	bit5~0 延时定时周期数
*/
static uint8_t IN_Last[(IN_NUM+7)/8];//输入点上一状态
static uint8_t IN[(IN_NUM+7)/8];//输入点
static uint8_t IN_Rising[(IN_NUM+7)/8];//输入点上升沿
static uint8_t IN_Falling[(IN_NUM+7)/8];//输入点下降沿

static uint8_t OUT_Last[(OUT_NUM+7)/8];//输出点上一状态
static uint8_t OUT[(OUT_NUM+7)/8];//输出点
static uint8_t OUT_Rising[(OUT_NUM+7)/8];//输出点上升沿
static uint8_t OUT_Falling[(OUT_NUM+7)/8];//输出点下降沿

static uint8_t RELAY_Last[(RELAY_NUM+7)/8];//中间继电器上一状态
static uint8_t RELAY[(RELAY_NUM+7)/8];//中间继电器
static uint8_t RELAY_Rising[(RELAY_NUM+7)/8];//中间继电器上升沿
static uint8_t RELAY_Falling[(RELAY_NUM+7)/8];//中间继电器下降沿

//IO管理结构体
IO_Man_TypeDef IO_Man;

//IO初始化
void SIMPLC_IO_Init(void)
{
    uint8_t i;

    IO_Init();
    
    for(i=0;i<(IN_NUM+7)/8;i++)
    {
        IN_Last[i] = 0;
        IN[i] = 0;
        IN_Rising[i] = 0;
        IN_Falling[i] = 0;
    }
    for(i=0;i<(OUT_NUM+7)/8;i++)
    {
        OUT_Last[i] = 0;
        OUT[i] = 0;
        OUT_Rising[i] = 0;
        OUT_Falling[i] = 0;
    }
    for(i=0;i<(RELAY_NUM+7)/8;i++)
    {
        RELAY_Last[i] = 0;
        RELAY[i] = 0;
        RELAY_Rising[i] = 0;
        RELAY_Falling[i] = 0;
    }

	IO_Man.X_Level = IN;
	IO_Man.Y_Level = OUT;
	IO_Man.M_Level = RELAY;
	
	IO_Man.X_FallingEdge = IN_Falling;
	IO_Man.Y_FallingEdge = OUT_Falling;
	IO_Man.M_FallingEdge = RELAY_Falling;
	
	IO_Man.X_RisingEdge = IN_Rising;
	IO_Man.Y_RisingEdge = OUT_Rising;
	IO_Man.M_RisingEdge = RELAY_Rising;
}

//读取输入点，带消抖功能
//static void _GetX(bit X, uint8_t Num)
//{
//	uint8_t cIndex = Num / 8;
//	uint8_t rIndex = Num % 8;
//
//	if(!X != (IN_State[Num]>>7) & 0x01)
//	{
//		//记录输入电平并开启延时
//		if(!X)
//			IN_State[Num] = 0x80|0x40;
//		else
//			IN_State[Num] = 0x40;
//	}
//	if(IN_State[Num] & 0x20)//延时时间到
//	{
//		IN_State[Num] &= ~0x20;
//		if(!X == (IN_State[Num]>>7) & 0x01)//如果还是之前的电平，表示输入点有效
//		{
//			//IN_Last[cIndex] = IN[cIndex];//记录上一个状态，需要在扫描前更新
//			if(!X)
//				IN[cIndex] |= 0x01<<rIndex;
//			else
//				IN[cIndex] &= ~(0x01<<rIndex);
//		}
//	}
//}

#define _GetX(X, Num) do{\
	if(!X)\
		IN[Num/8] |= 0x01<<(Num%8);\
	else\
		IN[Num/8] &= ~(0x01<<(Num%8));\
}while(0)

//刷新IO点
void SIMPLC_IO_Refresh(void)
{
	uint8_t i;	
/*----------------------------输入点---------------------------------*/
	//记录输入点上一状态
	for(i=0;i<(IN_NUM+7)/8;i++)
		IN_Last[i] = IN[i];

	//读取输入点
	_GetX(X0, X_ABSORB_O);
	_GetX(X1, X_ABSORB_L);
	_GetX(X2, X_PRINT_O);
	_GetX(X3, X_PRINT_L);
    _GetX(X8, X_SCRAPER);
    if(man.sensorLevel)//金属感应开关
    {
        _GetX(!X4, X_SHIFT_O);
        _GetX(!X5, X_POS);
        _GetX(!X6, X_SHIFT_L1);
        _GetX(!X7, X_SHIFT_L2);
    }
    else//光电开关
    {
        _GetX(X4, X_SHIFT_O);
        _GetX(X5, X_POS);
        _GetX(X6, X_SHIFT_L1);
        _GetX(X7, X_SHIFT_L2);        
    }
	
	for(i=0;i<(IN_NUM+7)/8;i++)
	{
		IN_Rising[i] = (IN_Last[i]^IN[i]) & IN[i];
		IN_Falling[i] = (IN_Last[i]^IN[i]) & IN_Last[i];
	}
    
/*----------------------------输出点--------------------------------------*/
	//输出点刷新
    Y0(OUT[Y_UPDOWN/8]&(0x01<<Y_UPDOWN));
    Y1(OUT[Y_FRONTBACK/8]&(0x01<<Y_FRONTBACK));
    Y2(OUT[Y_SHIFT/8]&(0x01<<Y_SHIFT));
    Y3(OUT[Y_POS/8]&(0x01<<Y_POS));
    Y4(OUT[Y_SCRAPER/8]&(0x01<<Y_SCRAPER));
    Y5(OUT[Y_SHIFT2/8]&(0x01<<Y_SHIFT2));

	for(i=0;i<(OUT_NUM+7)/8;i++)
	{
		OUT_Rising[i] = (OUT_Last[i]^OUT[i]) & OUT[i];
		OUT_Falling[i] = (OUT_Last[i]^OUT[i]) & OUT_Last[i];
		OUT_Last[i] = OUT[i];
	}

/*----------------------------中间继电器----------------------------------*/
	for(i=0;i<(RELAY_NUM+7)/8;i++)
	{
		RELAY_Rising[i] = (RELAY_Last[i]^RELAY[i]) & RELAY[i];
		RELAY_Falling[i] = (RELAY_Last[i]^RELAY[i]) & RELAY_Last[i];
		RELAY_Last[i] = RELAY[i];
	}
}

//----------------------------IO_API-------------------------------
//读IO
//Componet 元件类型，可取X_COMPONET Y_COMPONET M_COMPONET
//Index 元件编号 
//Type 电平类型，可取LEVEL FALLING_EDGE RISING_EDGE
uint8_t SIMPLC_IO_Get(uint8_t Component, uint8_t Index, uint8_t Type)
{
	uint8_t **p = &IO_Man.X_Level;
	return (uint8_t)((*(p + Component + 3*Type))[Index/8]>>(Index%8) & 0x01);
}

//写IO
void SIMPLC_IO_Set(uint8_t Component, uint8_t Index, uint8_t Level)
{
	uint8_t **p = &IO_Man.X_Level;
	if(Level)
		(*(p + Component))[Index/8] |= (0x01<<(Index%8));
	else
		(*(p + Component))[Index/8] &= ~(0x01<<(Index%8));	
}