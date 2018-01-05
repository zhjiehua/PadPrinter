/**************** (C) COPYRIGHT  ��۵���Ƕ��ʽ���������� **********************
 * �ļ���  ��i2c_eeprom.c
 * ����    ��STM8SӲ��I2C ������ģʽ���ö�д������    
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

/* Includes ------------------------------------------------------------------*/
#include "i2c_eeprom.h"
static void WaiteI2C_SDA_Idle(void);

/*******************************************************************************
 * ����: I2C_MASTERMODE_Init
 * ����: I2C��ģʽ��ʼ��
 * �β�: I2C_Speed
 * ����: ��
 * ˵��: 
               
 ******************************************************************************/
void I2C_MASTERMODE_Init(I2C_SpeedMode_TypeDef I2C_Speed)
{
	
	I2C_Init(I2C_Speed, I2C1_SLAVE_ADDRESS7, I2C_DUTYCYCLE_2,\
		I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq()/1000000);
	I2C_Cmd(ENABLE);
}

/*******************************************************************************
 * ����: I2C_EEPROM_WriteOneByte
 * ����: I2C��ģʽ��I2C�������ڲ���ַдһ���ֽ�
 * �β�: WriteAddr  Byte
 * ����: ��
 * ˵��: 
               
 ******************************************************************************/
void I2C_EEPROM_WriteOneByte(u8 WriteAddr,u8 Byte)
{
	/* �ȴ����� */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* ����ʼλ */
	I2C_GenerateSTART(ENABLE);
	
	/* ����EV5 ��������������һ��Ӧ���ź�*/
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
	
	/* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
	I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
	
	/* ����EV6�������־λ��������������һ��Ӧ���ź� */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	 
	/* ������������д�����ڲ��ĵ�ַ */
	I2C_SendData(WriteAddr);
	/* ����EV8�������־λ ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	/* �������дһ���ֽ� */
	I2C_SendData(Byte); 
	
	/* ����EV8�������־λ��������������һ��Ӧ���ź� */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
	
	/* ������λ */
	I2C_GenerateSTOP(ENABLE);
}

/*******************************************************************************
 * ����: I2C_EEPROM_ReadOneByte
 * ����: �Ӵ��豸��ȡһ���ֽ�
 * �β�: ReadAddr 
 * ����: �ֽ�����
 * ˵��: �� 
 ******************************************************************************/
u8 I2C_EEPROM_ReadOneByte(u8 ReadAddr)
{
	u8 RxData;
	/* �ȴ����� */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* ����ʼλ */
	I2C_GenerateSTART(ENABLE);
	
	/* ����EV5 ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* ����I2C��������ַ��I2C���豸Ϊдģʽ*/ 
	I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
	
	/* ����EV6�������־λ */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
        
	I2C_Cmd(ENABLE); 

	I2C_SendData(ReadAddr);

	/* ����EV8�������־λ */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	/* ����ʼλ */ 
	I2C_GenerateSTART(ENABLE);
	
	/* ����EV5 ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* ����I2C��������ַ��I2C���豸Ϊ��ģʽ*/
	I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_RX);
	
	/* ����EV6 ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	/* ����EV7��������������һ��Ӧ���ź� */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
	RxData=I2C_ReceiveData();
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	
	/* ������λ*/
	I2C_GenerateSTOP(ENABLE);
        return RxData;
}
/*******************************************************************************
 * ����: I2C_EERROM_WriteOnePage
 * ����: I2C��ģʽ��I2C�������ڲ���ַдһ��һҳ����
 * �β�: pBuffer  WriteAddr   NumByteToWrite
 * ����: ��
 * ˵��: ����AT24C02һҳ���ֻ��д8��Byte���������8��Byte�󽫻Ḳ��ҳ���׵�ַ��
          Ȼ�����δ�����ˣ�Ҳ����˵����AT24C02���׵�ַ00��ʼд10���ֽڣ����ô�
          ��������00��ַ���ǵ�1���ֽڵ����ݣ����ǵ�9���ֽڵ����ݣ�01��ַ���ǵ�10
          ���ֽڵ����ݣ�002���ɻ��ǵ�3���ֽڵ�����
               
 ******************************************************************************/
void I2C_EERROM_WriteOnePage(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
    /* �ȴ����� */
    while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
    
    /* ����ʼλ */
    I2C_GenerateSTART(ENABLE);
    
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
    
    /* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
  
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
  
    I2C_SendData((u8)(WriteAddr)); 
    /* ����EV8 ��������������һ��Ӧ���ź�*/
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));  
  
    /* ���������豸д����*/
    while(NumByteToWrite--)  
    {
      
      I2C_SendData(*pBuffer); 
  
      /*ָ��ָ����һ����ַ*/
      pBuffer++; 
    
      /* ����EV6 ��������������һ��Ӧ���ź�*/
      while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_AcknowledgeConfig(I2C_ACK_CURR);
    }
  
    /* ������λ*/
    I2C_GenerateSTOP(ENABLE);
}

/*******************************************************************************
 * ����: I2C_EEPROM_ReadBuffer
 * ����: I2C��ģʽ��I2C��������������ֽ����ݣ������pBuffer����
 * �β�: pBuffer  WriteAddr   NumByteToWrite
 * ����: ��
 * ˵��: 
               
 ******************************************************************************/
void I2C_EEPROM_ReadBuffer(u8* pBuffer, u8 ReadAddr, u8 NumByteToRead)
{  
    /* �ȴ����� */
    while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
  
    /* ����ʼλ */
    I2C_GenerateSTART(ENABLE);
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
   
    I2C_SendData((u8)(ReadAddr)); 
    /* ����EV8 ��������������һ��Ӧ���ź�*/
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
    /* ����ʼλ */ 
    I2C_GenerateSTART(ENABLE);
    /* ����EV5 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* ����I2C��������ַ��I2C���豸Ϊ��ģʽ*/
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_RX);
    /* ����EV6 ��������������һ��Ӧ���ź�*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    /*�����ڴӴ��豸��ȡ���� */
  while(NumByteToRead)  
  {

      /* ����EV6 */
    if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)){      
      /*�� EEPROM ��ȡһ���ֽ�*/
      *pBuffer = I2C_ReceiveData();
      /* ָ��ָ���¸�����ֽڵĵ�ַ*/
      pBuffer++;  
      /* �������һ���ֽ�*/
      if(NumByteToRead == 1){
      /* ����ҪӦ��*/
        I2C_AcknowledgeConfig(I2C_ACK_NONE);
      /* ������λ*/
        I2C_GenerateSTOP(ENABLE);  
      }
      else      
        /* �������һ���ֽ�����豸����Ӧ���ź�*/
        I2C_AcknowledgeConfig(I2C_ACK_CURR);
      NumByteToRead--;
    }   
  }
}

/*******************************************************************************
 * ����: I2C_EEPROM_WriteMultiplePage
 * ����: I2C��ģʽ��I2C������д������ֽ�����
 * �β�: pBuffer  WriteAddr   NumByteToWrite
 * ����: ��
 * ˵��: ���ô˺�������AT24C02�������ַ��ʼд�����(���Ϊ255����AT24C02����)�ֽ�
               
 ******************************************************************************/
void I2C_EEPROM_WriteMultiplePage(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % EEPROM_Page_Byte_Size;/*����һҳ�Ŀ�ʼд�ĵ�ַ*/
  count = EEPROM_Page_Byte_Size - Addr;/*����һҳ�ĵ�ַʣ������*/
  NumOfPage =  NumByteToWrite / EEPROM_Page_Byte_Size;/*д��������ҳ��*/
  NumOfSingle = NumByteToWrite % EEPROM_Page_Byte_Size;/*д������ҳʣ�������*/
 
  /* д���ĵ�ַ����ҳ���׵�ַ  */
  if(Addr == 0){ 
    /*д�����ֽ�������һҳ*/
    if(NumOfPage == 0) 
      I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumOfSingle);
     
    /*д�����ֽ�������һҳ*/
    else{
      while(NumOfPage--){
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, EEPROM_Page_Byte_Size ); /*дһҳ*/
        WriteAddr +=  EEPROM_Page_Byte_Size ;
        pBuffer+= EEPROM_Page_Byte_Size ;
        WaiteI2C_SDA_Idle();
      }/*д����ҳ*/
      if(NumOfSingle!=0){/*дβ��*/
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* ����д���ĵ�ַ����ҳ���׵�ַ*/
  else { 
  
    if (NumOfPage == 0) {/*д�����ֽ�������һҳ */
    
      if (NumOfSingle > count){
       /*Ҫд������ҳʣ����������ڲ���һҳ�ĵ�ַʣ������*/
        temp = NumOfSingle - count;
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, count);/*�ѵ�ǰҳ�ĵ�ַд��*/
        WaiteI2C_SDA_Idle();
        WriteAddr +=  count;
        pBuffer += count;
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, temp);/*���µ�һҳдʣ����ֽ�*/
      }
      else
      {
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else{ /*д�����ֽ�������һҳ*/
    
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / EEPROM_Page_Byte_Size;
      NumOfSingle = NumByteToWrite % EEPROM_Page_Byte_Size;

      I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, count);/*�ѵ�ǰҳ�ĵ�ַд��*/
      WaiteI2C_SDA_Idle();
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, EEPROM_Page_Byte_Size);
        WaiteI2C_SDA_Idle();
        WriteAddr +=  EEPROM_Page_Byte_Size;
        pBuffer += EEPROM_Page_Byte_Size;
      }

      if (NumOfSingle != 0)
      {
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  WaiteI2C_SDA_Idle();
}
static void WaiteI2C_SDA_Idle(void)
{
  u32 nCount=0xfff;
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

/****************** (C) COPYRIGHT ��۵���Ƕ��ʽ���������� *****END OF FILE****/