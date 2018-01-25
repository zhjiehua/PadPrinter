
#ifndef __MISC_H_
#define __MISC_H_

#include "stm8s.h"

#define SEG_FLASH_PERIOD    (1000/2)
#define SEG_FLASH_TIMES     (2)

#define OUTPUT_FLASH_PERIOD    (1000/2)

#define DELAY_KEY_FILTER_PERIOD    (100)

void MachineSensorsHead_Check(void);
void MachineSensorsPlatform_Check(void);
void EEPROM_Check(void);

void MachineState_Check(void);
void MachineType_Check(void);

void Segment_Flash(void);
void LED_RedGreen(void);
void Key_Sound(void);
void Output_Flash(void);

#endif