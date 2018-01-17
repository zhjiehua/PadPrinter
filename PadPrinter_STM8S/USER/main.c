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
#include "misc.h"

static void IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
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
    
	/*�����ⲿʱ��24MΪ��ʱ��*/ 
	SystemClock_Init(HSI_Clock);//HSI_Clock  HSE_Clock
    
    SIMPLC_IO_Init();
    SIMPLC_Timer_Init();
    SIMPLC_Timer_Create(0, 1000, 0);//ACTIN DELAY
    SIMPLC_Timer_Create(1, 100, 0);//KEY DELAY FAST SLOW
    SIMPLC_Timer_Create(2, 100, 0);//KEY BEEP
    SIMPLC_Timer_Create(3, 100, 0);//OUTPUT FLASH
    
    Uart2_Init(115200);
    UART1_printf("\r\nHello PadPrinter!\r\n");
    
    //Beep_Init(BEEP_FREQUENCY_1KHZ);
	TIM2_PWM_Init();

    AT24CXX_Init();
    
    TM1638_Init();
    TM1638_SendData(0, "88888");
    
    enableInterrupts();

    //��ʱ1S�鿴�������ʾ�Ƿ�����
//    TS(1, 200);
//    while(!TG(1));
//    while(1)
//    {
//        ledCnt++;
//        if(ledCnt > 5000)
//        {
//            ledCnt = 0;
//            break;
//        }
//    }

    EEPROM_Check();

    MachineState_Check();//�Ѽ���������
    
    //MachineType_Check();
    
    while(ledCnt < 2)
    {
        if(!GML(M_BEEPER))
        {
            TIM2_Cmd(ENABLE);
            SML(M_BEEPER, 1);
            TS(2, 200);
            //UART1_printf("Beeping Up\r\n");
        }
        else if(GML(M_BEEPER) && TG(2))
        {
            SML(M_BEEPER, 0);
            TIM2_Cmd(DISABLE);
            //UART1_printf("Beeping Down\r\n");
            
            ledCnt++;
        }
    }
    
    SML(M_POWERON, 1);
    SML(M_HOME_FINISH, 1);
    TS(1, 100);
    
    IWDG_Init();
    
	while (1)
	{
#if 1
        //ι��
        IWDG_ReloadCounter();
        
        //ˢ�°���
        Key_Refresh();
        
        //ˢ��IO��
		SIMPLC_IO_Refresh();
        
        //ִ�й���
        Project_Run();
        
//        if(GXL(X_SHIFT_L1))
//            UART1_printf("X_SHIFT_L1\r\n");
//        if(GXL(X_POS))
//            UART1_printf("X_POS\r\n");

//        if(GXL(X_SHIFT_L2))
//            UART1_printf("X_SHIFT_L2\r\n");
//        if(GXL(X_SHIFT_L1))
//            UART1_printf("X_SHIFT_L1\r\n");
//        if(GXL(X_POS))
//            UART1_printf("X_POS\r\n");
//        if(GXL(X_SHIFT_O))
//            UART1_printf("X_SHIFT_O\r\n");
        
//        ledCnt++;
//        if(ledCnt > 5000)
//        {
//            ledCnt = 0;
//            //if(!GML(M_PROGRAM_AUTO))
//                //UART1_printf("Running\r\n");
//        }
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
