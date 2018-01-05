/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��flash_eeprom.c
 * ����    ��Flash�ڲ�Data ROM��дʵ��   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-12-10
*******************************************************************************/
#ifndef __FLASH_EEPROM_H
#define __FLASH_EEPROM_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "stm8s.h"
#include "stm8s_flash.h"

/* �Զ��������� */
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
/* �Զ���� */

/* ȫ�ֱ������� */

void WriteMultiBlockByte(BlockStartAddress_TypeDef BlockStartAddress,FLASH_MemType_TypeDef FLASH_MemType, 
                FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer,uint8_t BlockNum);
void ReadMultiBlockByte(BlockStartAddress_TypeDef BlockStartAddress,uint8_t BlockNum,
                        uint8_t ReadBlockByte[]);

#endif

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/