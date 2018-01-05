
#include <stm8s.h>
#include "beep.h"
#include "uart1.h"
#include "tim2.h"

#include <stdio.h>

void Beep_Init(BEEP_Frequency_TypeDef BEEP_Frequency)
{  
#ifdef DEBUG
	AlternateFunctionRemapping(AFR7_PD4_BEEP);
#endif
	CLK_LSICmd(ENABLE);
	BEEP_LSICalibrationConfig(LSI_128kHz);
	BEEP_Init(BEEP_Frequency);
	BEEP_Cmd(DISABLE);
}
