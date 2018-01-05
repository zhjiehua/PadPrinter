
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


//����ˢ��
void Key_Refresh(void)
{
    uint8_t key = TM1638_ReadKey(0);
    uint8_t i;

    for(i=0;i<8;i++)
        SML(i, 0);

    if(key)
        SML(TM1638_ReadKey(0)-1, 1);
}

//��д�Ա����
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
  
    if(GMR(M_KEY_FAST))//�Ӳ���
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
    else if(GMR(M_KEY_SLOW))//������
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
    
    if(GMR(M_KEY_UPDOWN))//�ӳ���
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);  
      
        project.pCurProSelfAction->act++;
        if(project.pCurProSelfAction->act > HOME)//12
            project.pCurProSelfAction->act = ACTION_NONE;//0
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    else if(GMR(M_KEY_FRONTBACK))//������
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);   
      
        project.pCurProSelfAction->act--;
        if(project.pCurProSelfAction->act >= 255)//12
            project.pCurProSelfAction->act = HOME;
        
        sprintf((char*)project.segStr, "%2d", project.pCurProSelfAction->act);
        TM1638_SendData(3, project.segStr);
    }
    
    if(GMR(M_KEY_AUX))//�޸�flag
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

//ѡ�����
void ProgramSelect(void)
{
    if(GMR(M_PROGRAM_SEL))//ˢ��һ���ϴιػ�ǰ�ĳ������
    {
        UART1_printf("Selecting Program...\r\n");
        
        //��ȡ�������
        project.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);
        
        //ˢ�������
        sprintf((char*)project.segStr, "Pro%2d", project.programNum);
        TM1638_SendData(0, project.segStr);
    }
    
    if(GMR(M_KEY_FAST))//�ӳ���
    {
        if(!GML(M_INTSET))
        {
            if(project.programNum >= PROJECTNUM-1)
                project.programNum = 0;
            else
                project.programNum++;
            
            //ˢ�������
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
    else if(GMR(M_KEY_SLOW))//������
    {
        if(!GML(M_INTSET))
        {
            if(project.programNum <= 0)
                project.programNum = PROJECTNUM-1;
            else
                project.programNum--;
            
            //ˢ�������
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
    else if(GMR(M_KEY_STARTSTOP))//ѡ�����򣬸���project�ṹ��
    {
        if(!GML(M_INTSET))
        {
            //���������ŵ�EEPROM
            AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, project.programNum);
            
            UART1_printf("The selected Program is %s\r\n", project.segStr);
            
            //ѡ������˳�ѡ�����
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

//�������˸
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
            
            //��˸�����ʾ����
            sprintf((char*)project.segStr, "%5d", project.productOutput);
            TM1638_SendData(0, project.segStr);
        }
    }
}

//����������İ���ʶ��
void KeyDistinguish(void)
{
    if(GML(M_MODE_AUTO))//����ִ���Զ�����
    {
        if(GMR(M_KEY_STARTSTOP))
        {
            SML(M_CHANGEMODE_PAUSE, 1);
            
            UART1_printf("Stop the program.\r\n");
            
            SML(M_PROGRAM_FOOT, 1);//��ʾ�������Ѱ�������
        }
        if(GMR(M_KEY_AUX))//����
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
        if(GMR(M_KEY_FOOT))//��̤
        {
            SML(M_CHANGEMODE_PAUSE, 1);
            
            UART1_printf("Foot---Stop the program.\r\n");
        }
    }
    else// if(project.programMode == MANUAL)
    {
        if(GMR(M_KEY_STARTSTOP))//����ֹͣ
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
                    SML(M_MODE_AUTO, 1);//�����Զ�����
                    
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
            
            SML(M_PROGRAM_FOOT, 1);//��ʾ�������Ѱ�������
        }
        if(GMR(M_KEY_AUX))//����
        {
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            SML(M_MAN_AUX, 1);
        }
        if(GMR(M_KEY_UPDOWN))//����
        {
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            SML(M_MAN_UPDOWN, 1);
        }
        if(GMR(M_KEY_FRONTBACK))//ǰ��
        {
            SML(M_MODE_MANUAL, 1);
            
            SML(M_FLAG_MANUAL, 1);
            
            SML(M_MAN_FRONTBACK, 1);
        }
        if(GMR(M_KEY_FOOT))//��̤
        {
            if(!GML(M_PROGRAM_FOOT))
            {
                SML(M_PROGRAM_PERIOD, 1);
            }
            
            SML(M_MODE_AUTO, 1);
        }
        if(GMR(M_KEY_FAST))//��
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
        if(GMR(M_KEY_SLOW))//��
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

//������һ������
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

//������
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
            if(GXL(X_ABSORB_L))//������λ
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
            if(GXL(X_ABSORB_O))//������λ
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

//����
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
            if(GXL(X_ABSORB_L))//������λ
            {
                SYL(Y_UPDOWN, 0);
                project.printHeadState = 2;
            }
        break;
        case 2:
            if(GXL(X_ABSORB_O))//������λ
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
            if(GXL(X_PRINT_O))//������λ
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
            if(GXL(X_ABSORB_O))//������λ
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
            if(GML(M_HOME_FINISH))//�ȴ���ԭ�㵽λ
            {
                SYL(Y_UPDOWN, 1);
                project.printHeadState = 1;
                
                UART1_printf("Action----Print Down\r\n");
            }
        break;
        case 1:
            if(GXL(X_PRINT_L))//������λ
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
            if(GXL(X_PRINT_O))//������λ
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

//����
void Print(void)
{
    switch(project.printHeadState)
    {
        case 0:
            if(GML(M_HOME_FINISH))//�ȴ���ԭ�㵽λ
            {
                SYL(Y_UPDOWN, 1);
                project.printHeadState = 1;
                
                UART1_printf("Action----Print\r\n");
            }
            else
                UART1_printf("Action----Print----Waiting for Home Finish\r\n");
        break;
        case 1:
            if(GXL(X_PRINT_L))//������λ
            {
                SYL(Y_UPDOWN, 0);
                project.printHeadState = 2;
            }
        break;
        case 2:
            if(GXL(X_PRINT_O))//������λ
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
            if(GXL(X_SHIFT_L1) && GXL(X_SHIFT_L2))//��ǰ���ѵ����޵㣬�����˻ص�ԭ��
            {
                if(project.platformPos == 0)//ֻ��һ��λ�ã��������˲���
                {
                    UART1_printf("Action----Shift----Finish----%d\r\n", project.platformPos);
                    
                    if(GML(M_MODE_AUTO))
                        NextAction();
                    else if(GML(M_MODE_MANUAL))
                        SML(M_MAN_AUX, 0);
                }
                else//�˻ص�λ��0
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
                if(project.platformPos == 0)//λ��0ֱ����λ
                {
                    SYL(Y_POS, 1);
                    SYL(Y_SHIFT, 1);
                    
                    UART1_printf("Action----Shift0----%d\r\n", project.platformPos);
                    
                    project.platformState = 3;
                }
                else//�Ȼ���һ��
                {
                    SYL(Y_SHIFT, 0);//����һ��
                    
                    project.platformState = 1;
                    
                    UART1_printf("Action----Shift++++%d\r\n", project.platformPos);
                }
            }
        break;
        case 1:
            if(GXF(X_SHIFT_L2))//���˵�λ
            {
                SYL(Y_POS, 0);
                SYL(Y_SHIFT, 1);
                
                //UART1_printf("Action----Shift----%d\r\n", project.platformPos);
                
                project.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_POS))//������λ����
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
        case 0://�Ȼ�λһ��
            SYL(Y_SHIFT, 0);
            
            project.platformState = 1;
            
            UART1_printf("Action----Return\r\n");
        break;
        case 1:
            if(GXF(X_SHIFT_L2))
            {
                SYL(Y_POS, 0);//�ٽ���λ�����ջ���
                
                project.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_SHIFT_O))//ȷ����λ
            {
                UART1_printf("Action----Return----Finish----%d\r\n", project.platformPos);
              
                project.platformPos = 0;
                
                //���¶���
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
                    case 0://�Ȼ�λһ��
                        SYL(Y_SHIFT, 0);
                        
                        SML(M_HOME_FINISH, 0);
                        
                        UART1_printf("Action----Home Return\r\n");
                        
                        SML(M_HOME_CHECK, 1);//��ʼ����Ƿ��ԭ�㵽λ
                        
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
            //                SYL(Y_POS, 0);//�ٽ���λ�����ջ���
            //                
            //                project.platformState = 0;
            //                
            //                SML(M_HOME_CHECK, 1);//��ʼ����Ƿ��ԭ�㵽λ
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