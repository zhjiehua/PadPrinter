#include "management.h"
#include "project.h"
#include "misc.h"
#include "actions.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include <stm8s.h>
#include "io.h"
#include "uart1.h"
#include "tm1638.h"
#include "flash_eeprom.h"
#include "24cxx.h"
#include "tim5.h"

//如果自编程需要手动则将该宏改为1
#define PROGRAMSELF_ACTION  0


//按键刷新
void Key_Refresh(void)
{
    uint8_t key = TM1638_ReadKey(0);
    uint8_t i;

    for(i=0;i<8;i++)
        SML(i, 0);

    if(key)
        SML(TM1638_ReadKey(0)-1, 1);
}

//编写自编程序
void ProgramSelf(void)
{
    if(GMR(M_PROGRAM_SELF))
    {
        ProgramRead();
        
        project.programSelfPos = 0;
        
        sprintf((char*)project.segStr, "%02d-", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%02d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    
    if(GMR(M_KEY_FAST))//加步骤
    {
        project.programSelfPos++;
        if(project.programSelfPos >= MAX_ACTIONS_PER_PROJECT-1)
            project.programSelfPos = 0;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%02d-", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%02d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    else if(GMR(M_KEY_SLOW))//减步骤
    {
        project.programSelfPos--;
        if(project.programSelfPos >= 255)
            project.programSelfPos = MAX_ACTIONS_PER_PROJECT-1;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%02d-", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%02d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    
    //--------------------------------------------------------------------------------------
    if(GMR(M_KEY_UPDOWN))//加程序
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);
        
        if(project.printHeadPos&FRONTBACK_MASK)
        {
            if(project.printHeadPos&UPDOWN_MASK)
            {
#if PROGRAMSELF_ACTION
                //PrintUp();
                SML(M_MAN_UPDOWN, 1);
                SML(M_MODE_MANUAL, 1);
#else
                project.printHeadPos &= ~UPDOWN_MASK;          
#endif
                project.pCurProSelfAction->act = PRINT_UP;
            }
            else
            {
#if PROGRAMSELF_ACTION
                //PrintDown();
                SML(M_MAN_UPDOWN, 1);
                SML(M_MODE_MANUAL, 1);
#else
                project.printHeadPos |= UPDOWN_MASK; 
#endif
                project.pCurProSelfAction->act = PRINT_DOWN;
            }
        }
        else
        {
#if PROGRAMSELF_ACTION   
            //Absorb();
            SML(M_MAN_UPDOWN, 1);
            SML(M_MODE_MANUAL, 1);
#endif
            
            project.pCurProSelfAction->act = ABSORB;
        }
        
        project.programSelfPos++;
        if(project.programSelfPos >= MAX_ACTIONS_PER_PROJECT-3)
            project.programSelfPos = 0;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%02d-", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%02d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    else if(GMR(M_KEY_FRONTBACK))//减程序
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);   
      
        if(project.printHeadPos&UPDOWN_MASK)
        {
            ;
        }
        else
        {
            if(project.printHeadPos&FRONTBACK_MASK)
            {
                //Back();
#if PROGRAMSELF_ACTION            
                SML(M_MAN_FRONTBACK, 1);
                SML(M_MODE_MANUAL, 1);
#else
                project.printHeadPos &= ~FRONTBACK_MASK;
#endif
            
                project.pCurProSelfAction->act = BACK;
            }
            else
            {
#if PROGRAMSELF_ACTION            
                SML(M_MAN_FRONTBACK, 1);
                SML(M_MODE_MANUAL, 1);
#else
                project.printHeadPos |= FRONTBACK_MASK;
#endif
                project.pCurProSelfAction->act = FRONT;
            }

            
        }
        
        project.programSelfPos++;
        if(project.programSelfPos >= MAX_ACTIONS_PER_PROJECT-3)
            project.programSelfPos = 0;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%02d-", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%02d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    
    if(GMR(M_KEY_AUX))//修改flag
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);

#if PROGRAMSELF_ACTION  
        //Shift();
        SML(M_MAN_AUX, 1);
        SML(M_MODE_MANUAL, 1);
#endif
        
        if(project.machineType == MACHINE_2SENSORS)
        {
            if(!project.platformPushFlag)
            {
                project.pCurProSelfAction->act = PUSH;
                project.platformPushFlag = 1;
            }
            else
            {
                project.pCurProSelfAction->act = RETURN;
                project.platformPushFlag = 0;
            }
        }
        else
            project.pCurProSelfAction->act = SHIFT;
        
        project.programSelfPos++;
        if(project.programSelfPos >= MAX_ACTIONS_PER_PROJECT-3)
            project.programSelfPos = 0;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%02d-", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%02d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    
    if(GMR(M_KEY_STARTSTOP))
    {
        if(GML(M_PROGRAM_SELF_CHANGE))
        {
            UART1_printf("Saving the Program0...\r\n");
            
            project.pCurProSelfAction->act = HOME;
            project.programSelfPos++;
            project.pCurProSelfAction = project.program + project.programSelfPos;
            project.pCurProSelfAction->act = ACTION_NONE;
            
            Program0Save();
            
            //
            project.programNum = 0;
            AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, project.programNum);
            AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, project.programNum);
            AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, project.programNum);
        }
        
        UART1_printf("Saved the Program0 finish!\r\n");
        
        project.mode = NORMAL;
        
        SML(M_SEG_FLASH, 1);
       
        while(1);
    }

#if PROGRAMSELF_ACTION  
    
    if(GML(M_MODE_MANUAL))
    {
        if(GML(M_MAN_FRONTBACK))
        {
            if(project.printHeadPos&UPDOWN_MASK)
            {
                UART1_printf("Warnning : Can not run the Back or Front action!\r\n");
            }
            else
            {
                if(project.printHeadPos&FRONTBACK_MASK)
                {
                    Back();
                }
                else
                {
                    Front();
                }
            }
        }
        
        if(GML(M_MAN_UPDOWN))
        {
            if(project.printHeadPos&FRONTBACK_MASK)
            {
                if(project.printHeadPos&UPDOWN_MASK)
                {
                    PrintUp();
                }
                else
                {
                    PrintDown();
                }
            }
            else
            {
                Absorb();
            }
        }
        
        if(GML(M_MAN_AUX))
        {
            if(project.machineType == MACHINE_2SENSORS)
            {
                if(project.platformPushFlag)
                    Push();
                else
                    Return();
            }
            else
            {
                if(GML(M_MAN_AUX_RETURN))
                    Return();
                else
                    Shift();
                
                SML(M_MAN_FLAG_AUX, 1);
            }
        }
        
        if(!GML(M_MAN_UPDOWN) && !GML(M_MAN_FRONTBACK) && !GML(M_MAN_AUX))
            SML(M_MODE_MANUAL, 0);
    }
#endif
    
}

//选择程序
void ProgramSelect(void)
{
    if(GMR(M_PROGRAM_SEL))//刷新一次上次关机前的程序序号
    {
        UART1_printf("Selecting Program...\r\n");
        
        //读取程序序号
        project.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);
        
        //刷新数码管
        sprintf((char*)project.segStr, "Pro%02d", project.programNum);
        TM1638_SendData(0, project.segStr);
        
        project.machineType = (MachineType_TypeDef)AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE);
        project.sensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_SENSORLEVEL);
        project.actionUnit = AT24CXX_ReadOneByte(EEPROM_ADDR_ACTIONUNIT);
        
        project.internalSetting[INTERNALSETTING_DEFAULT].val = defaultInternalSetting[INTERNALSETTING_DEFAULT].val;
        project.internalSetting[INTERNALSETTING_DEFAULT].min = defaultInternalSetting[INTERNALSETTING_DEFAULT].min;
        project.internalSetting[INTERNALSETTING_DEFAULT].max = defaultInternalSetting[INTERNALSETTING_DEFAULT].max;
        project.internalSetting[INTERNALSETTING_MACHINETYPE].val = project.machineType;
        project.internalSetting[INTERNALSETTING_MACHINETYPE].min = defaultInternalSetting[INTERNALSETTING_MACHINETYPE].min;
        project.internalSetting[INTERNALSETTING_MACHINETYPE].max = defaultInternalSetting[INTERNALSETTING_MACHINETYPE].max;
        project.internalSetting[INTERNALSETTING_SENSORLEVEL].val = project.sensorLevel;
        project.internalSetting[INTERNALSETTING_SENSORLEVEL].min = defaultInternalSetting[INTERNALSETTING_SENSORLEVEL].min;
        project.internalSetting[INTERNALSETTING_SENSORLEVEL].max = defaultInternalSetting[INTERNALSETTING_SENSORLEVEL].max;
        project.internalSetting[INTERNALSETTING_ACTIONUNIT].val = project.actionUnit;
        project.internalSetting[INTERNALSETTING_ACTIONUNIT].min = defaultInternalSetting[INTERNALSETTING_ACTIONUNIT].min;
        project.internalSetting[INTERNALSETTING_ACTIONUNIT].max = defaultInternalSetting[INTERNALSETTING_ACTIONUNIT].max;
    }
    
    if(GMR(M_KEY_SLOW))//加程序  M_KEY_FAST  M_KEY_SLOW
    {
        if(!GML(M_INTSET))
        {
            if(project.programNum >= PROJECTNUM-1)
                project.programNum = 0;
            else
                project.programNum++;
            
            //刷新数码管
            sprintf((char*)project.segStr, "Pro%02d", project.programNum);
            TM1638_SendData(0, project.segStr);
        }
        else
        {
            project.intSetPos++;
            if(project.intSetPos > INTERNALSETTING_COUNT-1)
                project.intSetPos = 0;
            
            sprintf((char*)project.segStr, "%02d-%02d", project.intSetPos, project.internalSetting[project.intSetPos].val);
            TM1638_SendData(0, project.segStr);
        }
    }
    else if(GMR(M_KEY_FAST))//减程序
    {
        if(!GML(M_INTSET))
        {
            if(project.programNum <= 0)
                project.programNum = PROJECTNUM-1;
            else
                project.programNum--;
            
            //刷新数码管
            sprintf((char*)project.segStr, "Pro%02d", project.programNum);
            TM1638_SendData(0, project.segStr);
        }
        else
        {
            project.intSetPos--;
            if(project.intSetPos >= 255)
                project.intSetPos = INTERNALSETTING_COUNT-1;
            
            sprintf((char*)project.segStr, "%02d-%02d", project.intSetPos, project.internalSetting[project.intSetPos].val);
            TM1638_SendData(0, project.segStr);
        }
    }
    else if(GMR(M_KEY_STARTSTOP))//选定程序，更新project结构体
    {
        if(!GML(M_INTSET))
        {
            //保存程序序号到EEPROM
            AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, project.programNum);
            
            UART1_printf("The selected Program is %s\r\n", project.segStr);
            
            //选择完后退出选择程序
//            if(project.programNum == 0)
//            {
//                project.mode = PROGRAM_SELF;
//                SML(M_PROGRAM_SELF, 1);
//            }
//            else
            {
                project.mode = NORMAL;
                SML(M_PROGRAM_READ, 1);
            }
        }
        else//-------------------------------------------------------------------------------
        {
            //SML(M_INTSET, 0);
            
            switch(project.intSetPos)
            {
                case 0://恢复出厂设置
                    if(project.internalSetting[project.intSetPos].val)
                    {
                        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0xAA);
                        UART1_printf("Reset default data!\r\n");
                    }
                    else
                        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
                break;
                case 1://调节动作延时的最小延时单位
                    if(project.internalSetting[project.intSetPos].val > 0)
                    {
                        AT24CXX_WriteOneByte(EEPROM_ADDR_ACTIONUNIT, project.internalSetting[project.intSetPos].val);
                        UART1_printf("Change the actionUnit is %d!\r\n", project.internalSetting[project.intSetPos].val);
                    }
                    else
                    {
                        AT24CXX_WriteOneByte(EEPROM_ADDR_ACTIONUNITDELAY, 1);
                        UART1_printf("Change the actionUnit is 1!\r\n");
                    }
                break;
                case 2://设备识别
                    if(project.internalSetting[project.intSetPos].val > 0)
                    {
                        project.machineType = MACHINE_2SENSORS;
                        UART1_printf("++++++++This is a 2 sensors machine!!!!\r\n");
                    }
                    else
                    {
                        project.machineType = MACHINE_4SENSORS;
                        UART1_printf("++++++++This is a 4 sensors machine!!!!\r\n");
                    }
                    AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, project.internalSetting[project.intSetPos].val);
                break;
                case 3://传感器电平
                    if(project.internalSetting[project.intSetPos].val > 0)
                    {
                        project.sensorLevel = 1;
                        UART1_printf("++++++++This is a 12V sensors machine!!!!\r\n");
                    }
                    else
                    {
                        project.sensorLevel = 0;
                        UART1_printf("++++++++This is a 5V sensors machine!!!!\r\n");
                    }
                    AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL, project.internalSetting[project.intSetPos].val);
                break;
                default:
                break;
            }
        }
    }
    else if(GMR(M_KEY_AUX))
    {
        if(!GML(M_INTSET))
        {
            project.intSetAuxCnt++;
            if(project.intSetAuxCnt >= 10)
            {
                sprintf((char*)project.segStr, "%02d-%02d", project.intSetPos, project.internalSetting[project.intSetPos].val);
                TM1638_SendData(0, project.segStr);
                
                SML(M_INTSET, 1);
            }
        }
        else
        {
            SML(M_INTSET, 0);
            
            sprintf((char*)project.segStr, "Pro%02d", project.programNum);
            TM1638_SendData(0, project.segStr);
        }
    }
    else if(GML(M_INTSET))
    {
        if(GMR(M_KEY_UPDOWN))
        {
            project.internalSetting[project.intSetPos].val++;
            if(project.internalSetting[project.intSetPos].val > project.internalSetting[project.intSetPos].max)
                project.internalSetting[project.intSetPos].val = project.internalSetting[project.intSetPos].min;
            
            sprintf((char*)project.segStr, "%02d", project.internalSetting[project.intSetPos].val);
            TM1638_SendData(3, project.segStr);
        }
        else if(GMR(M_KEY_FRONTBACK))
        {
            project.internalSetting[project.intSetPos].val--;
            if(project.internalSetting[project.intSetPos].min == 0)
            {
                if(project.internalSetting[project.intSetPos].val >= 255)
                    project.internalSetting[project.intSetPos].val = project.internalSetting[project.intSetPos].max;
            }
            else
            {
                if(project.internalSetting[project.intSetPos].val < project.internalSetting[project.intSetPos].min)
                    project.internalSetting[project.intSetPos].val = project.internalSetting[project.intSetPos].max;
            }
            
            sprintf((char*)project.segStr, "%02d", project.internalSetting[project.intSetPos].val);
            TM1638_SendData(3, project.segStr);
        }
    }
}

//正常启动后的按键识别
void Key_Distinguish(void)
{
    if(GML(M_CAN_CHANGE_DELAY))
    {
        if(TG(1))
        {
            if(GML(M_KEY_SLOW))//快  M_KEY_SLOW  M_KEY_FAST
            {
                SML(M_OUTPUT_FLASH_FLAG, 0);
                SML(M_SAVE_DELAY, 1);

                project.pCurAction->delay++;
                if(project.pCurAction->delay > MAX_ACTIONUNIT_DELAY)
                    project.pCurAction->delay = MAX_ACTIONUNIT_DELAY;
                
                project.pCurAction->flag |= 0x80;//修改标志位，提示需要保存
                
                //UART1_printf("Add delay time.\r\n");
                
                sprintf((char*)project.segStr, "%5d", project.pCurAction->delay);
                TM1638_SendData(0, project.segStr);
            }
            else if(GML(M_KEY_FAST))//慢
            {
                SML(M_OUTPUT_FLASH_FLAG, 0);
                SML(M_SAVE_DELAY, 1);
                
                project.pCurAction->delay--;
                if(project.pCurAction->delay == 0 || project.pCurAction->delay >= MAX_ACTIONUNIT_DELAY)
                    project.pCurAction->delay = 0;
                
                project.pCurAction->flag |= 0x80;//修改标志位，提示需要保存
                
                //UART1_printf("Dec delay time.\r\n");
                
                sprintf((char*)project.segStr, "%5d", project.pCurAction->delay);
                TM1638_SendData(0, project.segStr);
            }
            else
                SML(M_OUTPUT_FLASH_FLAG, 1);
            
            TS(1, DELAY_KEY_FILTER_PERIOD);
        }
    }
    
    if(GML(M_MODE_AUTO))//正在执行自动程序
    {
        if(GMR(M_KEY_STARTSTOP))
        {
            SML(M_CHANGEMODE_PAUSE, 1);
            
            UART1_printf("Stop the program.\r\n");
            
            SML(M_PROGRAM_FOOT, 1);//表示开机后已按过启动
        }
        if(GMR(M_KEY_AUX))//辅助
        {
            if(!GML(M_PROGRAM_AUX) && !GML(M_MACHINE_AUX_FAULT))
            {
                SML(M_PROGRAM_AUX, 1);
                
                //SML(M_MAN_FLAG_AUX, 0);
                
                SML(M_AUTO_FLAG_AUX, 1);
                
                UART1_printf("Add the auxiliary action to current program.\r\n");
            }
        }
        if(GMR(M_KEY_FOOT))//脚踏
        {
            SML(M_CHANGEMODE_PAUSE, 1);
            
            UART1_printf("Foot---Stop the program.\r\n");
        }
        
        if(GMR(M_KEY_UPDOWN))//上下
        {
            SML(M_AUTO_MAN_UPDOWN, 1);
        }
        if(GMR(M_KEY_FRONTBACK))//前后
        {
            SML(M_AUTO_MAN_FRONTBACK, 1);
        }
    }
    else// if(project.programMode == MANUAL)
    {
        if(GMR(M_KEY_STARTSTOP))//启动停止
        {
            if(GML(M_FLAG_MANUAL))
            {
                if(GML(M_MAN_FRONTBACK))//前后动作正在动作，则等到前后动作停止后才启动自动程序
                    SML(M_MAN_FRONTBACK_AUTO, 1);
                else
                {
                    SML(M_FLAG_MANUAL, 0);
                    
                    UART1_printf("\r\nRestore the initial status.\r\n");
                    
                    SML(M_MODE_RESTORE, 1);
                    
                    //SML(M_PROGRAM_AUX, 0);
                    
                    if(project.printHeadPos&UPDOWN_MASK)
                        SML(M_RESTORE_UP, 1);
                    
                    SML(M_RESTORE_HOME, 1);
                }
            }
            else
            {
                SML(M_MODE_AUTO, 1);//启动自动程序
                
                UART1_printf("Start the program...\r\n");
            }
            
            SML(M_PROGRAM_FOOT, 1);//表示开机后已按过启动
        }
        if(GMR(M_KEY_AUX))//辅助
        {
            if(!GML(M_MACHINE_AUX_FAULT))
            {
                SML(M_MODE_MANUAL, 1);
                
                SML(M_FLAG_MANUAL, 1);
                
                SML(M_MAN_AUX, 1);
            }
        }
        if(GMR(M_KEY_UPDOWN))//上下
        {
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            SML(M_MAN_UPDOWN, 1);
        }
        if(GMR(M_KEY_FRONTBACK))//前后
        {
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            if(GML(M_MAN_FRONTBACK) && !GML(M_MAN_FRONTBACK_STOP))
                SML(M_MAN_FRONTBACK_STOP, 1);
            else
                SML(M_MAN_FRONTBACK, 1);
        }
        if(GMR(M_KEY_FOOT))//脚踏
        {
            if(!GML(M_PROGRAM_FOOT))
            {
                SML(M_PROGRAM_PERIOD, 1);
            }
            
            SML(M_MODE_AUTO, 1);
        }
    }
}

//进入下一个动作
void NextAction(void)
{
    if(!GML(M_PROGRAM_AUX))
    {
        do
        {
            project.actionPos++;
            if(project.actionPos >= project.actionCnt)
            {
                project.productOutput++;
                //sprintf((char*)project.segStr, "%05d", project.productOutput);
                //TM1638_SendData(0, project.segStr);
                
                AT24CXX_WriteLenByte(project.productOutputAddr, project.productOutput, 2);
                
                if(GML(M_PROGRAM_PERIOD))
                {
                    SML(M_MODE_AUTO, 0);
                    SML(M_PROGRAM_PERIOD, 0);
                }
                
                //保存修改过的延时参数
                if(GML(M_SAVE_DELAY))
                {   
                    uint8_t i;
                    //ACTION_TypeDef *pCurDelayAction;
                  
                    for(i=0;i<project.actionCnt;i++)
                    {
                        project.pCurDelayAction = project.program + i;
                        if(project.pCurDelayAction->flag&0x80)
                        {
                            project.pCurDelayAction->flag &= ~0x80;
                            
                            IWDG_ReloadCounter();
                            
                            AT24CXX_WriteOneByte(project.programAddr+i*3+2, project.pCurDelayAction->delay);
                            AT24CXX_WriteOneByte(project.programAddr+i*3+2, project.pCurDelayAction->delay);
                            AT24CXX_WriteOneByte(project.programAddr+i*3+2, project.pCurDelayAction->delay);
                            
                            IWDG_ReloadCounter();
                            
                            UART1_printf("Save the action %d delay : %d\r\n", i, project.pCurDelayAction->delay);
                        }
                    }
                    SML(M_SAVE_DELAY, 0);
                }
                
                project.actionPos = 0;
                
                UART1_printf("\r\n");
            }
            
            project.pCurAction = project.program + project.actionPos; 
        }while(!(project.pCurAction->flag&0x01));
        
        if(project.pCurAction->act == ABSORB
           || project.pCurAction->act == ABSORB_DOWN
           || project.pCurAction->act == PRINT
           || project.pCurAction->act == PRINT_DOWN)
        {
            SML(M_CAN_CHANGE_DELAY, 1);
        }
        else
            SML(M_CAN_CHANGE_DELAY, 0);
    }
    else
    {
        project.actionPos++;
        if(project.actionPos >= project.actionCnt)
        {
            project.productOutput++;
            //sprintf((char*)project.segStr, "%05d", project.productOutput);
            //TM1638_SendData(0, project.segStr);
            
            AT24CXX_WriteLenByte(project.productOutputAddr, project.productOutput, 2);
            
            if(GML(M_PROGRAM_PERIOD))
            {
                SML(M_MODE_AUTO, 0);
                SML(M_PROGRAM_PERIOD, 0);
            }
            
            //保存修改过的延时参数
            if(GML(M_SAVE_DELAY))
            {
                uint8_t i;
                //ACTION_TypeDef *pCurDelayAction;
              
                for(i=0;i<project.actionCnt;i++)
                {
                    project.pCurDelayAction = project.program + i;
                    if(project.pCurDelayAction->flag&0x80)
                    {
                        project.pCurDelayAction->flag &= ~0x80;
                        
                        IWDG_ReloadCounter();
                        
                        AT24CXX_WriteOneByte(project.programAddr+i*3+2, project.pCurDelayAction->delay);
                        AT24CXX_WriteOneByte(project.programAddr+i*3+2, project.pCurDelayAction->delay);
                        AT24CXX_WriteOneByte(project.programAddr+i*3+2, project.pCurDelayAction->delay);
                        
                        IWDG_ReloadCounter();
                        
                        UART1_printf("Save the action %d delay : %d\r\n", i, project.pCurDelayAction->delay);
                    }
                }
                SML(M_SAVE_DELAY, 0);
            }
            
            project.actionPos = 0;
            
            UART1_printf("\r\n");
        }
        
        project.pCurAction = project.program + project.actionPos;
        
        if(project.pCurAction->act == ABSORB
           || project.pCurAction->act == ABSORB_DOWN
           || project.pCurAction->act == PRINT
           || project.pCurAction->act == PRINT_DOWN)
        {
            SML(M_CAN_CHANGE_DELAY, 1);
        }
        else
        {
            SML(M_CAN_CHANGE_DELAY, 0);
        }
    }
    
    if(!(project.printHeadPos&UPDOWN_MASK))//暂停不能压
    {
        if(GML(M_CHANGEMODE_PAUSE))
        {
            SML(M_MODE_AUTO, 0);
            SML(M_CHANGEMODE_PAUSE, 0);
        }
        
        if(GML(M_AUTO_MAN_UPDOWN))
        {
            SML(M_MODE_AUTO, 0);
            
            SML(M_AUTO_MAN_UPDOWN, 0);
            
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            SML(M_MAN_UPDOWN, 1);
        }
        else if(GML(M_AUTO_MAN_FRONTBACK))
        {
            SML(M_MODE_AUTO, 0);
            
            SML(M_AUTO_MAN_FRONTBACK, 0);
            
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            if(GML(M_MAN_FRONTBACK) && !GML(M_MAN_FRONTBACK_STOP))
                SML(M_MAN_FRONTBACK_STOP, 1);
            else
                SML(M_MAN_FRONTBACK, 1);
        }
    }
    
    project.delay = (*(project.pCurAction)).delay;
    
    if(GML(M_AUTO_FLAG_AUX))
    {
        SML(M_AUTO_FLAG_AUX, 0);
        
        SML(M_MODE_AUTO, 0);
        SML(M_MODE_RESTORE, 1);
        
        if(project.printHeadPos&UPDOWN_MASK)
            SML(M_RESTORE_UP, 1);
        
        SML(M_RESTORE_HOME, 1);
        
        UART1_printf("\r\nRestore the initial status.\r\n");
    }
}

void Home_Check(void)
{
    if(project.machineType == MACHINE_4SENSORS)
    {
        if(project.platformState == 1)
        {
            if(!GXL(X_SHIFT_L2))
            {
                SYL(Y_POS, 0);//再将定位气缸收回来
                
                project.platformState = 2;
            }
//            else
//                UART1_printf("Action----Home Return----Waiting for X_SHIFT_L2\r\n");
        }
        else if(project.platformState == 2)
        {
            if(GXL(X_SHIFT_O))
            {
                SML(M_HOME_FINISH, 1);
                SML(M_HOME_CHECK, 0);
                
                project.platformPos = 0;
                
                project.platformState = 0;
                
                UART1_printf("Action----Home Return----Finish\r\n");
            }
//            else
//                UART1_printf("Action----Home Return----Waiting for X_SHIFT_O\r\n");
        }
    }
    else
    {
        if(GXL(X_POS))
        {
            SML(M_HOME_FINISH, 1);
            SML(M_HOME_CHECK, 0);
            
            project.platformPos = 0;
            
            project.platformState = 0;
            
            UART1_printf("Action----Home Return----Finish\r\n");
        }
    }
}

void Push_Check(void)
{
    if(project.platformState == 1)
    {
        if(GXL(X_SHIFT_L1))
        {
            SYL(Y_SHIFT, 0);
            
            project.platformPos = 1;
            project.platformState = 2;
        }
    }
    else if(project.platformState == 2)
    {
        if(GXL(X_POS))
        {
            SYL(Y_POS, 0);  
          
            SML(M_HOME_FINISH, 1);
            SML(M_PUSH_CHECK, 0);
            
            project.platformPos = 0;
            
            project.platformState = 0;
            
            UART1_printf("Action----PushHome----Finish\r\n");
        }
    }
}

void Push2_Check(void)
{
    if(project.platformState == 1)
    {
        if(GXL(X_SHIFT_L1))
        {
            SYL(Y_SHIFT, 0);
            
            project.platformPos = 1;
            project.platformState = 2;
        }
    }
    else if(project.platformState == 2)
    {
        if(GXL(X_POS))
        {
            SYL(Y_POS, 0);  
          
            //SML(M_HOME_FINISH, 1);
            //SML(M_PUSH_CHECK, 0);
            
            project.platformPos = 0;
            
            project.platformState = 3;
            
            //UART1_printf("Action----PushHome----Finish\r\n");
        }
    }
    else if(project.platformState == 3)
    {
        SYL(Y_SHIFT, 1);
        SYL(Y_POS, 1);
        
        project.platformState = 4;
    }
    else if(project.platformState == 4)
    {
        if(GXL(X_SHIFT_L1))
        {
            SYL(Y_SHIFT, 0);
            
            //SML(M_HOME_FINISH, 1);
            
            project.platformPos = 1;
            project.platformState = 5;
        }
    }
    else if(project.platformState == 5)
    {
        if(GXL(X_POS))
        {
            SYL(Y_POS, 0);  
          
            SML(M_HOME_FINISH, 1);
            SML(M_PUSH2_CHECK, 0);
            
            project.platformPos = 0;
            
            project.platformState = 0;
            
            UART1_printf("Action----Push2Home----Finish\r\n");
        }
    }
}
