/**************** (C) COPYRIGHT  ��۵���Ƕ��ʽ���������� **********************
 * �ļ���  ��i2c_ee.c
 * ����    ��I2C �������ö�д������    
 * ʵ��ƽ̨����۵���STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ:779814207
 * ����    ��
 * �޸�ʱ�� ��2011-12-20

  ��۵���STM8������Ӳ������
    |--------------------|
    |  I2C_SCL-PE1       |
    |  I2C_SDA-PE2       |
    |--------------------|

*******************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __I2C_EEPROM_H
#define __I2C_EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum{
  STANDARDSPEED=100000,
  FASTSPEED=400000
}I2C_SpeedMode_TypeDef;
/* Private define ------------------------------------------------------------*/

#define I2C1_SLAVE_ADDRESS7    0xA0
#define EEPROM_BASE_ADDRESS    0x00
#define EEPROM_Page_Byte_Size  8   /*EEPROM ÿҳ���д8Byte*/
#define EEPROM_ADDRESS         0xA0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2C_MASTERMODE_Init(I2C_SpeedMode_TypeDef I2C_Speed);
void I2C_EEPROM_WriteOneByte(u8 WriteAddr,u8 Byte);
u8 I2C_EEPROM_ReadOneByte(u8 ReadAddr);
void I2C_EERROM_WriteOnePage(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EEPROM_ReadBuffer(u8* pBuffer, u8 ReadAddr, u8 NumByteToRead);
void I2C_EEPROM_WriteMultiplePage(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);

#endif /* __I2C_EEPROM_H */

/******************* (C) COPYRIGHT ��۵���Ƕ��ʽ���������� *****END OF FILE****/


