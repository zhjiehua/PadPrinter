#include "management.h"
#include "misc.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"
#include "timer.h"

#include "simplc_io.h"
#include "simplc_timer.h"

void ProgramSelect(void)
{
    if(GMR(M_KEY_SLOW))//加程序  M_KEY_FAST  M_KEY_SLOW
    {
        if(man.programNum >= PROJECTNUM-1)
            man.programNum = 0;
        else
            man.programNum++;
        
        //刷新数码管
        sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
        TM1638_SendData(0, man.segStr);
    }
    else if(GMR(M_KEY_FAST))//减程序
    {
        if(man.programNum <= 0)
            man.programNum = PROJECTNUM-1;
        else
            man.programNum--;
        
        //刷新数码管
        sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
        TM1638_SendData(0, man.segStr);
    }
    else if(GMR(M_KEY_STARTSTOP))//选定程序，更新project结构体
    {
        //保存程序序号到EEPROM
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, man.programNum);
        
        printf("The selected Program is %s\r\n", man.segStr);
        
        while(1);
    }
    else if(GMR(M_KEY_AUX))//按10下进入内部参数设置程序
    {
#if(HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 4 && HARDWARE_VERSION_APPEND == 'B') //24B版本的SW8使用的是矩阵键盘
        if(GML(M_KEY_SW8))
#elif (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR <= 4)
        if(!INT_SW8)
#elif (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 5 && HARDWARE_VERSION_APPEND == 'A')//25A版本SW8改成拨码开关了
        if(1)
#endif
        {
            man.intSetAuxCnt++;
            if(man.intSetAuxCnt >= 10)
            {
                sprintf((char*)man.segStr, "%02d-%02d", (int)(man.intSetPos), (int)(man.internalSetting[man.intSetPos].val));
                TM1638_SendData(0, man.segStr);

                man.mode = PM_PARAMETER_SET;
            
                printf("Bootloader : Parameter setting...\r\n");


                man.machineType = (MachineType_TypeDef)AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE);
                man.platformSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL);
                man.headSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL);
                man.delayUnit = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAYUNIT);
                man.returnPosDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_RETURNPOSDELAYFACTOR);
                man.shiftPosDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_SHIFTPOSDELAYFACTOR);
                man.returnNoneDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_RETURNNONEDELAYFACTOR);
                man.shiftStopDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_SHIFTSTOPDELAYFACTOR);
                man.restoreDelay = AT24CXX_ReadOneByte(EEPROM_ADDR_RESTOREDELAY);
                man.frontBackDelay = AT24CXX_ReadOneByte(EEPROM_ADDR_FRONTBACKDELAY);

                printf("++++++++The return pos delay factor is %d\r\n", (int)man.returnPosDelayFactor);
                printf("++++++++The machine type is %d\r\n", (int)man.machineType);
                printf("++++++++The platform sensor level is %d\r\n", (int)man.platformSensorLevel);
                printf("++++++++The head sensor level is %d\r\n", (int)man.headSensorLevel);
                printf("++++++++The delay unit is %d\r\n", (int)man.delayUnit);               
                printf("++++++++The return pos delay factor is %d\r\n", (int)man.returnPosDelayFactor);
                printf("++++++++The shift pos delay factor is %d\r\n", (int)man.shiftPosDelayFactor);
                printf("++++++++The return none delay factor is %d\r\n", (int)man.returnNoneDelayFactor);
                printf("++++++++The shift stop delay factor is %d\r\n", (int)man.shiftStopDelayFactor);
                printf("++++++++The restore delay is %d\r\n", (int)man.restoreDelay);
                printf("++++++++The front back delay is %d\r\n", (int)man.frontBackDelay);

                man.internalSetting[INTERNALSETTING_DEFAULT].val = defaultInternalSetting[INTERNALSETTING_DEFAULT].val;
                man.internalSetting[INTERNALSETTING_MACHINETYPE].val = man.machineType;
                man.internalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val = man.platformSensorLevel;
                man.internalSetting[INTERNALSETTING_HEADSENSORLEVEL].val = man.headSensorLevel;
                man.internalSetting[INTERNALSETTING_DELAYUNIT].val = man.delayUnit;

                man.internalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].val = man.returnPosDelayFactor;
                man.internalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].val = man.shiftPosDelayFactor;
                man.internalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].val = man.returnNoneDelayFactor;
                man.internalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].val = man.shiftStopDelayFactor;

                man.internalSetting[INTERNALSETTING_RESTOREDALAY].val = man.restoreDelay;
                man.internalSetting[INTERNALSETTING_FRONTBACKDALAY].val = man.frontBackDelay;
            }
        }
    }
    else if(GMR(M_KEY_UPDOWN))
    {
#if(HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 4 && HARDWARE_VERSION_APPEND == 'B') //24B版本的SW8使用的是矩阵键盘
        if(GML(M_KEY_SW8))
#elif (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR <= 4)
        if(!INT_SW8)
#elif (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 5 && HARDWARE_VERSION_APPEND == 'A')//25A版本SW8改成拨码开关了
        if(1)
#endif
         {
            man.factoryModeCnt++;
            if(man.factoryModeCnt >= 10)
            {
                man.mode = PM_FACTORY;

#if !MACHINE_FIX    
                man.platformSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL);
                man.headSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL);
#else
                man.platformSensorLevel = defaultInternalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val;
                man.headSensorLevel = defaultInternalSetting[INTERNALSETTING_HEADSENSORLEVEL].val;
#endif
                        
                printf("Bootloader : Factory Mode...\r\n");
            }
         }
    }
}


void ParameterSet(void)
{
    if(GMR(M_KEY_SLOW))
    {
        man.intSetPos++;
        if(man.intSetPos > INTERNALSETTING_COUNT-1)
            man.intSetPos = 0;
        
        sprintf((char*)man.segStr, "%02d-%02d", (int)(man.intSetPos), (int)(man.internalSetting[man.intSetPos].val));
        TM1638_SendData(0, man.segStr);
    }
    else if(GMR(M_KEY_FAST))
    {
        man.intSetPos--;
        if(man.intSetPos >= 255)
            man.intSetPos = INTERNALSETTING_COUNT-1;
        
        sprintf((char*)man.segStr, "%02d-%02d", (int)(man.intSetPos), (int)(man.internalSetting[man.intSetPos].val));
        TM1638_SendData(0, man.segStr);
    }
    else if(GMR(M_KEY_UPDOWN))
    {
        man.internalSetting[man.intSetPos].val++; 
               
        if(man.internalSetting[man.intSetPos].val > defaultInternalSetting[man.intSetPos].max)
            man.internalSetting[man.intSetPos].val = defaultInternalSetting[man.intSetPos].min;
        
        sprintf((char*)man.segStr, "%02d", (int)(man.internalSetting[man.intSetPos].val));
        TM1638_SendData(3, man.segStr);
    }
    else if(GMR(M_KEY_FRONTBACK))
    {
        man.internalSetting[man.intSetPos].val--;

        if(defaultInternalSetting[man.intSetPos].min == 0)
        {
            if(man.internalSetting[man.intSetPos].val >= 255)
                man.internalSetting[man.intSetPos].val = defaultInternalSetting[man.intSetPos].max;
        }
        else
        {
            if(man.internalSetting[man.intSetPos].val < defaultInternalSetting[man.intSetPos].min)
                man.internalSetting[man.intSetPos].val = defaultInternalSetting[man.intSetPos].max;
        }

        sprintf((char*)man.segStr, "%02d", (int)(man.internalSetting[man.intSetPos].val));
        TM1638_SendData(3, man.segStr);
    }
    else if(GMR(M_KEY_STARTSTOP))
    {
        switch(man.intSetPos)
        {
            case INTERNALSETTING_DEFAULT://恢复出厂设置
                if(man.internalSetting[man.intSetPos].val)
                {
                    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0xAA);
                    printf("Reset default data!\r\n");
                }
                else
                    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
            break;
            case INTERNALSETTING_DELAYUNIT://调节动作延时的最小延时单位
                if(man.internalSetting[man.intSetPos].val > 0)
                {
                    AT24CXX_WriteOneByte(EEPROM_ADDR_DELAYUNIT, man.internalSetting[man.intSetPos].val);
                    printf("Change the delayUnit is %d!\r\n", (int)man.internalSetting[man.intSetPos].val);
                }
                else
                {
                    AT24CXX_WriteOneByte(EEPROM_ADDR_DELAYUNIT, 1);
                    printf("Change the delayUnit is 1!\r\n");
                }
            break;
            case INTERNALSETTING_MACHINETYPE://设备识别
                if(man.internalSetting[man.intSetPos].val == 1)
                {
                    man.machineType = MACHINE_2SENSORS;
                    printf("++++++++This is a 2 sensors machine!!!!\r\n");
                }
                else if(man.internalSetting[man.intSetPos].val == 2)
                {
                    man.machineType = MACHINE_3SENSORS;
                    printf("++++++++This is a 3 sensors machine!!!!\r\n");
                }
                else if(man.internalSetting[man.intSetPos].val == 0)
                {
                    man.machineType = MACHINE_0SENSORS;
                    printf("++++++++This is a 0 sensors machine!!!!\r\n");
                }
                else
                {
                    man.machineType = MACHINE_4SENSORS;
                    printf("++++++++This is a 4 sensors machine!!!!\r\n");
                }
                AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_PLATFORMSENSORLEVEL://平台传感器电平
                if(man.internalSetting[man.intSetPos].val > 0)
                {
                    man.platformSensorLevel = 1;
                    printf("++++++++This is a 12V sensors machine--platform!!!!\r\n");
                }
                else
                {
                    man.platformSensorLevel = 0;
                    printf("++++++++This is a 5V sensors machine--platform!!!!\r\n");
                }
                AT24CXX_WriteOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL, man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_HEADSENSORLEVEL://印头传感器电平
                if(man.internalSetting[man.intSetPos].val > 0)
                {
                    man.headSensorLevel = 1;
                    printf("++++++++This is a 12V sensors machine--head!!!!\r\n");
                }
                else
                {
                    man.headSensorLevel = 0;
                    printf("++++++++This is a 5V sensors machine--head!!!!\r\n");
                }
                AT24CXX_WriteOneByte(EEPROM_ADDR_HEADSENSORLEVEL, man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_RETURNPOSDELAYFACTOR://
                AT24CXX_WriteOneByte(EEPROM_ADDR_RETURNPOSDELAYFACTOR, man.internalSetting[man.intSetPos].val);
                printf("++++++++The return pos delay factor is %d\r\n", (int)man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_SHIFTPOSDELAYFACTOR://
                AT24CXX_WriteOneByte(EEPROM_ADDR_SHIFTPOSDELAYFACTOR, man.internalSetting[man.intSetPos].val);
                printf("++++++++The shift pos delay factor is %d\r\n", (int)man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_RETURNNONEDELAYFACTOR://
                AT24CXX_WriteOneByte(EEPROM_ADDR_RETURNNONEDELAYFACTOR, man.internalSetting[man.intSetPos].val);
                printf("++++++++The return none delay factor is %d\r\n", (int)man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_SHIFTSTOPDELAYFACTOR://
                AT24CXX_WriteOneByte(EEPROM_ADDR_SHIFTSTOPDELAYFACTOR, man.internalSetting[man.intSetPos].val);
                printf("++++++++The shift stop delay factor is %d\r\n", (int)man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_RESTOREDALAY://
                AT24CXX_WriteOneByte(EEPROM_ADDR_RESTOREDELAY, man.internalSetting[man.intSetPos].val);
                printf("++++++++The restore delay is %d\r\n", (int)man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_FRONTBACKDALAY://
                AT24CXX_WriteOneByte(EEPROM_ADDR_FRONTBACKDELAY, man.internalSetting[man.intSetPos].val);
                printf("++++++++The front back delay is %d\r\n", (int)man.internalSetting[man.intSetPos].val);
            break;
            default:
            break;
        }
    }
}


