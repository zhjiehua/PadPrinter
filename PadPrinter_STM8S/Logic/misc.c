#include "management.h"
#include "project.h"
#include "misc.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include <stm8s.h>
#include "io.h"
#include "uart1.h"
#include "tm1638.h"
#include "flash_eeprom.h"
#include "24cxx.h"
#include "tim5.h"

void MachineSensorsHead_Check(void)
{
    TM1638_SendData(0, "1");
  
    GXL(X_ABSORB_L) ? TM1638_SendData(1, "8") : TM1638_SendData(1, "-");
    GXL(X_ABSORB_O) ? TM1638_SendData(2, "8") : TM1638_SendData(2, "-");
    GXL(X_PRINT_O) ? TM1638_SendData(3, "8") : TM1638_SendData(3, "-");
    GXL(X_PRINT_L) ? TM1638_SendData(4, "8") : TM1638_SendData(4, "-");
}

void MachineSensorsPlatform_Check(void)
{
    TM1638_SendData(0, "2");
  
    GXL(X_SHIFT_L2) ? TM1638_SendData(1, "8") : TM1638_SendData(1, "-");
    GXL(X_SHIFT_L1) ? TM1638_SendData(2, "8") : TM1638_SendData(2, "-");
    GXL(X_POS) ? TM1638_SendData(3, "8") : TM1638_SendData(3, "-");
    GXL(X_SHIFT_O) ? TM1638_SendData(4, "8") : TM1638_SendData(4, "-");
}

void EEPROM_Check(void)
{
    uint8_t eepromDefaultData;
    
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
            
            if(!GML(M_BEEPER))
            {
                TIM2_Cmd(ENABLE);
                SML(M_BEEPER, 1);
                TS(2, 50);
                UART1_printf("Beeping Up\r\n");
            }
            else if(GML(M_BEEPER) && TG(2))
            {
                SML(M_BEEPER, 0);
                TIM2_Cmd(DISABLE);
                UART1_printf("Beeping Down\r\n");
            }
        }
    }
}

void MachineState_Check(void)
{
    //SIMPLC_IO_Refresh();
    Key_Refresh();
    if(GML(M_KEY_AUX))
        return;
  
    UART1_printf("Machine state checking...\r\n");

    TM1638_SendData(0, "o----");

    while(1)//���ӡͷ
    {
        SIMPLC_IO_Refresh();
        
        if(GXL(X_ABSORB_O))
        {
            TM1638_SendData(0, "88888");
            break;
        }
        
        if(GXL(X_ABSORB_L))
            TM1638_SendData(4, "1");
        else if(GXL(X_PRINT_O))
            TM1638_SendData(4, "3");
        else if(GXL(X_PRINT_L))
            TM1638_SendData(4, "4");
        else
            TM1638_SendData(0, "o----");
    }
    
    TM1638_SendData(0, "--o--");
    
    while(1)//���ƽ̨
    {
        SIMPLC_IO_Refresh();
        
        if(GXL(X_SHIFT_L1))
            TM1638_SendData(4, "2");
        else if(GXL(X_SHIFT_O))
        {
            if(GXL(X_SHIFT_L2))
                TM1638_SendData(4, "1");
            else if(GXL(X_POS))
                TM1638_SendData(4, "3");
            else
            {
                project.machineType = MACHINE_4SENSORS;
                
                TM1638_SendData(0, "88888");
                
                UART1_printf("++++++++This is a 4 sensors machine!!!!\r\n");
                
                break;
            }
        }
        else if(GXL(X_POS))
        {
                project.machineType = MACHINE_2SENSORS;
                
                TM1638_SendData(0, "88888");
                
                UART1_printf("++++++++This is a 2 sensors machine!!!!\r\n");
                
                break;
        }
        else
            TM1638_SendData(0, "--o--");
    }
    
    UART1_printf("Machine state check finish!\r\n");
}

void MachineType_Check(void)
{
    uint16_t cnt = 0;
  
    while(cnt < 500)
    {
        cnt++;
        SIMPLC_IO_Refresh();
    }
  
    if(GXL(X_SHIFT_L2) && GXL(X_SHIFT_O) && GXL(X_POS) && !GXL(X_SHIFT_L1))
    {
        project.machineType = MACHINE_2SENSORS;
        
        UART1_printf("++++++++This is a 2 sensors machine!!!!\r\n");
    }
    else
    {
        project.machineType = MACHINE_4SENSORS;
        
        UART1_printf("++++++++This is a 4 sensors machine!!!!\r\n");
    }
}

//�������˸
void Segment_Flash(void)
{
    if(GMR(M_SEG_FLASH))
    {
        UART1_printf("Segment flashing...\r\n");      
        TS(0, SEG_FLASH_PERIOD);
        project.segFlashTimes = SEG_FLASH_TIMES;
    }
    
    if(!GML(M_SEG_FLASHFLAG) && TG(0))
    {
        TM1638_SendData(0, "     ");
        
        SML(M_SEG_FLASHFLAG, 1);
        TS(0, 1000);
    }
    if(GML(M_SEG_FLASHFLAG) && TG(0))
    {
        sprintf((char*)project.segStr, "Pro%2d", project.programNum);
        TM1638_SendData(0, project.segStr);
        
        SML(M_SEG_FLASHFLAG, 0);
        TS(0, 1000);
        
        project.segFlashCnt++;
        if(project.segFlashCnt >= project.segFlashTimes)
        {
            SML(M_SEG_FLASH, 0);
            
            SML(M_PROGRAM_READY, 1);
            
            UART1_printf("PadPrinter is ready...\r\n");
            
            //��˸�����ʾ����
            sprintf((char*)project.segStr, "%5d", project.productOutput);
            TM1638_SendData(0, project.segStr);
            
            SML(M_OUTPUT_FLASH_FLAG, 1);
            TS(3, 500);
        }
    }
}

void LED_RedGreen(void)
{
    GYL(Y_UPDOWN) ? (project.led |= 0x02) : (project.led &= ~0x02); 
    GYL(Y_FRONTBACK) ? (project.led |= 0x01) : (project.led &= ~0x01);
    GYL(Y_SHIFT) ? (project.led |= 0x04) : (project.led &= ~0x04);  
    sprintf((char*)project.segStr, "%d", project.led);
    TM1638_LED(5, project.segStr);
}

void Key_Sound(void)
{
    if(GML(M_KEY_FAST) || GML(M_KEY_SLOW) || GML(M_KEY_AUX) || GML(M_KEY_UPDOWN) || GML(M_KEY_FRONTBACK)
       || GML(M_KEY_STARTSTOP) || GML(M_KEY_FOOT) || GML(M_KEY_CLEAR))
    {
        TIM2_Cmd(ENABLE);
        SML(M_BEEPER, 1);
        TS(2, 50);
    }
    if(GML(M_BEEPER) && TG(2))
    {
        SML(M_BEEPER, 0);
        TIM2_Cmd(DISABLE);
    }
}

void Output_Flash(void)
{
    if(TG(3))
    {
        TS(3, OUTPUT_FLASH_PERIOD);  
        
        if(GML(M_OUTPUT_FLASH))
            SML(M_OUTPUT_FLASH, 0);
        else
            SML(M_OUTPUT_FLASH, 1);
    }
    if(!GML(M_OUTPUT_FLASH))
    {
        sprintf((char*)project.segStr, "%5d", project.productOutput);
        TM1638_SendData(0, project.segStr);
    }
    else
    {
        TM1638_SendData(0, "     ");
    }
}