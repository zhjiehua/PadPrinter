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
              
                project.printHeadPos |= UPDOWN_MASK;
                
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
              
                project.printHeadPos &= ~UPDOWN_MASK;
                
                if(GML(M_MODE_AUTO))
                {
                    if(project.pCurAction->act != HOME)
                        NextAction();
                }
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                else if(GML(M_MODE_RESTORE))
                    SML(M_RESTORE_UP, 0);
                
                
                
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
              
                project.printHeadPos &= ~UPDOWN_MASK;
                
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
              
                project.printHeadPos |= FRONTBACK_MASK;
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_FRONTBACK, 0);
                
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
                
                project.printHeadPos &= ~FRONTBACK_MASK;
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_FRONTBACK, 0);
                else if(GML(M_MODE_RESTORE))
                    SML(M_RESTORE_HOME, 0);

                project.printHeadState = 0;
                
                SML(M_HOME_RETURN, 0);
                
                SML(M_PUSH_RETURN, 0);
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
              
                project.printHeadPos |= UPDOWN_MASK;
                
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
              
                project.printHeadPos &= ~UPDOWN_MASK;
                
                if(GML(M_MODE_AUTO))
                {
                    if(project.pCurAction->act != HOME)
                        NextAction();
                }
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_UPDOWN, 0);
                else if(GML(M_MODE_RESTORE))
                    SML(M_RESTORE_UP, 0);
                
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
//            else
//                UART1_printf("Action----Print----Waiting for Home Finish\r\n");
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
                    {
                        project.pCurAction->act = RETURN; //*(project.pCurAction)).act
                    }
                    else if(GML(M_MODE_MANUAL))
                        SML(M_MAN_AUX_RETURN, 1);
                    
                    UART1_printf("Action----Shift++++Return\r\n");
                }
            }
            else//未到极限点
            {                      
                if(project.platformPos == 0)//位置0直接移位
                {
                    //if(GML(X_SHIFT_O))
                    {
                        SYL(Y_POS, 1);
                        SYL(Y_SHIFT, 1);
                        
                        UART1_printf("Action----Shift0----%d\r\n", project.platformPos);
                        
                        project.platformState = 3;
                    }
                }
                else//先回退一点
                {
                    //if(GML(X_SHIFT_L1))
                    {
                        SYL(Y_SHIFT, 0);//回退一点
                        
                        project.platformState = 1;
                        
                        UART1_printf("Action----Shift++++%d\r\n", project.platformPos);
                    }
//                    else
//                        UART1_printf("Action----Shift++++Waitinf for X_SHIFT_L1\r\n");
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
                project.platformPos++;
                
                UART1_printf("Action----Shift----Finish----%d\r\n", project.platformPos);
                
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
    uint8_t originSensor;
    switch(project.platformState)
    {
        case 0://先回位一点
            //if(GML(X_SHIFT_L1))
            {
                SYL(Y_SHIFT, 0);
                
                project.platformState = 1;
                
                SML(M_HOME_FINISH, 0);
                
                UART1_printf("Action----Return\r\n");
            }
        break;
        case 1:
            if(project.machineType == MACHINE_2SENSORS)
            {
                project.platformState = 2;
            }
            else
            {
                if(!GXL(X_SHIFT_L2))
                {
                    SYL(Y_POS, 0);//再将定位气缸收回来
                    
                    project.platformState = 2;
                }
            }
        break;
        case 2:
            if(project.machineType == MACHINE_2SENSORS)
                originSensor = X_POS;
            else
                originSensor = X_SHIFT_O;
            
            if(GXL(originSensor))
            //if(GXL(X_SHIFT_O))//确定到位
            //if(GXL(X_POS))
            {
                project.platformPos = 0;
                
                project.platformPushFlag = 0;
                
                UART1_printf("Action----Return----Finish----%d\r\n", project.platformPos);
                
                //更新动作
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                {
                    SML(M_MAN_AUX, 0);
                    SML(M_MAN_AUX_RETURN, 0);
                }
                
                project.platformState = 0;
                
                SML(M_HOME_FINISH, 1);
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
            //if(GXL(X_POS))
            {
                SYL(Y_SHIFT, 1);
                SYL(Y_POS, 1);
                
                project.platformState = 1;
                
                UART1_printf("Action----Push\r\n");
            }
//            else
//                UART1_printf("Action----Push++++++++++\r\n");
        break;
        case 1:
            if(GXL(X_SHIFT_L1))
            {
                project.platformPos = 1;
              
                project.platformState = 0;
                
                project.platformPushFlag = 1;
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_AUX, 0);
                
                UART1_printf("Action----Push----Finish\r\n");
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
                        SML(M_HOME_CHECK, 1);//开始检查是否回原点到位
                        
                        UART1_printf("Action----Home Return\r\n");
                        
                        project.platformState = 1;
                        
                    break;
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

void PushHome(void)
{
    switch(project.platformState)
    {
        case 0:
            //if(GXL(X_POS))
            {
                SYL(Y_SHIFT, 1);
                SYL(Y_POS, 1);
                
                UART1_printf("Action----PushHome\r\n");
                
                SML(M_PUSH_CHECK, 1);
                SML(M_HOME_FINISH, 0);
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_AUX, 0);
                
                project.platformState = 1;
            }
        break;
        default:
        break;
    }
}

void Push2Home(void)
{
    switch(project.platformState)
    {
        case 0:
            //if(GXL(X_POS))
            {
                SYL(Y_SHIFT, 1);
                SYL(Y_POS, 1);
                
                UART1_printf("Action----Push2Home\r\n");
                
                SML(M_PUSH2_CHECK, 1);
                SML(M_HOME_FINISH, 0);
                
                if(GML(M_MODE_AUTO))
                    NextAction();
                else if(GML(M_MODE_MANUAL))
                    SML(M_MAN_AUX, 0);
                
                project.platformState = 1;
            }
        break;
        default:
        break;
    }
}
