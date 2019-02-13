#include "management.h"
#include "misc.h"
#include "project.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"
#include "timer.h"

#include "simplc_io.h"
#include "simplc_timer.h"



//编写自编程序
void ProgramSelf(void)
{
    if(GMR(M_KEY_FAST))//加步骤
    {
        man.programSelfPos++;
        if(man.programSelfPos >= MAX_ACTIONS_PER_PROJECT-1)
            man.programSelfPos = 0;
        
        man.pCurProSelfAction = man.program + man.programSelfPos;
        
        sprintf((char*)man.segStr, "%02d-", (int)(man.programSelfPos));
        TM1638_SendData(0, man.segStr);
        
        sprintf((char*)man.segStr, "%02d", (int)(man.pCurProSelfAction->act));
        TM1638_SendData(3, man.segStr);
    }
    else if(GMR(M_KEY_SLOW))//减步骤
    {
        man.programSelfPos--;
        if(man.programSelfPos >= 255)
            man.programSelfPos = MAX_ACTIONS_PER_PROJECT-1;
        
        man.pCurProSelfAction = man.program + man.programSelfPos;
        
        sprintf((char*)man.segStr, "%02d-", (int)(man.programSelfPos));
        TM1638_SendData(0, man.segStr);
        
        sprintf((char*)man.segStr, "%02d", (int)(man.pCurProSelfAction->act));
        TM1638_SendData(3, man.segStr);
    }
    
    //--------------------------------------------------------------------------------------
    if(GMR(M_KEY_UPDOWN))//上下
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);
        
        if(man.headPos&FRONTBACK_MASK)
        {
            if(man.headPos&UPDOWN_MASK)
            {

                man.headPos &= ~UPDOWN_MASK;          

                man.pCurProSelfAction->act = PRINT_UP;
            }
            else
            {
                man.headPos |= UPDOWN_MASK; 

                man.pCurProSelfAction->act = PRINT_DOWN;
            }
        }
        else
        {
            
            man.pCurProSelfAction->act = ABSORB;
        }
        
        man.programSelfPos++;
        if(man.programSelfPos >= MAX_ACTIONS_PER_PROJECT-1)
            man.programSelfPos = 0;
        
        man.pCurProSelfAction = man.program + man.programSelfPos;
        
        sprintf((char*)man.segStr, "%02d-", (int)(man.programSelfPos));
        TM1638_SendData(0, man.segStr);
        
        sprintf((char*)man.segStr, "%02d", (int)(man.pCurProSelfAction->act));
        TM1638_SendData(3, man.segStr);
    }
    else if(GMR(M_KEY_FRONTBACK))//前后
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);   
      
        if(man.headPos&UPDOWN_MASK)
        {
            ;
        }
        else
        {
            if(man.headPos&FRONTBACK_MASK)
            {

                man.headPos &= ~FRONTBACK_MASK;
            
                man.pCurProSelfAction->act = BACK;
            }
            else
            {
                man.headPos |= FRONTBACK_MASK;

                man.pCurProSelfAction->act = FRONT;
            }
        }
        
        man.programSelfPos++;
        if(man.programSelfPos >= MAX_ACTIONS_PER_PROJECT-1)
            man.programSelfPos = 0;
        
        man.pCurProSelfAction = man.program + man.programSelfPos;
        
        sprintf((char*)man.segStr, "%02d-", (int)(man.programSelfPos));
        TM1638_SendData(0, man.segStr);
        
        sprintf((char*)man.segStr, "%02d", (int)(man.pCurProSelfAction->act));
        TM1638_SendData(3, man.segStr);
    }
    
    if(GMR(M_KEY_AUX))//穿梭
    {
        SML(M_PROGRAM_SELF_CHANGE, 1);

        if(man.programNum == 3 || man.programNum == 4)
            man.actPlatform = SHIFTRETURN;
        else if(man.programNum == 10)
            man.actPlatform = SHIFTRETURN2;
        else
            man.pCurProSelfAction->act = SHIFT;
        
        man.programSelfPos++;
        if(man.programSelfPos >= MAX_ACTIONS_PER_PROJECT-1)
            man.programSelfPos = 0;
        
        man.pCurProSelfAction = man.program + man.programSelfPos;
        
        sprintf((char*)man.segStr, "%02d-", (int)(man.programSelfPos));
        TM1638_SendData(0, man.segStr);
        
        sprintf((char*)man.segStr, "%02d", (int)(man.pCurProSelfAction->act));
        TM1638_SendData(3, man.segStr);
    }
    
    if(GMR(M_KEY_STARTSTOP))//保存并结束
    {
        if(GML(M_PROGRAM_SELF_CHANGE))
        {
            printf("Saving the Program0...\r\n");
            
//            man.pCurProSelfAction->act = HOME;
//            man.programSelfPos++;
//            man.pCurProSelfAction = man.program + man.programSelfPos;
            man.pCurProSelfAction->act = ACTION_NONE;
            
            Program0Save();
            
            //
            man.programNum = 0;
            AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, man.programNum);
        }
        
        printf("Saved the Program0 finish!\r\n");
       
        while(1);
    }//if(GMR(M_KEY_STARTSTOP)) 
}
