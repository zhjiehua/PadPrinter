/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ********************
 * �ļ���  ��main.c
 * ����    �������������̶�Ƶ�ʵ����� 1K ,2K,4K     
 * ʵ��ƽ̨��iCreate STM8������
 * ��汾  ��V2.1.0
 * ����    �����
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-12-5
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_iwdg.h"
#include "sysclock.h"
#include "beep.h"
#include "tim2.h"
#include "uart2.h"
#include "tm1638.h"
#include "io.h"
#include "flash_eeprom.h"
#include "i2c_eeprom.h"
#include "24cxx.h"
#include "option.h"

#include "tim1.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include "project.h"

static void delay_ms( u32 nTime)
{
    uint16_t i,j,k;
	while(0 >= nTime)
    {
        for(i=0;i<2000;i++)
            for(j=0;j<2000;j++)
                for(k=0;k<2000;k++);
 
	  	nTime--;
    }
}

static void IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_128);
    IWDG_SetReload(0xFF);
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
    IWDG_Enable();
}

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
    uint16_t ledCnt = 0;
    uint8_t eepromDefaultData;
    
	/*�����ⲿʱ��24MΪ��ʱ��*/ 
	SystemClock_Init(HSI_Clock);//HSI_Clock  HSE_Clock
    
    SIMPLC_IO_Init();
    SIMPLC_Timer_Init();
    SIMPLC_Timer_Create(0, 1000, 0);
    SIMPLC_Timer_Create(1, 100, 0);
    SIMPLC_Timer_Create(2, 100, 0);
    
    enableInterrupts();
    
	Uart2_Init(115200);
    UART1_printf("\r\nHello PadPrinter!\r\n");
    
	//Beep_Init(BEEP_FREQUENCY_1KHZ);
	TIM2_PWM_Init();
    
    TM1638_Init();
    TM1638_SendData(0, "88888");
    //delay_ms(1500);
    //TS(0, 1000);
    //while(!TG(0));//��ʱ1S�鿴�������ʾ�Ƿ�����
    
    AT24CXX_Init();
    eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
    if(eepromDefaultData == 0xAA)//��ʼ�����������
    {
        ProgramResetDefault();
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
    }
    if(eepromDefaultData != 0x55)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
    }
    if(eepromDefaultData != 0x55)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
    }
    if(eepromDefaultData != 0x55)
    {
        while(1)
        {
            TM1638_SendData(0, "-----");
            UART1_printf("EEPROM is error!\r\n");
            
            TIM2_Cmd(ENABLE);
//            //delay_ms(30000);
//            TS(0, 1000);
//            TS(0, 1000);
//            while(!TG(0));
//            TIM2_Cmd(DISABLE);
//            //delay_ms(30000);
//            TS(0, 1000);
//            TS(0, 1000);
//            while(!TG(0));
        }
    }

    SML(M_POWERON, 1);
    SML(M_HOME_FINISH, 1);
    //IWDG_Init();
    
	while (1)
	{
#if 1
        //ι��
        //IWDG_ReloadCounter();
        
        //ˢ�°���
        Key_Refresh();
        
        //ˢ��IO��
		SIMPLC_IO_Refresh();
        
        //ִ�й���
        Project_Run();
        
        ledCnt++;
        if(ledCnt > 5000)
        {
            ledCnt = 0;
            //if(!GML(M_PROGRAM_AUTO))
                //UART1_printf("Running\r\n");
        }
//#else       
        UartDataParse();
#endif
      
    }
}





#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/
