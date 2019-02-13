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
        if(!INT_SW8)
        {
            man.intSetAuxCnt++;
            if(man.intSetAuxCnt >= 10)
            {
                sprintf((char*)man.segStr, "%02d-%02d", (int)(man.intSetPos), (int)(man.internalSetting[man.intSetPos].val));
                TM1638_SendData(0, man.segStr);

                man.mode = PM_PARAMETER_SET;
            
                printf("Bootloader : Parameter setting...\r\n");


                man.machineType = (MachineType_TypeDef)AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE);
                man.sensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_SENSORLEVEL);
                man.delayUnit = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAYUNIT);
                man.returnPosDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_RETURNPOSDELAYFACTOR);
                man.shiftPosDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_SHIFTPOSDELAYFACTOR);
                man.returnNoneDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_RETURNNONEDELAYFACTOR);
                man.shiftStopDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_SHIFTSTOPDELAYFACTOR);
        
                printf("++++++++The return pos delay factor is %d\r\n", (int)man.returnPosDelayFactor);
                printf("++++++++The machine type is %d\r\n", (int)man.machineType);
                printf("++++++++The sensor level is %d\r\n", (int)man.sensorLevel);
                printf("++++++++The delay unit is %d\r\n", (int)man.delayUnit);               
                printf("++++++++The return pos delay factor is %d\r\n", (int)man.returnPosDelayFactor);
                printf("++++++++The shift pos delay factor is %d\r\n", (int)man.shiftPosDelayFactor);
                printf("++++++++The return none delay factor is %d\r\n", (int)man.returnNoneDelayFactor);
                printf("++++++++The shift stop delay factor is %d\r\n", (int)man.shiftStopDelayFactor);

                man.internalSetting[INTERNALSETTING_DEFAULT].val = defaultInternalSetting[INTERNALSETTING_DEFAULT].val;
                man.internalSetting[INTERNALSETTING_DEFAULT].min = defaultInternalSetting[INTERNALSETTING_DEFAULT].min;
                man.internalSetting[INTERNALSETTING_DEFAULT].max = defaultInternalSetting[INTERNALSETTING_DEFAULT].max;
                man.internalSetting[INTERNALSETTING_MACHINETYPE].val = man.machineType;
                man.internalSetting[INTERNALSETTING_MACHINETYPE].min = defaultInternalSetting[INTERNALSETTING_MACHINETYPE].min;
                man.internalSetting[INTERNALSETTING_MACHINETYPE].max = defaultInternalSetting[INTERNALSETTING_MACHINETYPE].max;
                man.internalSetting[INTERNALSETTING_SENSORLEVEL].val = man.sensorLevel;
                man.internalSetting[INTERNALSETTING_SENSORLEVEL].min = defaultInternalSetting[INTERNALSETTING_SENSORLEVEL].min;
                man.internalSetting[INTERNALSETTING_SENSORLEVEL].max = defaultInternalSetting[INTERNALSETTING_SENSORLEVEL].max;
                man.internalSetting[INTERNALSETTING_DELAYUNIT].val = man.delayUnit;
                man.internalSetting[INTERNALSETTING_DELAYUNIT].min = defaultInternalSetting[INTERNALSETTING_DELAYUNIT].min;
                man.internalSetting[INTERNALSETTING_DELAYUNIT].max = defaultInternalSetting[INTERNALSETTING_DELAYUNIT].max;
        
                man.internalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].val = man.returnPosDelayFactor;
                man.internalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].min = defaultInternalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].min;
                man.internalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].max = defaultInternalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].max;
                man.internalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].val = man.shiftPosDelayFactor;
                man.internalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].min = defaultInternalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].min;
                man.internalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].max = defaultInternalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].max;
                man.internalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].val = man.returnNoneDelayFactor;
                man.internalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].min = defaultInternalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].min;
                man.internalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].max = defaultInternalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].max;
                man.internalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].val = man.shiftStopDelayFactor;
                man.internalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].min = defaultInternalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].min;
                man.internalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].max = defaultInternalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].max; 
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
        if(man.internalSetting[man.intSetPos].val > man.internalSetting[man.intSetPos].max)
            man.internalSetting[man.intSetPos].val = man.internalSetting[man.intSetPos].min;
        
        sprintf((char*)man.segStr, "%02d", (int)(man.internalSetting[man.intSetPos].val));
        TM1638_SendData(3, man.segStr);
    }
    else if(GMR(M_KEY_FRONTBACK))
    {
        man.internalSetting[man.intSetPos].val--;
        if(man.internalSetting[man.intSetPos].min == 0)
        {
            if(man.internalSetting[man.intSetPos].val >= 255)
                man.internalSetting[man.intSetPos].val = man.internalSetting[man.intSetPos].max;
        }
        else
        {
            if(man.internalSetting[man.intSetPos].val < man.internalSetting[man.intSetPos].min)
                man.internalSetting[man.intSetPos].val = man.internalSetting[man.intSetPos].max;
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
                else
                {
                    man.machineType = MACHINE_4SENSORS;
                    printf("++++++++This is a 4 sensors machine!!!!\r\n");
                }
                AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, man.internalSetting[man.intSetPos].val);
            break;
            case INTERNALSETTING_SENSORLEVEL://传感器电平
                if(man.internalSetting[man.intSetPos].val > 0)
                {
                    man.sensorLevel = 1;
                    printf("++++++++This is a 12V sensors machine!!!!\r\n");
                }
                else
                {
                    man.sensorLevel = 0;
                    printf("++++++++This is a 5V sensors machine!!!!\r\n");
                }
                AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL, man.internalSetting[man.intSetPos].val);
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
            default:
            break;
        }
    }
}


