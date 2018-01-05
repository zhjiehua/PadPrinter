
#include "management.h"
#include "project.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include <stm8s.h>
#include "io.h"
#include "uart1.h"
#include "tm1638.h"
#include "flash_eeprom.h"
#include "24cxx.h"
#include "tim5.h"


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
        
        sprintf((char*)project.segStr, "%2d", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
        
        
        if(project.pCurProSelfAction->flag)
            TM1638_SendData(2, "-");
        else
            TM1638_SendData(2, " ");
    }
  
    if(GMR(M_KEY_FAST))//加步骤
    {
        project.programSelfPos++;
        if(project.programSelfPos >= MAX_ACTIONS_PER_PROJECT-1)
            project.programSelfPos = 0;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%2d", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
        
        if(project.pCurProSelfAction->flag)
            TM1638_SendData(2, "-");
        else
            TM1638_SendData(2, " ");
    }
    else if(GMR(M_KEY_SLOW))//减步骤
    {
        project.programSelfPos--;
        if(project.programSelfPos >= 255)
            project.programSelfPos = MAX_ACTIONS_PER_PROJECT-1;
        
        project.pCurProSelfAction = project.program + project.programSelfPos;
        
        sprintf((char*)project.segStr, "%2d", project.programSelfPos);
        TM1638_SendData(0, project.segStr);
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
        
        if(project.pCurProSelfAction->flag)
            TM1638_SendData(2, "-");
        else
            TM1638_SendData(2, " ");
    }
    
    if(GMR(M_KEY_UPDOWN))//加程序
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);  
      
        project.pCurProSelfAction->act++;
        if(project.pCurProSelfAction->act > HOME)//12
            project.pCurProSelfAction->act = ACTION_NONE;//0
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    else if(GMR(M_KEY_FRONTBACK))//减程序
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);   
      
        project.pCurProSelfAction->act--;
        if(project.pCurProSelfAction->act >= 255)//12
            project.pCurProSelfAction->act = HOME;
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    
    if(GMR(M_KEY_AUX))//修改flag
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);
        
        if(!project.pCurProSelfAction->flag)//12
        {
            project.pCurProSelfAction->flag = 1;
            TM1638_SendData(2, "-");
        }
        else
        {
            project.pCurProSelfAction->flag = 0;
            TM1638_SendData(2, " ");
        }
    }
    
    if(GMR(M_KEY_STARTSTOP))
    {
        if(GML(M_PROGRAM_SELF_CHANGE))
            Program0Save();
        
        project.mode = NORMAL;
        
        SML(M_SEG_FLASH, 1);
    }
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
        sprintf((char*)project.segStr, "Pro%2d", project.programNum);
        TM1638_SendData(0, project.segStr);
    }
    
    if(GMR(M_KEY_FAST))//加程序
    {
        if(!GML(M_INTSET))
        {
            if(project.programNum >= PROJECTNUM-1)
                project.programNum = 0;
            else
                project.programNum++;
            
            //刷新数码管
            sprintf((char*)project.segStr, "Pro%2d", project.programNum);
            TM1638_SendData(0, project.segStr);
        }
        else
        {
            project.intSetPos++;
            if(project.intSetPos > 10)
                project.intSetPos = 0;
            
            sprintf((char*)project.segStr, "%2d-", project.intSetPos);
            TM1638_SendData(0, project.segStr);
        }
    }
    else if(GMR(M_KEY_SLOW))//减程序
    {
        if(!GML(M_INTSET))
        {
            if(project.programNum <= 0)
                project.programNum = PROJECTNUM-1;
            else
                project.programNum--;
            
            //刷新数码管
            sprintf((char*)project.segStr, "Pro%2d", project.programNum);
            TM1638_SendData(0, project.segStr);
        }
        else
        {
            project.intSetPos--;
            if(project.intSetPos >= 255)
                project.intSetPos = 10;
            
            sprintf((char*)project.segStr, "%2d-", project.intSetPos);
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
            if(project.programNum == 0)
            {
                project.mode = PROGRAM_SELF;
                SML(M_PROGRAM_SELF, 1);
            }
            else
            {
                project.mode = NORMAL;
                SML(M_PROGRAM_READ, 1);
            }
        }
        else
        {
            SML(M_INTSET, 0);
            
            switch(project.intSetPos)
            {
                case 0:
                    if(project.intSetVal)
                    {
                        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0xAA);
                        UART1_printf("Reset default data!\r\n");
                    }
                    else
                        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
                break;
                case 1:
                    if(project.intSetVal > 0)
                    {
                        AT24CXX_WriteLenByte(EEPROM_ADDR_ACTIONUNITDELAY, 40*project.intSetVal, 2);
                        UART1_printf("Change the actionUnitDelay is %d!\r\n", 40*project.intSetVal);
                    }
                    else
                    {
                        AT24CXX_WriteLenByte(EEPROM_ADDR_ACTIONUNITDELAY, 1, 2);
                        UART1_printf("Change the actionUnitDelay is 1!\r\n");
                    }
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
                sprintf((char*)project.segStr, "%2d-%2d", project.intSetPos, project.intSetVal);
                TM1638_SendData(0, project.segStr);
                
                SML(M_INTSET, 1);
            }
        }
    }
    else if(GML(M_INTSET))
    {
        if(GMR(M_KEY_UPDOWN))
        {
            project.intSetVal++;
            if(project.intSetVal > 10)
                project.intSetVal = 0;
            
            sprintf((char*)project.segStr, "%2d", project.intSetVal);
            TM1638_SendData(3, project.segStr);
        }
        else if(GMR(M_KEY_FRONTBACK))
        {
            project.intSetVal--;
            if(project.intSetVal >= 255)
                project.intSetVal = 10;
            
            sprintf((char*)project.segStr, "%2d", project.intSetVal);
            TM1638_SendData(3, project.segStr);
        }
    }
}

//数码管闪烁
void SegFlash(void)
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
            
            //闪烁完后，显示产量
            sprintf((char*)project.segStr, "%5d", project.productOutput);
            TM1638_SendData(0, project.segStr);
        }
    }
}

//正常启动后的按键识别
void KeyDistinguish(void)
{
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
            if(!GML(M_PROGRAM_AUX))
            {
                SML(M_PROGRAM_AUX, 1);
                
                //SML(M_MAN_FLAG_AUX, 0);
                
                SML(M_AUTO_FLAG_AUX, 1);
                
                UART1_printf("Add the auxiliary action to current program.\r\n");
            }
//            else
//            {
//                SML(M_PROGRAM_AUX, 0);
//                
//                UART1_printf("Del the auxiliary action from current program.\r\n");
//            }
        }
        if(GMR(M_KEY_FOOT))//脚踏
        {
            SML(M_CHANGEMODE_PAUSE, 1);
            
            UART1_printf("Foot---Stop the program.\r\n");
        }
    }
    else// if(project.programMode == MANUAL)
    {
        if(GMR(M_KEY_STARTSTOP))//启动停止
        {
            if(!GML(M_ADJ_DELAY))
            {
                if(GML(M_FLAG_MANUAL))
                {
                    SML(M_FLAG_MANUAL, 0);
                    
                    UART1_printf("\r\nRestore the initial status.\r\n");
                    
                    SML(M_MODE_RESTORE, 1);
                    
                    //SML(M_PROGRAM_AUX, 0);
                    
                    if(project.printHeadPos&UPDOWN_MASK)
                        SML(M_RESTORE_UP, 1);
                    
                    SML(M_RESTORE_HOME, 1);
                }
                else
                {
                    SML(M_MODE_AUTO, 1);//启动自动程序
                    
                    UART1_printf("Start the program...\r\n");
                }
                
                
                //
            }
            else
            {
                sprintf((char*)project.segStr, "%5d", project.productOutput);
                TM1638_SendData(0, project.segStr);
                
                AT24CXX_WriteOneByte(project.programAddr+project.actionPos*3+2, project.pCurAction->delay);
                AT24CXX_WriteOneByte(project.programAddr+project.actionPos*3+2, project.pCurAction->delay);
                AT24CXX_WriteOneByte(project.programAddr+project.actionPos*3+2, project.pCurAction->delay);
                
                SML(M_ADJ_DELAY, 0);
            }
            
            SML(M_PROGRAM_FOOT, 1);//表示开机后已按过启动
        }
        if(GMR(M_KEY_AUX))//辅助
        {
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            SML(M_MAN_AUX, 1);
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
        if(GMR(M_KEY_FAST))//快
        {
            SML(M_ADJ_DELAY, 1);
            
            project.pCurAction->delay++;
            if(project.pCurAction->delay > MAX_ACTIONUNIT_DELAY)
                project.pCurAction->delay = 0;
            
            project.delay = project.pCurAction->delay;
            
            UART1_printf("Add delay time.\r\n");
            
            sprintf((char*)project.segStr, "%5d", project.pCurAction->delay);
            TM1638_SendData(0, project.segStr);
        }
        if(GMR(M_KEY_SLOW))//慢
        {
            SML(M_ADJ_DELAY, 1);
            
            project.pCurAction->delay--;
            if(project.pCurAction->delay >= 255)
                project.pCurAction->delay = MAX_ACTIONUNIT_DELAY;
            
            project.delay = project.pCurAction->delay;
            
            UART1_printf("Dec delay time.\r\n");
            
            sprintf((char*)project.segStr, "%5d", project.pCurAction->delay);
            TM1638_SendData(0, project.segStr);
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
                project.actionPos = 0;
                
                project.productOutput++;
                sprintf((char*)project.segStr, "%5d", project.productOutput);
                TM1638_SendData(0, project.segStr);
                
                AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, project.productOutput, 2);
                
                if(GML(M_PROGRAM_PERIOD))
                {
                    SML(M_MODE_AUTO, 0);
                    SML(M_PROGRAM_PERIOD, 0);
                }
                
                UART1_printf("\r\n");
            }
            
            project.pCurAction = project.program + project.actionPos;
        }while(!project.pCurAction->flag);
    }
    else
    {
        project.actionPos++;
        if(project.actionPos >= project.actionCnt)
        {
            project.actionPos = 0;
            
            project.productOutput++;
            sprintf((char*)project.segStr, "%5d", project.productOutput);
            TM1638_SendData(0, project.segStr);
            
            AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, project.productOutput, 2);
            
            if(GML(M_PROGRAM_PERIOD))
            {
                SML(M_MODE_AUTO, 0);
                SML(M_PROGRAM_PERIOD, 0);
            }
            
            UART1_printf("\r\n");
        }
        
        project.pCurAction = project.program + project.actionPos;
    }
    
    if(GML(M_CHANGEMODE_PAUSE))
    {
        SML(M_MODE_AUTO, 0);
        SML(M_CHANGEMODE_PAUSE, 0);
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

//吸油下
void AbsorbDown(void)
{
    switch(project.printHeadState)
    {
        case 0:
            SYL(Y_UPDOWN, 1);
            project.printHeadState = 1;
            
            UART1_printf("Action----Absorb Down\r\n");
        break;
        case 1:
            if(GXL(X_ABSORB_L))//动作到位
            {
                UART1_printf("Action----Absorb Down----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                
                project.printHeadPos |= UPDOWN_MASK;
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

void AbsorbUp(void)
{
    switch(project.printHeadState)
    {
        case 0:
            SYL(Y_UPDOWN, 0);
            project.printHeadState = 1;
            
            UART1_printf("Action----Absorb Up\r\n");
        break;
        case 1:
            if(GXL(X_ABSORB_O))//动作到位
            {
                UART1_printf("Action----Absorb Up----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                {
                    if(project.pCurAction->act != HOME)
                        NextAction();
                }
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                else if(GML(M_MODE_RESTORE))
                    SML(M_RESTORE_UP, 0);
                
                project.printHeadPos &= ~UPDOWN_MASK;
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

//吸油
void Absorb(void)
{
    switch(project.printHeadState)
    {
        case 0:
            SYL(Y_UPDOWN, 1);
            project.printHeadState = 1;
            
            UART1_printf("Action----Absorb\r\n");
        break;
        case 1:
            if(GXL(X_ABSORB_L))//动作到位
            {
                SYL(Y_UPDOWN, 0);
                project.printHeadState = 2;
            }
        break;
        case 2:
            if(GXL(X_ABSORB_O))//动作到位
            {
                UART1_printf("Action----Absorb----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

void Front(void)
{
    switch(project.printHeadState)
    {
        case 0:
            SYL(Y_FRONTBACK, 1);
            project.printHeadState = 1;
            
            UART1_printf("Action----Front\r\n");
        break;
        case 1:
            if(GXL(X_PRINT_O))//动作到位
            {
                UART1_printf("Action----Front----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_FRONTBACK, 0);
                
                project.printHeadPos |= FRONTBACK_MASK;
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

void Back(void)
{
    switch(project.printHeadState)
    {
        case 0:
            SYL(Y_FRONTBACK, 0);
            project.printHeadState = 1;
            
            UART1_printf("Action----Back\r\n");
        break;
        case 1:
            if(GXL(X_ABSORB_O))//动作到位
            {
                UART1_printf("Action----Back----Finish\r\n");
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_FRONTBACK, 0);
                else if(GML(M_MODE_RESTORE))
                    SML(M_RESTORE_HOME, 0);
                
                project.printHeadPos &= ~FRONTBACK_MASK;
                
                project.printHeadState = 0;
                
                SML(M_HOME_RETURN, 0);
            }
        break;
        default:
        break;
    }
}

void PrintDown(void)
{
    switch(project.printHeadState)
    {
        case 0:
            if(GML(M_HOME_FINISH))//等待回原点到位
            {
                SYL(Y_UPDOWN, 1);
                project.printHeadState = 1;
                
                UART1_printf("Action----Print Down\r\n");
            }
        break;
        case 1:
            if(GXL(X_PRINT_L))//动作到位
            {
                UART1_printf("Action----Print Down----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                
                project.printHeadPos |= UPDOWN_MASK;
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

void PrintUp(void)
{
    switch(project.printHeadState)
    {
        case 0:
            SYL(Y_UPDOWN, 0);
            project.printHeadState = 1;
            
            UART1_printf("Action----Print Up\r\n");
        break;
        case 1:
            if(GXL(X_PRINT_O))//动作到位
            {
                UART1_printf("Action----Print Up----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                {
                    if(project.pCurAction->act != HOME)
                        NextAction();
                }
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                else if(GML(M_MODE_RESTORE))
                    SML(M_RESTORE_UP, 0);
                
                project.printHeadPos &= ~UPDOWN_MASK;
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

//吸油
void Print(void)
{
    switch(project.printHeadState)
    {
        case 0:
            if(GML(M_HOME_FINISH))//等待回原点到位
            {
                SYL(Y_UPDOWN, 1);
                project.printHeadState = 1;
                
                UART1_printf("Action----Print\r\n");
            }
            else
                UART1_printf("Action----Print----Waiting for Home Finish\r\n");
        break;
        case 1:
            if(GXL(X_PRINT_L))//动作到位
            {
                SYL(Y_UPDOWN, 0);
                project.printHeadState = 2;
            }
        break;
        case 2:
            if(GXL(X_PRINT_O))//动作到位
            {
                UART1_printf("Action----Print----Finish\r\n");
              
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                
                project.printHeadState = 0;
            }
        break;
        default:
        break;
    }
}

void Shift(void)
{
    switch(project.platformState)
    {
        case 0:
            if(GXL(X_SHIFT_L1) && GXL(X_SHIFT_L2))//当前点已到极限点，则先退回到原点
            {
                if(project.platformPos == 0)//只有一个位置，则跳过此步骤
                {
                    UART1_printf("Action----Shift----Finish----%d\r\n", project.platformPos);
                    
                    if(GML(M_MODE_AUTO))
                        NextAction();
                    else if(GML(M_MODE_MANUAL))
                        SML(M_MAN_AUX, 0);
                }
                else//退回到位置0
                {
                    if(GML(M_MODE_AUTO))
                        project.pCurAction->act = RETURN; //*(project.pCurAction)).act
                    else if(GML(M_MODE_MANUAL))
                        SML(M_MAN_AUX_RETURN, 1);
                    
                    UART1_printf("Action----Shift++++Return\r\n");
                }
            }
            else
            {                      
                if(project.platformPos == 0)//位置0直接移位
                {
                    SYL(Y_POS, 1);
                    SYL(Y_SHIFT, 1);
                    
                    UART1_printf("Action----Shift0----%d\r\n", project.platformPos);
                    
                    project.platformState = 3;
                }
                else//先回退一点
                {
                    SYL(Y_SHIFT, 0);//回退一点
                    
                    project.platformState = 1;
                    
                    UART1_printf("Action----Shift++++%d\r\n", project.platformPos);
                }
            }
        break;
        case 1:
            if(GXF(X_SHIFT_L2))//回退到位
            {
                SYL(Y_POS, 0);
                SYL(Y_SHIFT, 1);
                
                //UART1_printf("Action----Shift----%d\r\n", project.platformPos);
                
                project.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_POS))//启动定位气缸
            {
                SYL(Y_POS, 1);
                
                project.platformState = 3;
            }
        break;
        case 3:
            if(GXL(X_SHIFT_L1))
            {     
                UART1_printf("Action----Shift----Finish----%d\r\n", project.platformPos);
              
                project.platformPos++;
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_AUX, 0);
                
                project.platformState = 0;
            }
        break;
        default:
        break;
    }
}


void Return(void)
{
    switch(project.platformState)
    {
        case 0://先回位一点
            SYL(Y_SHIFT, 0);
            
            project.platformState = 1;
            
            UART1_printf("Action----Return\r\n");
        break;
        case 1:
            if(GXF(X_SHIFT_L2))
            {
                SYL(Y_POS, 0);//再将定位气缸收回来
                
                project.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_SHIFT_O))//确定到位
            {
                UART1_printf("Action----Return----Finish----%d\r\n", project.platformPos);
              
                project.platformPos = 0;
                
                //更新动作
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                {
                    SML(M_MAN_AUX, 0);
                    SML(M_MAN_AUX_RETURN, 0);
                }
                
                project.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void Push(void)
{
    switch(project.platformState)
    {
        case 0://
            SYL(Y_POS, 0);
            SYL(Y_SHIFT, 1);

            project.platformState = 1;
            
            UART1_printf("Action----Push\r\n");
        break;
        case 1:
            if(GXF(X_POS))
            //if(GXR(X_SHIFT_O))
            {
                SYL(Y_SHIFT, 0);
                
                project.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_SHIFT_O))
            {
                UART1_printf("Action----Push----Finish\r\n");
              
                SML(M_PUSH_RETURN, 0);
                SML(M_PUSH, 0);
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                
                project.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void Home(void)
{
    if(GML(M_PROGRAM_AUX) || project.platformHomeFlag)//
    {
        if(project.platformPos != 0)
        {
            //RETURN
            if(!GML(M_HOME_RETURN))
            {
                SML(M_HOME_RETURN, 1);
                switch(project.platformState)
                {
                    case 0://先回位一点
                        SYL(Y_SHIFT, 0);
                        
                        SML(M_HOME_FINISH, 0);
                        
                        UART1_printf("Action----Home Return\r\n");
                        
                        SML(M_HOME_CHECK, 1);//开始检查是否回原点到位
                        
        //                if(project.platformPos != 0)
        //                {
                            project.platformState = 1;
        //                }
        //                else
        //                {
        //                    project.platformState = 2;
        //                    SYL(Y_POS, 0);
        //                }
                    break;
            //        case 1:
            //            if(GXF(X_SHIFT_L2))
            //            {
            //                SYL(Y_POS, 0);//再将定位气缸收回来
            //                
            //                project.platformState = 0;
            //                
            //                SML(M_HOME_CHECK, 1);//开始检查是否回原点到位
            //            }
            //        break;
            
                    default:
                    break;
                }
            }
        }
    }
    
    if(project.printHeadPos&UPDOWN_MASK)
    {
        if(project.printHeadPos&FRONTBACK_MASK)
            PrintUp();
        else
            AbsorbUp();
    }
    else
        Back();
}

void PrintHeadHome(void)
{
    if(project.printHeadPos&UPDOWN_MASK)
    {
        if(project.printHeadPos&FRONTBACK_MASK)
            PrintUp();
        else
            AbsorbUp();
    }
    else
        Back();
}