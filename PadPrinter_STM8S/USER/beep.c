/***************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� *****************/


#include "beep.h"
#include "option.h"
#define DEBUG

void Beep_Init(BEEP_Frequency_TypeDef BEEP_Frequency)
{
	#ifdef DEBUG
	AlternateFunctionRemapping(AFR7_PD4_BEEP);
	#endif
	CLK_LSICmd(ENABLE);
	BEEP_LSICalibrationConfig(LSI_128kHz);
	BEEP_Init(BEEP_Frequency);
	BEEP_Cmd(ENABLE);
}





/*************** (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/