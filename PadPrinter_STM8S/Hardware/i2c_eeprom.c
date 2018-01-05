/**************** (C) COPYRIGHT  风驰电子嵌入式开发工作室 **********************
 * 文件名  ：i2c_eeprom.c
 * 描述    ：STM8S硬件I2C 总线主模式配置读写函数库    
 * 实验平台：风驰电子STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ:779814207
 * 博客    ：
 * 修改时间 ：2011-12-20

  风驰电子STM8开发板硬件连接
    |--------------------|
    |  I2C_SCL-PE1       |
    |  I2C_SDA-PE2       |
    |--------------------|

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_eeprom.h"
static void WaiteI2C_SDA_Idle(void);

/*******************************************************************************
 * 名称: I2C_MASTERMODE_Init
 * 功能: I2C主模式初始化
 * 形参: I2C_Speed
 * 返回: 无
 * 说明: 
               
 ******************************************************************************/
void I2C_MASTERMODE_Init(I2C_SpeedMode_TypeDef I2C_Speed)
{
	
	I2C_Init(I2C_Speed, I2C1_SLAVE_ADDRESS7, I2C_DUTYCYCLE_2,\
		I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq()/1000000);
	I2C_Cmd(ENABLE);
}

/*******************************************************************************
 * 名称: I2C_EEPROM_WriteOneByte
 * 功能: I2C主模式往I2C从器件内部地址写一个字节
 * 形参: WriteAddr  Byte
 * 返回: 无
 * 说明: 
               
 ******************************************************************************/
void I2C_EEPROM_WriteOneByte(u8 WriteAddr,u8 Byte)
{
	/* 等待空闲 */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* 发起始位 */
	I2C_GenerateSTART(ENABLE);
	
	/* 测试EV5 ，检测从器件返回一个应答信号*/
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
	
	/* 设置I2C从器件地址，I2C主设备为写模式*/
	I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
	
	/* 测试EV6并清除标志位，检测从器件返回一个应答信号 */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	 
	/* 设置往从器件写数据内部的地址 */
	I2C_SendData(WriteAddr);
	/* 测试EV8并清除标志位 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	/* 向从器件写一个字节 */
	I2C_SendData(Byte); 
	
	/* 测试EV8并清除标志位，检测从器件返回一个应答信号 */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
	
	/* 发结束位 */
	I2C_GenerateSTOP(ENABLE);
}

/*******************************************************************************
 * 名称: I2C_EEPROM_ReadOneByte
 * 功能: 从从设备读取一个字节
 * 形参: ReadAddr 
 * 返回: 字节数据
 * 说明: 无 
 ******************************************************************************/
u8 I2C_EEPROM_ReadOneByte(u8 ReadAddr)
{
	u8 RxData;
	/* 等待空闲 */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* 发起始位 */
	I2C_GenerateSTART(ENABLE);
	
	/* 测试EV5 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* 设置I2C从器件地址，I2C主设备为写模式*/ 
	I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
	
	/* 测试EV6并清除标志位 */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
        
	I2C_Cmd(ENABLE); 

	I2C_SendData(ReadAddr);

	/* 测试EV8并清除标志位 */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	/* 发起始位 */ 
	I2C_GenerateSTART(ENABLE);
	
	/* 测试EV5 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* 设置I2C从器件地址，I2C主设备为读模式*/
	I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_RX);
	
	/* 测试EV6 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	/* 测试EV7，检测从器件返回一个应答信号 */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
	RxData=I2C_ReceiveData();
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	
	/* 发结束位*/
	I2C_GenerateSTOP(ENABLE);
        return RxData;
}
/*******************************************************************************
 * 名称: I2C_EERROM_WriteOnePage
 * 功能: I2C主模式往I2C从器件内部地址写一个一页数据
 * 形参: pBuffer  WriteAddr   NumByteToWrite
 * 返回: 无
 * 说明: 对于AT24C02一页最大只能写8个Byte，如果超出8个Byte后将会覆盖页的首地址，
          然后依次存放下了，也就是说当从AT24C02的首地址00开始写10个字节，调用此
          函数，则00地址不是第1个字节的内容，而是第9个字节的内容，01地址则是第10
          个字节的内容，002依旧还是第3个字节的内容
               
 ******************************************************************************/
void I2C_EERROM_WriteOnePage(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
    /* 等待空闲 */
    while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
    
    /* 发起始位 */
    I2C_GenerateSTART(ENABLE);
    
    /* 测试EV5 ，检测从器件返回一个应答信号*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
    
    /* 设置I2C从器件地址，I2C主设备为写模式*/
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
  
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
  
    I2C_SendData((u8)(WriteAddr)); 
    /* 测试EV8 ，检测从器件返回一个应答信号*/
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));  
  
    /* 不断往从设备写数据*/
    while(NumByteToWrite--)  
    {
      
      I2C_SendData(*pBuffer); 
  
      /*指针指向下一个地址*/
      pBuffer++; 
    
      /* 测试EV6 ，检测从器件返回一个应答信号*/
      while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      I2C_AcknowledgeConfig(I2C_ACK_CURR);
    }
  
    /* 发结束位*/
    I2C_GenerateSTOP(ENABLE);
}

/*******************************************************************************
 * 名称: I2C_EEPROM_ReadBuffer
 * 功能: I2C主模式往I2C从器件读任意个字节数据，存放在pBuffer缓存
 * 形参: pBuffer  WriteAddr   NumByteToWrite
 * 返回: 无
 * 说明: 
               
 ******************************************************************************/
void I2C_EEPROM_ReadBuffer(u8* pBuffer, u8 ReadAddr, u8 NumByteToRead)
{  
    /* 等待空闲 */
    while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
  
    /* 发起始位 */
    I2C_GenerateSTART(ENABLE);
    /* 测试EV5 ，检测从器件返回一个应答信号*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* 设置I2C从器件地址，I2C主设备为写模式*/
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_TX);
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
   
    I2C_SendData((u8)(ReadAddr)); 
    /* 测试EV8 ，检测从器件返回一个应答信号*/
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
    /* 发起始位 */ 
    I2C_GenerateSTART(ENABLE);
    /* 测试EV5 ，检测从器件返回一个应答信号*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  
    /* 设置I2C从器件地址，I2C主设备为读模式*/
    I2C_Send7bitAddress(EEPROM_ADDRESS, I2C_DIRECTION_RX);
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    /*不断在从从设备读取数据 */
  while(NumByteToRead)  
  {

      /* 测试EV6 */
    if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)){      
      /*从 EEPROM 读取一个字节*/
      *pBuffer = I2C_ReceiveData();
      /* 指针指向下个存放字节的地址*/
      pBuffer++;  
      /* 读到最后一个字节*/
      if(NumByteToRead == 1){
      /* 不需要应答*/
        I2C_AcknowledgeConfig(I2C_ACK_NONE);
      /* 发结束位*/
        I2C_GenerateSTOP(ENABLE);  
      }
      else      
        /* 不是最后一个字节向从设备发送应答信号*/
        I2C_AcknowledgeConfig(I2C_ACK_CURR);
      NumByteToRead--;
    }   
  }
}

/*******************************************************************************
 * 名称: I2C_EEPROM_WriteMultiplePage
 * 功能: I2C主模式往I2C从器件写任意个字节数据
 * 形参: pBuffer  WriteAddr   NumByteToWrite
 * 返回: 无
 * 说明: 调用此函数可在AT24C02的任意地址开始写任意个(最大为255，由AT24C02决定)字节
               
 ******************************************************************************/
void I2C_EEPROM_WriteMultiplePage(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % EEPROM_Page_Byte_Size;/*不满一页的开始写的地址*/
  count = EEPROM_Page_Byte_Size - Addr;/*不满一页的地址剩余容量*/
  NumOfPage =  NumByteToWrite / EEPROM_Page_Byte_Size;/*写了完整的页数*/
  NumOfSingle = NumByteToWrite % EEPROM_Page_Byte_Size;/*写完完整页剩余的容量*/
 
  /* 写进的地址是在页的首地址  */
  if(Addr == 0){ 
    /*写进的字节数不足一页*/
    if(NumOfPage == 0) 
      I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumOfSingle);
     
    /*写进的字节数大于一页*/
    else{
      while(NumOfPage--){
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, EEPROM_Page_Byte_Size ); /*写一页*/
        WriteAddr +=  EEPROM_Page_Byte_Size ;
        pBuffer+= EEPROM_Page_Byte_Size ;
        WaiteI2C_SDA_Idle();
      }/*写完整页*/
      if(NumOfSingle!=0){/*写尾数*/
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* 假如写进的地址不在页的首地址*/
  else { 
  
    if (NumOfPage == 0) {/*写进的字节数不足一页 */
    
      if (NumOfSingle > count){
       /*要写完完整页剩余的容量大于不满一页的地址剩余容量*/
        temp = NumOfSingle - count;
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, count);/*把当前页的地址写完*/
        WaiteI2C_SDA_Idle();
        WriteAddr +=  count;
        pBuffer += count;
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, temp);/*在新的一页写剩余的字节*/
      }
      else
      {
        I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else{ /*写进的字节数大于一页*/
    
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / EEPROM_Page_Byte_Size;
      NumOfSingle = NumByteToWrite % EEPROM_Page_Byte_Size;

      I2C_EERROM_WriteOnePage(pBuffer, WriteAddr, count);/*把当前页的地址写完*/
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

/****************** (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/