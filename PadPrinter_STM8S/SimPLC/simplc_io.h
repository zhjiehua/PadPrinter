/*
	IN_NUM�������������ĸ�����������˳��Ϊ0~IN_NUM��10���ƣ�.
	����PLC�е�X10 X11����ʵ����X8 X9

	OUT_NUM�������������ĸ�����������˳��Ϊ0~OUT_NUM��10���ƣ�.

	IO_Refresh()��������ˢ������IO��״̬��������������������������������Ҫ��while(1)�е���

	���ĺ���_GetX(uint8_t X, uint8_t Num)���ڶ�ȡ����㣬�����˲����ܡ�

	�˲�ʱ����SHAKE_TIME*IO_FRE_DOUBLE����ʱ���ڣ�����SHAKE_TIME��ΧΪ0~31.
	���ڵ��˲�ʱ����SHAKE_TIME*IO_FRE_DOUBLE��1ms
*/

#ifndef __SIMPLC_IO_H__
#define __SIMPLC_IO_H__

#include "stm8s.h"
#include "stm8s_gpio.h"
#include "io.h"

#define SHAKE_TIME 0//32����ʱ����
#define IO_FRE_DOUBLE 0//�����õĶ�ʱ��Ƶ

#define IN_NUM 8 //�������
#define OUT_NUM 4  //�������
#define RELAY_NUM 80 //�м�̵�������

//Ԫ������
#define X_COMPONET	0
#define Y_COMPONET	1
#define M_COMPONET	2

//��ƽ����
#define LEVEL			0
#define FALLING_EDGE	1
#define RISING_EDGE		2

//IO�㶨�����
typedef struct
{
	//��ƽ
	uint8_t *X_Level;
	uint8_t *Y_Level;
	uint8_t *M_Level;

	//�½���
	uint8_t *X_FallingEdge;
	uint8_t *Y_FallingEdge;
	uint8_t *M_FallingEdge;

	//������
	uint8_t *X_RisingEdge;
	uint8_t *Y_RisingEdge;
	uint8_t *M_RisingEdge;
}IO_Man_TypeDef;

//���������
#define X0  GPIO_ReadInputPin(IN_ABSORB_O_PORT, IN_ABSORB_O_PIN)
#define X1  GPIO_ReadInputPin(IN_ABSORB_L_PORT, IN_ABSORB_L_PIN)
#define X2  GPIO_ReadInputPin(IN_PRINT_O_PORT, IN_PRINT_O_PIN)
#define X3  GPIO_ReadInputPin(IN_PRINT_L_PORT, IN_PRINT_L_PIN)
#define X4  GPIO_ReadInputPin(IN_SHIFT_O_PORT, IN_SHIFT_O_PIN)
#define X5  GPIO_ReadInputPin(IN_POS_PORT, IN_POS_PIN)
#define X6  GPIO_ReadInputPin(IN_SHIFT_L1_PORT, IN_SHIFT_L1_PIN)
#define X7  GPIO_ReadInputPin(IN_SHIFT_L2_PORT, IN_SHIFT_L2_PIN)

//���������
#define Y0(x)  ((x)?GPIO_WriteHigh(OUT_UPDOWN_PORT, OUT_UPDOWN_PIN):GPIO_WriteLow(OUT_UPDOWN_PORT, OUT_UPDOWN_PIN))
#define Y1(x)  ((x)?GPIO_WriteHigh(OUT_FRONTBACK_PORT, OUT_FRONTBACK_PIN):GPIO_WriteLow(OUT_FRONTBACK_PORT, OUT_FRONTBACK_PIN))
#define Y2(x)  ((x)?GPIO_WriteHigh(OUT_SHIFT_PORT, OUT_SHIFT_PIN):GPIO_WriteLow(OUT_SHIFT_PORT, OUT_SHIFT_PIN))
#define Y3(x)  ((x)?GPIO_WriteHigh(OUT_POS_PORT, OUT_POS_PIN):GPIO_WriteLow(OUT_POS_PORT, OUT_POS_PIN))

//M �����м�̵�������
#define M_KEY_FAST              0
#define M_KEY_SLOW              1
#define M_KEY_AUX               2
#define M_KEY_UPDOWN            3
#define M_KEY_FRONTBACK         4
#define M_KEY_STARTSTOP         5
#define M_KEY_FOOT              6
#define M_KEY_CLEAR             7


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
#define M_MAN_FLAG_AUX          32
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

#define M_PUSH_CHECK            63
#define M_PUSH2_CHECK           64

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

//X
#define X_ABSORB_O        0
#define X_ABSORB_L        1
#define X_PRINT_O         2
#define X_PRINT_L         3
#define X_SHIFT_O         4
#define X_POS             5
#define X_SHIFT_L1        6
#define X_SHIFT_L2        7

//Y
#define Y_UPDOWN          0
#define Y_FRONTBACK       1
#define Y_SHIFT           2
#define Y_POS             3

extern uint8_t IN_State[IN_NUM];//������˲���
/*
	bit8 ��¼�����ƽ
	bit7 ������ʱ�Ŀ���
	bit6 ��ʱʱ�䵽��־
	bit5~0 ��ʱ��ʱ������
*/

void SIMPLC_IO_Init(void);
void SIMPLC_IO_Refresh(void);

uint8_t SIMPLC_IO_Get(uint8_t Component, uint8_t Index, uint8_t Type);
void SIMPLC_IO_Set(uint8_t Component, uint8_t Index, uint8_t Level);

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