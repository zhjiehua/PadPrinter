/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：flash_eeprom.c
 * 描述    ：Flash内部Data ROM读写实验   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-12-10
*******************************************************************************/
#ifndef __FLASH_EEPROM_H
#define __FLASH_EEPROM_H

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"
#include "stm8s_flash.h"

/* 自定义新类型 */
typedef enum{
  
  #if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) \
      || defined (STM8AF52Ax) || defined (STM8AF62Ax)
  Block_0,
  Block_1,
  Block_2,
  Block_3,
  Block_4,
  Block_5,
  Block_6,
  Block_7,
  Block_8,
  Block_9,
  Block_10,
  Block_11,
  Block_12,
  Block_13,
  Block_14,
  Block_15
  #endif /* STM8S208, STM8S207, STM8S007, STM8AF52Ax, STM8AF62Ax */
  #if defined(STM8S105) || defined(STM8S005) || defined(STM8AF626x)
  Block_0,
  Block_1,
  Block_2,
  Block_3,
  Block_4,
  Block_5,
  Block_6,
  Block_7 
  #endif /* STM8S105 or STM8AF626x */
  #if defined(STM8S103) || defined(STM8S003) ||  defined(STM8S903)
  Block_0,
  Block_1,
  Block_2,
  Block_3,
  Block_4,
  Block_5,
  Block_6,
  Block_7,
  Block_8,
  Block_9
  #endif /* STM8S103, STM8S903 */
  
}BlockStartAddress_TypeDef;
/* 自定义宏 */

/* 全局变量定义 */

void WriteMultiBlockByte(BlockStartAddress_TypeDef BlockStartAddress,FLASH_MemType_TypeDef FLASH_MemType, 
                FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer,uint8_t BlockNum);
void ReadMultiBlockByte(BlockStartAddress_TypeDef BlockStartAddress,uint8_t BlockNum,
                        uint8_t ReadBlockByte[]);

#endif

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/