#include "management.h"
#include "man.h"
#include "misc.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"
#include "timer.h"

void MachineSensorsHeadCheck(void)
{
    TM1638_SendData(0, "1");
  
    GXL(X_ABSORB_L) ? TM1638_SendData(1, "8") : TM1638_SendData(1, "-");
    GXL(X_ABSORB_O) ? TM1638_SendData(2, "8") : TM1638_SendData(2, "-");
    GXL(X_PRINT_O) ? TM1638_SendData(3, "8") : TM1638_SendData(3, "-");
    GXL(X_PRINT_L) ? TM1638_SendData(4, "8") : TM1638_SendData(4, "-");
}

void MachineSensorsPlatformCheck(void)
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
    
    //ProgramResetDefault();

    eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
    //eepromDefaultData = 0xAA;
//    if(eepromDefaultData == 0xAA)//初始化程序和数据
//    {
//        ProgramResetDefault();
//        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
//    }
    if(eepromDefaultData != 0x55)
    {
        //AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
        printf("eeprom read 1\r\n");
    }

    WDT_CONTR = 0x3C;

    if(eepromDefaultData != 0x55)
    {
        //AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
        printf("eeprom read 2\r\n");
    }

    WDT_CONTR = 0x3C;

    if(eepromDefaultData != 0x55)//=============初始化程序和数据
    {
        printf("Reset default data!!!\r\n");

        ProgramResetDefault();

        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
    }

    WDT_CONTR = 0x3C;

    if(eepromDefaultData != 0x55)
    {
        //AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
        printf("eeprom read 3\r\n");
    }

    WDT_CONTR = 0x3C;

    if(eepromDefaultData != 0x55)
    {
        //AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
        eepromDefaultData = AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT);
        printf("eeprom read 4\r\n");
    }

    WDT_CONTR = 0x3C;

    if(eepromDefaultData != 0x55)
    {
        printf("EEPROM is error!\r\n");
        while(1)
        {
            TM1638_SendData(0, "-----");            
            
            Timer3_Cmd(1);
            TS(0, 100);
            while(!TG(0))
                WDT_CONTR = 0x3C;
            Timer3_Cmd(0);
        }
    }
}

void MachineStateCheck(void)
{
    //SIMPLC_IO_Refresh();
    Key_Refresh();
    if(GML(M_KEY_AUX) || GML(M_KEY_FAST) || GML(M_KEY_SLOW))
        return;
  
    printf("Machine state checking...\r\n");
    
    TM1638_SendData(0, "    ");

    while(1)//检测印头
    {
        WDT_CONTR = 0x3C;

        SIMPLC_IO_Refresh();
        
        UartDataParse();

        if(GXL(X_ABSORB_O) && !GXL(X_ABSORB_L) && !GXL(X_PRINT_O) && !GXL(X_PRINT_L))
        {
            TM1638_SendData(0, "88888");
            break;
        }
        else if(!GXL(X_ABSORB_O) && GXL(X_ABSORB_L) && !GXL(X_PRINT_O) && !GXL(X_PRINT_L))
            TM1638_SendData(0, "01");
        else if(!GXL(X_ABSORB_O) && !GXL(X_ABSORB_L) && GXL(X_PRINT_O) && !GXL(X_PRINT_L))
            TM1638_SendData(0, "03");
        else if(!GXL(X_ABSORB_O) && !GXL(X_ABSORB_L) && !GXL(X_PRINT_O) && GXL(X_PRINT_L))
            TM1638_SendData(0, "04");
        else if(GXL(X_ABSORB_O) && GXL(X_ABSORB_L) && !GXL(X_PRINT_O) && !GXL(X_PRINT_L))
            TM1638_SendData(0, "01");
        else if(GXL(X_ABSORB_O) && !GXL(X_ABSORB_L) && GXL(X_PRINT_O) && !GXL(X_PRINT_L))
            TM1638_SendData(0, "03");
        else if(GXL(X_ABSORB_O) && !GXL(X_ABSORB_L) && !GXL(X_PRINT_O) && GXL(X_PRINT_L))
            TM1638_SendData(0, "04");
        else
            TM1638_SendData(0, "00");
        
        if(!GXL(X_ABSORB_O))
            TM1638_SendData(3, "20");
        else
            TM1638_SendData(3, "00");
    }
    
    printf("Machine state check finish!\r\n");
}

void MachineTypeCheck(void)
{
    uint16_t cnt = 0;
    uint16_t timeCount;
    
    if(man.programNum == 4)
        timeCount = 3000;
    else
        timeCount = 6000;  

    TM1638_SendData(0, "--A--");

    printf("Machine type checking...\r\n");

    while(cnt++ < timeCount)//检测平台
    {
        SIMPLC_IO_Refresh();
        
        WDT_CONTR = 0x3C;
        
        UartDataParse();
        
        if(man.programNum != 0 && man.programNum != 2 && man.programNum != 4 && man.programNum != 6 && man.programNum != 8)
            cnt = 0;

        if(man.programNum == 11)//程序11不检查平台
            break;

//        if(GXL(X_POS))
//        {
//            if(GXL(X_SHIFT_L1))
//                TM1638_SendData(4, "2");
//            else
//            {
//                man.machineType = MACHINE_2SENSORS;
//                
//                TM1638_SendData(0, "88888");
//                
//                printf("++++++++This is a 2 sensors machine!!!!\r\n");
//                
//                break;
//            }
//        }
//        else if(GXL(X_SHIFT_O))
//        {
//            if(GXL(X_SHIFT_L2))
//                TM1638_SendData(4, "1");
//            else if(GXL(X_SHIFT_L1))
//                TM1638_SendData(4, "2");
//            else if(GXL(X_POS))
//                TM1638_SendData(4, "3");
//            else
//            {
//                man.machineType = MACHINE_4SENSORS;
//                
//                TM1638_SendData(0, "88888");
//                
//                printf("++++++++This is a 4 sensors machine!!!!\r\n");
//                
//                break;
//            }
//        }
//        else
//            TM1638_SendData(0, "--A--");
        
//        if(man.programNum == 1 || man.programNum == 2) //单色程序不用检查平台
//            break;

        if(man.machineType == MACHINE_4SENSORS)
        {
            if(GXL(X_SHIFT_O) && !GXL(X_POS) && !GXL(X_SHIFT_L1) && !GXL(X_SHIFT_L2))
                break;
            else
                TM1638_SendData(0, "--A--");
        }
        else if(man.machineType == MACHINE_2SENSORS)
        {
#if MACHINE3SENSOR_SIM_2SENSOR            
            if(GXL(X_SHIFT_O) && !GXL(X_POS))  //2传感器12V设备，使用3传感器12V设备测试
#else
            if(GXL(X_POS) && !GXL(X_SHIFT_L1)) //正常2传感器5V设备
#endif
                break;
            else
                TM1638_SendData(0, "--A--");
        }
        else if(man.machineType == MACHINE_3SENSORS)
        {            
            if(GXL(X_SHIFT_O) && !GXL(X_POS) && !GXL(X_SHIFT_L1))
                break;
            else
                TM1638_SendData(0, "--A--");
        }
    }
    
    if(cnt >= timeCount)
    {
        SML(M_MACHINE_AUX_FAULT, 1);
        printf("AUX is fault!\r\n");
    }
    else
        printf("AUX is good! %d\r\n", (int)cnt);

    printf("Machine type check finish!\r\n");
}

//数码管闪烁
void Segment_Flash(void)
{
#define FLASH_PEROID 300

    TS(0, FLASH_PEROID);
    while(!TG(0))
        WDT_CONTR = 0x3C;
    TM1638_SendData(0, "     ");
    
    TS(0, FLASH_PEROID);
    while(!TG(0))
        WDT_CONTR = 0x3C;
    sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
    TM1638_SendData(0, man.segStr);

    TS(0, FLASH_PEROID);
    while(!TG(0))
        WDT_CONTR = 0x3C;
    TM1638_SendData(0, "     ");
    
    TS(0, FLASH_PEROID);
    while(!TG(0))
        WDT_CONTR = 0x3C;
    sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
    TM1638_SendData(0, man.segStr);
}

void LED_RedGreen(void)
{
    GYL(Y_UPDOWN) ? (man.led |= 0x02) : (man.led &= ~0x02); 
    GYL(Y_FRONTBACK) ? (man.led |= 0x01) : (man.led &= ~0x01);
    GYL(Y_SHIFT) ? (man.led |= 0x04) : (man.led &= ~0x04);  
    sprintf((char*)man.segStr, "%d", (int)(man.led));
    TM1638_LED(5, man.segStr);
}

void Key_Sound(void)
{
    if(GML(M_KEY_FAST) || GML(M_KEY_SLOW) || GML(M_KEY_AUX) || GML(M_KEY_UPDOWN) || GML(M_KEY_FRONTBACK)
       || GML(M_KEY_STARTSTOP) || GML(M_KEY_FOOT) || GML(M_KEY_CLEAR))
    {
        Timer3_Cmd(1);
        SML(M_BEEPER, 1);
        TS(2, 50);
    }
    if(GML(M_BEEPER) && TG(2))
    {
        Timer3_Cmd(0);
        SML(M_BEEPER, 0);
    }
}

void Output_Flash(void)
{
    if(GML(M_OUTPUT_FLASH))//
    {
        if(TG(3))
        {
            TS(3, OUTPUT_FLASH_PERIOD);

            sprintf((char*)man.segStr, "%05d", (int)(man.productOutput));
            TM1638_SendData(0, man.segStr);
        }
    }
    else
        TS(3, OUTPUT_FLASH_PERIOD);
}

void MCUID_Check(void)
{
    uint8_t i;
    for(i=0;i<7;i++)
    {
        if(man.mcuID[i] != man.eepromMcuID[i])
        {
            printf("Error:mcuID is wrong!\r\n");
            while(1)
            {
                TM1638_SendData(0, "--P--");
                WDT_CONTR = 0x3C;
                
                UartDataParse();      
            }
        }
    }
}
