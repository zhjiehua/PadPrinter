#include "management.h"
#include "misc.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"

#include "simplc_io.h"
#include "simplc_timer.h"

 
//正常启动后的按键识别
void Key_Normal(void)
{
    /*调节延时*/
    if(TG(1))
    {
        if(GML(M_KEY_SLOW))//快  M_KEY_SLOW  M_KEY_FAST
        {
            SML(M_OUTPUT_FLASH_FLAG, 0);
            SML(M_SAVE_DELAY, 1);

            if(!(man.delayHeadPos)) //在后面
            {
                man.delayAbsorb++;
                if(man.delayAbsorb > MAX_ACTIONUNIT_DELAY)
                    man.delayAbsorb = MAX_ACTIONUNIT_DELAY;
                
                sprintf((char*)man.segStr, "%5d", (int)(man.delayAbsorb)); 
            }
            else
            {
                man.delayPrint[man.delayPlatformPos]++;
                if(man.delayPrint[man.delayPlatformPos] > MAX_ACTIONUNIT_DELAY)
                    man.delayPrint[man.delayPlatformPos] = MAX_ACTIONUNIT_DELAY;

                sprintf((char*)man.segStr, "%5d", (int)(man.delayPrint[man.delayPlatformPos]));
            }
            TM1638_SendData(0, man.segStr);

            SML(M_CHANGEDELAY, 1);

        }
        else if(GML(M_KEY_FAST))//慢
        {
            SML(M_OUTPUT_FLASH_FLAG, 0);
            SML(M_SAVE_DELAY, 1);

            if(!(man.delayHeadPos)) //在后面
            {
                man.delayAbsorb--;
                if(man.delayAbsorb == 0 || man.delayAbsorb >= MAX_ACTIONUNIT_DELAY)
                    man.delayAbsorb = 0;
                    
                sprintf((char*)man.segStr, "%5d", (int)(man.delayAbsorb)); 
            }
            else
            {
                man.delayPrint[man.delayPlatformPos]--;
                if(man.delayPrint[man.delayPlatformPos] == 0 || man.delayPrint[man.delayPlatformPos] >= MAX_ACTIONUNIT_DELAY)
                    man.delayPrint[man.delayPlatformPos] = 0;

                sprintf((char*)man.segStr, "%5d", (int)(man.delayPrint[man.delayPlatformPos]));
            }

            TM1638_SendData(0, man.segStr);

            SML(M_CHANGEDELAY, 1);
        }
        
        TS(1, DELAY_KEY_FILTER_PERIOD);

        if(!GML(M_KEY_FAST) && !GML(M_KEY_SLOW))
        {
            SML(M_OUTPUT_FLASH_FLAG, 1);
            //TS(3, OUTPUT_FLASH_PERIOD);

            //printf("GMF(M_KEY_FAST) || GMF(M_KEY_SLOW)\r\n");

            SML(M_CHANGEDELAY, 0);
        }
    }
            
    //记录动作按键
    if(GMR(M_KEY_STARTSTOP))
        man.keyPress = KEY_STARTSTOP;
    else if(GMR(M_KEY_AUX))
    {
        if(man.keyPress == KEY_STARTSTOP && !GML(M_MODE_AUTO))
        {
            if(!GML(M_AUTO_FLAG_AUX))
            {
                SML(M_AUTO_FLAG_AUX, 1);
            }    
        }
        else
            man.keyPress = KEY_AUX;
    }    
    else if(GMR(M_KEY_UPDOWN))
        man.keyPress = KEY_UPDOWN;
    else if(GMR(M_KEY_FRONTBACK))
        man.keyPress = KEY_FRONTBACK;
    else if(GMR(M_KEY_FOOT))
        man.keyPress = KEY_FOOT;  
    else if(!GML(M_MODE_AUTO) && GML(M_KEY_UPDOWN) && (GML(M_ABSORB_FINISH) || GML(M_PRINT_FINISH)))
        man.keyPress = KEY_UPDOWN;

    if(GML(M_ABSORBDOWN_FINISH) && GML(M_ABSORBUP_FINISH) && GML(M_ABSORB_FINISH)
            && GML(M_PRINTDOWN_FINISH) && GML(M_PRINTUP_FINISH) && GML(M_PRINT_FINISH)
            && GML(M_FRONT_FINISH) && GML(M_BACK_FINISH))
    {
        SML(M_ACTIONHEAD_FINISH, 1);
        man.actHead = ACTION_NONE;

        //printf("---------M_ACTIONHEAD_FINISH\r\n");
    }
    else
        SML(M_ACTIONHEAD_FINISH, 0);
        
    if(GML(M_SHIFT_FINISH) && GML(M_RETURN_FINISH) && GML(M_SHIFTRETURN_FINISH) && GML(M_SHIFTRETURN2_FINISH))
    {
        SML(M_ACTIONPLATFORM_FINISH, 1);
        man.actPlatform = ACTION_NONE;

        //printf("---++----M_ACTIONPLATFORM_FINISH\r\n");
    }
    else
        SML(M_ACTIONPLATFORM_FINISH, 0);  

    if(GML(M_ACTIONHEAD_FINISH) && GML(M_ACTIONPLATFORM_FINISH))
    {
        man.keyPressLast = KEY_NONE;

        SML(M_ACTION_FINISH, 1);

        //正在修改延时，暂停更新下一动作
//        if(GML(M_CHANGEDELAY))
//            return;
    }
    else
        SML(M_ACTION_FINISH, 0);

 
    if((!GML(M_ACTIONHEAD_FINISH) || !GML(M_ACTIONPLATFORM_FINISH))
        && man.keyPressLast == man.keyPress  && man.keyPress != KEY_NONE//正在动作时按了相同的键
        && (man.keyPress != KEY_STARTSTOP && man.keyPress != KEY_FRONTBACK && man.keyPress != KEY_FOOT))//这3个键可重复按2次
    {
        man.keyPress = KEY_NONE;
        //printf("man.keyPress = KEY_NONE\r\n");
    } 

//    if(man.keyPressLast == man.keyPress && man.keyPressLast != KEY_NONE)
//        printf("man.keyPressLast == man.keyPress\r\n");
    
    //KeyLogicBetweenDelay();
//    KeyLogic();








    //========================手动：前后不断切换=============================== 
    //if(!GML(M_MODE_AUTO) && GML(M_MAN_FRONTBACK) && ((GMR(M_ACTIONHEAD_FINISH) && !GML(M_CHANGEDELAY)) || GMF(M_CHANGEDELAY)))
    if(!GML(M_MODE_AUTO) && GML(M_MAN_FRONTBACK) && GMR(M_ACTIONHEAD_FINISH))
    //if(!GML(M_MODE_AUTO) && GML(M_MAN_FRONTBACK) && (GMR(M_ACTIONHEAD_FINISH) || GMF(M_CHANGEDELAY)))
    {
        //printf("Manual====FrontBack!\r\n");
        if((man.headPos&FRONTBACK_MASK))
        {
            //SML(M_ACTIONHEAD_FINISH, 0);
            man.actHead = BACK;
            //man.delay = man.delayPrint[man.platformPos];
            man.delay = 8;           
            SML(M_BACK_FINISH, 0);

            //printf("Manual====Back!\r\n");
        }
        else// if(!(man.headPos&FRONTBACK_MASK))
        {
            //SML(M_ACTIONHEAD_FINISH, 0);
            man.actHead = FRONT;
            //man.delay = man.delayAbsorb;
            man.delay = 8;
            SML(M_FRONT_FINISH, 0);

            //printf("Manual====Front!\r\n");
        }
    }

    //===========================自动============================
    if(GML(M_MODE_AUTO) && !GML(M_MODE_RESTORE) && (GMR(M_ACTIONHEAD_FINISH) || (GMR(M_ACTIONPLATFORM_FINISH) && !GML(M_MODE_RESTORE_QUIT))))
    //if(GML(M_MODE_AUTO) && !GML(M_MODE_RESTORE) && GMR(M_ACTIONHEAD_FINISH))
    {
//        if(GMR(M_ACTIONHEAD_FINISH))
//            printf("Action next GMR(M_ACTIONHEAD_FINISH)");
//        else
//            printf("Action next GMR(M_ACTIONPLATFORM_FINISH)");

        if(!GML(M_AUTO_FLAG_AUX))
        {
            do
            {
                //man.actionPos++;
                if(man.actionPos >= man.actionCnt)
                {              
                    NextPeroid();

//                    if(GML(M_PROGRAM_PERIOD))
//                    {
//                        SML(M_MODE_AUTO, 1);
//                        SML(M_MODE_RESTORE, 1);
//                        SML(M_PROGRAM_PERIOD, 1);
//
//                        SML(M_FLAG_MANUAL, 1);
//                    }
//                    else
                    {
                        SML(M_MODE_AUTO, 1);
                        SML(M_MODE_RESTORE, 1);
                    }

                    return;
                }
                
                man.pCurAction = man.program + man.actionPos;
                man.actionPos++;
    
                printf("Action next!\r\n");
                
                
                
                 
            }while(!(man.pCurAction->flag&0x01));
        }//if(!GML(M_AUTO_FLAG_AUX))
        else
        {
            if(man.actionPos >= man.actionCnt)
            {
                NextPeroid();

//                if(GML(M_PROGRAM_PERIOD))
//                {
//                    SML(M_MODE_AUTO, 0);
//                    SML(M_MODE_RESTORE, 0);
//                    SML(M_PROGRAM_PERIOD, 0);
//
//                    SML(M_FLAG_MANUAL, 1);
//                }
//                else
                {
                    SML(M_MODE_AUTO, 1);
                    SML(M_MODE_RESTORE, 1);
                }

                return;
            }
            
            man.pCurAction = man.program + man.actionPos;
            man.actionPos++;

            printf("Action next!\r\n");
                
        }//if(!GML(M_AUTO_FLAG_AUX))
        
        switch(man.pCurAction->act)
        {
            case ABSORB_DOWN:
                man.actHead = man.pCurAction->act;
                man.delay = man.delayAbsorb;
                SML(M_ABSORBDOWN_FINISH, 0);
                SML(M_ACTIONHEAD_FINISH, 0);
            break;
            case ABSORB:               
                man.actHead = man.pCurAction->act;
                man.delay = man.delayAbsorb;
                SML(M_ABSORB_FINISH, 0);
                SML(M_ACTIONHEAD_FINISH, 0);
            break;

            case PRINT_DOWN:
                man.actHead = man.pCurAction->act;
                man.delay = man.delayPrint[man.platformPos];
                SML(M_PRINTDOWN_FINISH, 0);
                SML(M_ACTIONHEAD_FINISH, 0);
            break;
            case PRINT:
                man.actHead = man.pCurAction->act;
                man.delay = man.delayPrint[man.platformPos];
                SML(M_PRINT_FINISH, 0);
                SML(M_ACTIONHEAD_FINISH, 0);
            break;

            //不需要延时
            case ABSORB_UP:
                SML(M_ABSORBUP_FINISH, 0);
                man.actHead = man.pCurAction->act;
                SML(M_ACTIONHEAD_FINISH, 0);
            break;
            case FRONT:
                SML(M_FRONT_FINISH, 0);
                man.actHead = man.pCurAction->act;
                SML(M_ACTIONHEAD_FINISH, 0);
            break;
            case BACK:
                SML(M_BACK_FINISH, 0);
                man.actHead = man.pCurAction->act;
                SML(M_ACTIONHEAD_FINISH, 0);
            break;
            case PRINT_UP:
                SML(M_PRINTUP_FINISH, 0);
                man.actHead = man.pCurAction->act;
                SML(M_ACTIONHEAD_FINISH, 0);
            break;

            case SHIFT:
            case RETURN:
                if(!GML(M_MACHINE_AUX_FAULT))//穿梭可以用
                {
                    man.actPlatform = man.pCurAction->act;
                    SML(M_ACTIONPLATFORM_FINISH, 0);
                }
                else
                {
                    NextPeroid();
                    SML(M_MODE_AUTO, 1);   //自编程是遇到平台有问题则先恢复再执行下一个周期
                    SML(M_MODE_RESTORE, 1);
                    //return;    //不能立即返回
                }
            break;
            case SHIFTRETURN:
                if(!GML(M_MACHINE_AUX_FAULT))//穿梭可以用
                {
                    man.actPlatform = man.pCurAction->act;
                    SML(M_ACTIONPLATFORM_FINISH, 0);

                    SML(M_MODE_RESTORE_QUIT, 1);
                    SML(M_SHIFTRETURN_FINISH, 0);
                    SML(M_ACTIONHEAD_FINISH, 0); //进入下一个动作    
                }
                else
                {
                    NextPeroid();
                    SML(M_MODE_AUTO, 1);   //自编程是遇到平台有问题则先恢复再执行下一个周期
                    SML(M_MODE_RESTORE, 1);
                    //return;    //不能立即返回    
                }
            break;
            case SHIFTRETURN2:
                if(!GML(M_MACHINE_AUX_FAULT))//穿梭可以用
                {
                    man.actPlatform = man.pCurAction->act;
                    SML(M_ACTIONPLATFORM_FINISH, 0);

                    SML(M_MODE_RESTORE_QUIT, 1);
                    SML(M_SHIFTRETURN2_FINISH, 0);
                    SML(M_ACTIONHEAD_FINISH, 0); //进入下一个动作
                    
                }
                else
                {
                    NextPeroid();
                    SML(M_MODE_AUTO, 1);   //自编程是遇到平台有问题则先恢复再执行下一个周期
                    SML(M_MODE_RESTORE, 1);
                    //return;    //不能立即返回    
                } 
            break;
            default:
            break;
        }  
    }
    else if(GML(M_MODE_RESTORE_QUIT) && GMR(M_ACTIONPLATFORM_FINISH))
        SML(M_MODE_RESTORE_QUIT, 0);

    //===========================恢复原始状态============================
    if(GML(M_MODE_AUTO) && GML(M_MODE_RESTORE))
    {
        //printf("Restore \r\n");
        if(GMR(M_ACTIONHEAD_FINISH))
        {
            if(man.headPos&FRONTBACK_MASK)
            {
                if((man.headPos&UPDOWN_MASK))
                {
                    man.actHead = PRINT_UP;
                    SML(M_PRINTUP_FINISH, 0);
                    
                    printf("Restore====PrintUp!\r\n");    
                }
                else
                {
                    man.actHead = BACK;
                    SML(M_BACK_FINISH, 0);

                    printf("Restore====Back!\r\n");
                }
                
            }
            else
            {
                if((man.headPos&UPDOWN_MASK))
                {
                    man.actHead = ABSORB_UP;
                    SML(M_ABSORBUP_FINISH, 0);

                    printf("Restore====AbsorbUp!\r\n");    
                }
                else
                {
                    SML(M_MODE_RESTORE, 0);
                    SML(M_ACTIONHEAD_FINISH, 0);

                    man.actionPos = 0;

                    printf("Restore quit!\r\n");

                    SML(M_FLAG_MANUAL, 0);

                    //暂停
                    if(GML(M_KEY_FOOT_PRESSED)) //跳过一次
                        SML(M_KEY_FOOT_PRESSED, 0);
                    else if(GML(M_PROGRAM_PERIOD))
                    {
                        SML(M_MODE_AUTO, 0);
                        SML(M_MODE_RESTORE, 0);
                        SML(M_PROGRAM_PERIOD, 0);

                        printf("M_PROGRAM_PERIOD\r\n");
                    }
                }
            }
        }

        if(GMR(M_ACTIONPLATFORM_FINISH))
        {
            if(GML(M_AUTO_FLAG_AUX) && man.programNum != 3 && man.programNum != 4)
            {
                if(man.platformPos)
                {
                    man.actPlatform = RETURN;
                    SML(M_RETURN_FINISH, 0);
                    
                    SML(M_MODE_RESTORE_QUIT, 1);//防止恢复原始状态时，RETURN动作完成时触发下一动作

                    printf("Restore====Return!\r\n");
                }   
            }
        }
    }


    KeyLogic();
}

void Normal(void)
{
    //识别按键
    Key_Normal();

    WDT_CONTR = 0x3C;


    //产量闪烁
    if(GML(M_OUTPUT_FLASH_FLAG))
        Output_Flash();

    //LED灯
    LED_RedGreen();

    //产量清0
    if(GMR(M_KEY_CLEAR))
    {
        man.productOutput = 0;
        sprintf((char*)man.segStr, "%05d", (int)(man.productOutput));
        TM1638_SendData(0, man.segStr);
        
        AT24CXX_WriteLenByte(man.productOutputAddr, 0, 2);

        WDT_CONTR = 0x3C;
    }

//    printf("man.actHead = %d\r\n", (int)man.actHead);
//    printf("man.pCurAction->act = %d\r\n", (int)man.pCurAction->act);

    //==================================================================
    //=============================执行动作=============================
    //==================================================================
    if(!GML(M_MODE_AUTO) && !GML(M_MODE_RESTORE))//手动中，前后动作需要延时
    {       
        if((man.actHead == FRONT || man.actHead == BACK) && man.delay)
        {
            if(!GML(M_ACTION_DELAY))
            {
                TS(0, man.delayUnit);//动作延时最小单位
                SML(M_ACTION_DELAY, 1);
            }
            else if(TG(0))
            {
                man.delay--;
                SML(M_ACTION_DELAY, 0);
    
                printf("+++++Manual+++++Delay = %d!\r\n", (int)man.delay);
    
                
            }
        }
        else
        {
            //前后动作特殊对待
            switch(man.actHead)
            {
                case FRONT:
                    Front();
                break;
                case BACK:
                    Back();
                break;
                default:
                break;
            }
        }
    
        //印头动作
        switch(man.actHead)
        {
            case ABSORB_DOWN:
                AbsorbDown();
            break;
            case ABSORB_UP:
                AbsorbUp();
            break;
            case ABSORB:
                Absorb();
            break;
//            case FRONT:
//                Front();
//            break;
//            case BACK:
//                Back();
//            break;
            case PRINT_DOWN:
                PrintDown();
            break;
            case PRINT_UP:
                PrintUp();
            break;
            case PRINT:
                Print();
            break;
            default:
            break;
        }
    }
    else if(GML(M_MODE_AUTO) && !GML(M_MODE_RESTORE))//自动程序中，吸油和印油需要延时
    {
        if((man.actHead == ABSORB_DOWN || man.actHead == ABSORB 
            || man.actHead == PRINT_DOWN || man.actHead == PRINT) && man.delay)
        {
            if(!GML(M_ACTION_DELAY))
            {
                TS(0, man.delayUnit);//动作延时最小单位
                SML(M_ACTION_DELAY, 1);
            }
            else if(TG(0))
            {
                man.delay--;
                SML(M_ACTION_DELAY, 0);
    
                //printf("+++++Auto+++++Delay = %d!\r\n", (int)man.delay);
            }
        }
        else
        {
            //吸油和印油动作特殊对待
            switch(man.actHead)
            {
                case ABSORB_DOWN:
                    AbsorbDown();
                break;
                case ABSORB:
                    Absorb();
                break;
                case PRINT_DOWN:
                    if(GML(M_SHIFT_FINISH) && GML(M_RETURN_FINISH) && GML(M_SHIFTRETURN_FINISH) && GML(M_SHIFTRETURN2_FINISH))
                        PrintDown();
                break;
                case PRINT:
                    if(GML(M_SHIFT_FINISH) && GML(M_RETURN_FINISH) && GML(M_SHIFTRETURN_FINISH) && GML(M_SHIFTRETURN2_FINISH))
                        Print();
                break;
                default:
                break;
            }
        }
    
        //印头动作
        switch(man.actHead)
        {
//            case ABSORB_DOWN:
//                AbsorbDown();
//            break;
            case ABSORB_UP:
                AbsorbUp();
            break;
//            case ABSORB:
//                Absorb();
//            break;
            case FRONT:
                Front();
            break;
            case BACK:
                Back();
            break;
//            case PRINT_DOWN:
//                PrintDown();
//            break;
            case PRINT_UP:
                PrintUp();
            break;
//            case PRINT:
//                Print();
//            break;
            default:
            break;
        }        
    }
    else if(GML(M_MODE_AUTO) && GML(M_MODE_RESTORE))//恢复原始状态
    {
         //印头动作
        switch(man.actHead)
        {
            case ABSORB_DOWN:
                AbsorbDown();
            break;
            case ABSORB_UP:
                AbsorbUp();
            break;
            case ABSORB:
                Absorb();
            break;
            case FRONT:
                Front();
            break;
            case BACK:
                Back();
            break;
            case PRINT_DOWN:
                PrintDown();
            break;
            case PRINT_UP:
                PrintUp();
            break;
            case PRINT:
                Print();
            break;
            default:
            break;
        }        
    }



    //=============================穿梭平台动作=============================
    if(!GML(M_MACHINE_AUX_FAULT))//穿梭可以用
    {
        //if(GML(M_PRINT_FINISH) && GML(M_PRINTDOWN_FINISH))
        {
            switch(man.actPlatform)
            {
                case SHIFT:
                    man.Shift();
                break;
                case RETURN:
                    man.Return();
                break;
                case SHIFTRETURN:
                    man.ShiftReturn();
                break;
                case SHIFTRETURN2:
                    man.ShiftReturn2();
                break;
                default:
                break;
            }
        }
    }
//    else
//        man.actPlatform = ACTION_NONE;





    
}
