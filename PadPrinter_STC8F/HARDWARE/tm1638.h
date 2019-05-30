/*
 * Copyright (c) 2013, Wei Shuai <cpuwolf@gmail.com>. All rights reserved.
 *
 */
#ifndef __TM1638_H_
#define __TM1638_H_

#define STB_PORT GPIOA                          //chip-select line  
#define CLK_PORT GPIOF                          //clock line  
#define DIO_PORT GPIOA                          //data line 
#define STB_PIN GPIO_PIN_2                      //chip-select line  
#define CLK_PIN GPIO_PIN_4                      //clock line  
#define DIO_PIN GPIO_PIN_1                      //data line 

#define KEY_NONE        0
#define KEY_FAST        1
#define KEY_SLOW        2
#define KEY_AUX         3
#define KEY_UPDOWN      4
#define KEY_FRONTBACK   5
#define KEY_STARTSTOP   6
#define KEY_FOOT        7
#define KEY_CLEAR       8
#define KEY_SW8         9
#define KEY_SW8_AUX     10

uint8_t TM1638_ReadKey(uint8_t mode);
void TM1638_LED(uint8_t i, uint8_t *str);
void TM1638_SendData(uint8_t i, uint8_t *str);
void TM1638_Init(void);

void TM1638_LEDIndependentControl(uint8_t i, uint16_t str);

#endif
