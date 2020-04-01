/*
	IN_NUM用于配置输入点的个数，输入点的顺序为0~IN_NUM（10进制）.
	对于PLC中的X10 X11等其实就是X8 X9

	OUT_NUM用于配置输出点的个数，输出点的顺序为0~OUT_NUM（10进制）.

	IO_Refresh()函数用于刷新所有IO的状态，包括所有输入输出。所以这个函数需要在while(1)中调用

	核心函数_GetX(uint8_t X, uint8_t Num)用于读取输入点，包括滤波功能。

	滤波时间是SHAKE_TIME*IO_FRE_DOUBLE个定时周期，其中SHAKE_TIME范围为0~31.
	现在的滤波时间是SHAKE_TIME*IO_FRE_DOUBLE个1ms
*/

#ifndef __SIMPLC_IO_H__
#define __SIMPLC_IO_H__

#include "common.h"
#include "io.h"

#define SHAKE_TIME 0//32个定时周期
#define IO_FRE_DOUBLE 0//消抖用的定时倍频

#define IN_NUM 9 //输入点数
#define OUT_NUM 6  //输出点数
#define RELAY_NUM 110 //中间继电器个数

//元件类型
#define X_COMPONET	0
#define Y_COMPONET	1
#define M_COMPONET	2

//电平类型
#define LEVEL			0
#define FALLING_EDGE	1
#define RISING_EDGE		2

//IO点定义管理
typedef struct
{
	//电平
	uint8_t *X_Level;
	uint8_t *Y_Level;
	uint8_t *M_Level;

	//下降沿
	uint8_t *X_FallingEdge;
	uint8_t *Y_FallingEdge;
	uint8_t *M_FallingEdge;

	//上升沿
	uint8_t *X_RisingEdge;
	uint8_t *Y_RisingEdge;
	uint8_t *M_RisingEdge;
}IO_Man_TypeDef;

//定义输入点
//#define X0  MIN1
//#define X1  MIN2
//#define X2  MIN3
//#define X3  MIN4
//#define X4  MIN9
//#define X5  MIN8
//#define X6  MIN7
//#define X7  MIN6
//#define X8  MIN5

#define X0  MIN1
#define X1  MIN2
#define X2  MIN3
#define X3  MIN4
#define X4  MIN5
#define X5  MIN6
#define X6  MIN7
#define X7  MIN8
#define X8  MIN9

//定义输出点
#define Y0(x)  MOUT1 = x
#define Y1(x)  MOUT2 = x
#define Y2(x)  MOUT3 = x
#define Y3(x)  MOUT4 = x
#define Y4(x)  MOUT5 = x
#define Y5(x)  MOUT6 = x

//#define Y0(x)  MOUT2 = x
//#define Y1(x)  MOUT1 = x
//#define Y2(x)  MOUT3 = x
//#define Y3(x)  MOUT4 = x

//M 定义中间继电器功能
#define M_KEY_FAST              0
#define M_KEY_SLOW              1
#define M_KEY_AUX               2
#define M_KEY_UPDOWN            3
#define M_KEY_FRONTBACK         4
#define M_KEY_STARTSTOP         5
#define M_KEY_FOOT              6
#define M_KEY_CLEAR             7
#define M_KEY_SW8               8

#define M_ONCE                  9
#define M_BEEPER                10

#define M_POWERON               11
#define M_PROGRAM_SEL           12
#define M_PROGRAM_SELF          13
#define M_PROGRAM_SELF_CHANGE   14

#define M_PROGRAM_HEAD_CHECK    15
#define M_PROGRAM_PLATFORM_CHECK    16

#define M_PROGRAM_READ          18
#define M_SEG_FLASH             19
#define M_SEG_FLASHFLAG         20
#define M_PROGRAM_READY         21

#define M_MODE_AUTO             22
#define M_MODE_MANUAL           23
#define M_MODE_PAUSE            24
#define M_CHANGEMODE_PAUSE      25

#define M_PROGRAM_PERIOD        26
#define M_PROGRAM_STEP          27
#define M_PROGRAM_FOOT          28
#define M_PROGRAM_AUX           29
#define M_MODE_RESTORE          30

#define M_FLAG_MANUAL           31
#define M_AUTO_FLAG_OIL         32
#define M_AUTO_FLAG_AUX         33

#define M_ADJ_DELAY             34

#define M_PRINTDOWN             35
#define M_PRINTUP               36
#define M_SHIFT                 37
#define M_SHIFT_RETURN          38
#define M_SHIFT_DIRECT          39
#define M_SHIFT_POS             40
#define M_SHIFT_TIME            41

#define M_ACTION_DELAY          42
#define M_RETURN                43
#define M_RETURN_POS            44
#define M_RETURN_FILTER         45
#define M_PUSH                  46
#define M_PUSH_RETURN           47

#define M_MAN_UPDOWN            48
#define M_MAN_FRONTBACK         49
#define M_MAN_FRONTBACK_STOP    50
#define M_MAN_AUX               51
#define M_MAN_AUX_RETURN        52

#define M_AUTO_MAN_UPDOWN       53
#define M_AUTO_MAN_FRONTBACK    54
#define M_MAN_UPDOWN_FB         55
#define M_MAN_FRONTBACK_AUTO    56

#define M_INTSET                57

#define M_KEY_FOOT_PRESSED      63
#define M_KEY_FOOT_AFTERFILTER  64

#define M_HOME_RETURN           65
#define M_HOME_CHECK            66
#define M_HOME_FINISH           67

#define M_RESTORE_RETURN        68
#define M_RESTORE_HOME          69
#define M_RESTORE_UP            70


#define M_OUTPUT_FLASH          71
#define M_OUTPUT_FLASH_FLAG     72
#define M_MACHINE_CHECK         73
#define M_MACHINE_AUX_FAULT     74

#define M_CAN_CHANGE_DELAY      75
#define M_SAVE_DELAY            76

#define M_AUTO_DELAYADJUST      77 //自动状态下调节延时参数，自动程序需要暂停一下，调节完成后，自动程序需要重新启动

#define M_MAN_FRONTBACK_UD      78


#define M_ABSORBDOWN_FINISH     80
#define M_ABSORBUP_FINISH       81
#define M_ABSORB_FINISH         82
#define M_PRINTDOWN_FINISH      83
#define M_PRINTUP_FINISH        84
#define M_PRINT_FINISH          85
#define M_FRONT_FINISH          86
#define M_BACK_FINISH           87

#define M_SHIFT_FINISH          88
#define M_RETURN_FINISH         89

#define M_SHIFTRETURN_FINISH    78  //
#define M_SHIFTRETURN2_FINISH   79

#define M_KEY_UPDOWN_PRESS      90
#define M_KEY_FRONTBACK_PRESS   91
#define M_KEY_AUX_PRESS         92

#define M_ACTIONHEAD_FINISH     93
#define M_ACTIONPLATFORM_FINISH 94
#define M_ACTION_FINISH         95

#define M_MODE_RESTORE_QUIT     96

#define M_CHANGEDELAY           97

#define M_CANNOTSTOP            98

#define M_RESTORE_DELAY         99

#define M_WAITMSIGNAL_NONE      100

//X
//#define X_ABSORB_O        0
//#define X_ABSORB_L        1
//#define X_PRINT_O         2
//#define X_PRINT_L         3
//#define X_SHIFT_O         4
//#define X_POS             5
//#define X_SHIFT_L1        6
//#define X_SHIFT_L2        7
//#define X_SCRAPER         8

#define X_ABSORB_O        0
#define X_ABSORB_L        1
#define X_PRINT_O         2
#define X_PRINT_L         3
#define X_SCRAPER         4
#define X_SHIFT_O         5
#define X_POS             6
#define X_SHIFT_L1        7
#define X_SHIFT_L2        8


//Y
#define Y_UPDOWN          0
#define Y_FRONTBACK       1
#define Y_SHIFT           2
#define Y_POS             3
#define Y_SCRAPER         4
#define Y_SHIFT2          5

extern uint8_t IN_State[IN_NUM];//输入点滤波用
/*
	bit8 记录输入电平
	bit7 开启延时的开关
	bit6 延时时间到标志
	bit5~0 延时定时周期数
*/

void SIMPLC_IO_Init(void);
void SIMPLC_IO_Refresh(void);

uint8_t SIMPLC_IO_Get(uint8_t Component, uint8_t Index, uint8_t Type);
void SIMPLC_IO_Set(uint8_t Component, uint8_t Index, uint8_t Level);

void SIMPLC_IO_SetRising(uint8_t Component, uint8_t Index, uint8_t Level);
#define SMR(x, y)   SIMPLC_IO_SetRising(M_COMPONET, x, y)

#define GML(x)   SIMPLC_IO_Get(M_COMPONET, x, LEVEL)
#define GMR(x)   SIMPLC_IO_Get(M_COMPONET, x, RISING_EDGE)
#define GMF(x)   SIMPLC_IO_Get(M_COMPONET, x, FALLING_EDGE)
#define SML(x, y)   SIMPLC_IO_Set(M_COMPONET, x, y)

#define GXL(x)   SIMPLC_IO_Get(X_COMPONET, x, LEVEL)
#define GXR(x)   SIMPLC_IO_Get(X_COMPONET, x, RISING_EDGE)
#define GXF(x)   SIMPLC_IO_Get(X_COMPONET, x, FALLING_EDGE)

#define SYL(x, y)   SIMPLC_IO_Set(Y_COMPONET, x, y)
#define GYL(x)   SIMPLC_IO_Get(Y_COMPONET, x, LEVEL)

#endif
