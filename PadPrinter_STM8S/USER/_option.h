/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：sysclock.c
 * 描述    ：系统时钟设置   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-11-9

*******************************************************************************/
#ifndef __OPTION_H
#define __OPTION_H
/* 包含系统头文件 */
#include "stm8s.h"
#include "sysclock.h"
/* 包含自定义头文件 */
/* 声明给外部使用的变量 */
/* 自定义常量宏和表达式宏 */
//typedef enum{
//  ROP_ADDRESS=(uint16_t)0x4800,
//  UBC_ADDRESS=(uint16_t)0x4801,
//  AFR_ADDRESS=(uint16_t)0x4803,
//  WTD_ADDRESS=(uint16_t)0x4805,
//  CLK_OPTION_ADDRESS=(uint16_t)0x4807,
//  HSE_CLOCK_STARTUP_ADDRESS=(uint16_t)0x4809,
//  FLASH_WAIT_STATES_ADDRESS=(uint16_t)0x480D,
//  BOOTLOADER_ADDRESS=(uint16_t)0x487E
//  
//}OptionByte_Address_TypeDef;

typedef enum{
  ROP=(uint8_t)0xaa,
  WTD_LSI_DISENABLE=(uint8_t)0xf7,
  WTD_LSI_ENABLE=(uint8_t)0x08,
  WTD_IWDG_SOFT_EN=(uint8_t)0xfb,
  WTD_IWDG_HARD_EN=(uint8_t)0x04,
  WTD_WWDG_SOFT_EN=(uint8_t)0xfd,
  WTD_WWDG_HARD_EN=(uint8_t)0x02,
  WTD_WWDG_HALT_DISRESET=(uint8_t)0xfe,
  WTD_WWDG_HALT_RESET=(uint8_t)0x01,
  CLK_EXTCLK_OSCIN_OUT=(uint8_t)0xf7,
  CLK_EXTCLK_OSCIN=(uint8_t)0x08,
  CLK_CKAWUSEL_LSI=(uint8_t)0xfb,
  CLK_CKAWUSEL_HSE=(uint8_t)0x04,
  FLASH_NOWAIT=(uint8_t)0xfe,
  FLASH_WAITONETIME=(uint8_t)0x01,

}OptionByte_TypeDef;

typedef enum{
  
  AFR7_PD4_BEEP=(uint8_t)0x80,
  AFR6_PB4_5_I2C_SDA_SCL=(uint8_t)0x40,
  AFR5_PB3_2_1_0_TIM1_ETR_CH2_1_0=(uint8_t)0x20,
  AFR4_PD7_TIM_CH4=(uint8_t)0x10,
  AFR3_PD0_TIM1_BKIN=(uint8_t)0x08,
  AFR2_PD0_CLK_CCO=(uint8_t)0x04,
  AFR1_PA3_TIM3_CH1_PD2_TIM2_CH3=(uint8_t)0x02,
  AFR0_PD3_ADC_ETR=(uint8_t)0x01 
}Remap_Function_TypeDef;

typedef enum{
  AFR7_PD4_TIM5_CH1=(uint8_t)0x7f,
  AFR6_PB5_AIN5_PB4_AIN4=(uint8_t)0xbf,
  AFR5_PB3_2_1_0_AIN3_2_1_0=(uint8_t)0xdf,
  AFR4_PD7_TLI=(uint8_t)0xef,
  AFR3_PD0_TIM3_CH2=(uint8_t)0xf7,
  AFR2_PD0_TIM3_CH2=(uint8_t)0xfb,
  AFR1_PA3_TIM2_CH3_PD2_TIM3_CH1=(uint8_t)0xfd,
  AFR0_PD3_TIM2_CH2=(uint8_t)0xfe
}Default_Function_TypeDef;

/* 声明给外部使用的函数 */
void AlternateFunctionRemapping(Remap_Function_TypeDef Function);
void DefaultFunctionRemapping(Default_Function_TypeDef Function);
void DefaultAllFunctionRemapping(void);
uint8_t ReadOptionByte(OptionByte_Address_TypeDef Option_ADDRESS);
#endif


/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/