#include "NormalRestore.h"
#include "NormalAuto.h"
#include "NormalMannual.h"
#include "simplc_io.h"
#include "management.h"
#include "stdio.h"
#include "tm1638.h"

//恢复模式的印头恢复
void NormalRestore_HeadRestore(void)
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
            printf("Restore quit!\r\n");

            man.actionPos = 0;

            SML(M_FLAG_MANUAL, 0);

            //处理按键
            NormalRestore_KeyProcess();

            //暂停
            if(GML(M_KEY_FOOT_PRESSED)) //跳过一次
            {
                SML(M_KEY_FOOT_PRESSED, 0);

                if(man.runMode == RM_RESTORE)//没按键按下
                {
                    man.runMode = RM_AUTO;
                    NormalAuto_ActionsChange();
                }
            }
            else
            {
                if(GML(M_PROGRAM_PERIOD))
                {
                    man.runMode = RM_MANNUAL;
                    SML(M_PROGRAM_PERIOD, 0);
    
                    printf("M_PROGRAM_PERIOD\r\n");
                }
                else
                {
                    if(man.runMode == RM_RESTORE)//没按键按下
                    {
                        man.runMode = RM_AUTO;
                        NormalAuto_ActionsChange();
                    }
                }
            }
        }
    }
}

//恢复模式的平台恢复
void NormalRestore_PlatformRestore(void)
{
    if(GML(M_AUTO_FLAG_AUX) && man.programNum != 3 && man.programNum != 4)// && man.programNum != 10)
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

void NormalRestore_KeyProcess(void)
{
    if(man.keyPress != KEY_NONE)
    {
        printf("The man.keyPress = %d\r\n", (int)man.keyPress);

        if(GML(M_ACTIONHEAD_FINISH)) //M_ACTIONHEAD_FINISH     M_ACTION_FINISH
        {
            //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

            man.keyPressLast = man.keyPress;
            man.keyPress = KEY_NONE;

            switch(man.keyPressLast)
            {
                case KEY_STARTSTOP:

                    printf("NormalRestore+++++KEY_STARTSTOP\r\n");

                    if(!(man.headPos&UPDOWN_MASK)) //暂停时不能压
                    {
                        man.runMode = RM_MANNUAL;

                        printf("Stop the program!\r\n");
                    }
                    else
                    {
                        man.keyPress = man.keyPressLast;//保留按键，等待可以暂停的机会
                        printf("Can not stop the program when the head is print!\r\n");
                    }
                break;
//                case KEY_AUX:
//
//                    printf("NormalRestore+++++KEY_AUX\r\n");
//
////                    man.keyPressLast = man.keyPress;
////                    man.keyPress = KEY_NONE;
//
//                    if(!GML(M_AUTO_FLAG_AUX))
//                    {
//                        SML(M_AUTO_FLAG_AUX, 1);
//                        
//                        //如果不在原始位置，则重新开始程序
//                        //改正为平台不在原始位置，则平台回位
//                        if(man.platformPos)
//                        {
//                            if(!GML(M_AUTO_FLAG_OIL))//如果印过了，则印头和平台都重新回位
//                            {
//                                man.runMode = RM_RESTORE;
//        
//                                SML(M_ACTION_FINISH, 0);
//                                SML(M_ACTIONHEAD_FINISH, 0);
//                                SML(M_ACTIONPLATFORM_FINISH, 0);
//        
//                                printf("Key  Restore the machine!\r\n");
//                            }
//                            else//如果没印过，则只回平台
//                            {    
//                                man.actPlatform = RETURN;
//                                SML(M_RETURN_FINISH, 0);
//                                SML(M_ACTIONPLATFORM_FINISH, 0);
//
//                                SML(M_MODE_RESTORE_QUIT, 1);
//
//                                printf("Key  Restore====Return!\r\n");
//                            }
//                        }
//                    }   
//                break;
//                case KEY_UPDOWN:
//
//                    printf("NormalRestore+++++KEY_UPDOWN\r\n");
//
//                    man.runMode = RM_MANNUAL;
//
//                    if(man.programNum == 2 || man.programNum == 4 || man.programNum == 6 || man.programNum == 8)
//                        SML(M_AUTO_FLAG_AUX, 0);
//
//                    man.keyPress = KEY_UPDOWN;
//                    NormalMannual_KeyProcess();
//
//                break;
//                case KEY_FRONTBACK:
//
//                    printf("NormalRestore+++++KEY_FRONTBACK\r\n");
//
//                    man.runMode = RM_MANNUAL;
//                    
//                    if(man.programNum == 2 || man.programNum == 4 || man.programNum == 6 || man.programNum == 8)
//                        SML(M_AUTO_FLAG_AUX, 0);
//
//                    man.keyPress = KEY_FRONTBACK;
//                    NormalMannual_KeyProcess();
//
//                break;
                case KEY_FOOT:
                    printf("NormalRestore+++++KEY_FOOT\r\n");

                    if(!(man.headPos&UPDOWN_MASK)) //注意：可能会在前面停下来
                    //if(!(man.headPos&UPDOWN_MASK) && !(man.headPos&FRONTBACK_MASK))
                    {
                        man.runMode = RM_MANNUAL;

                        printf("Stop the program!\r\n");
                    }

                    if(!GML(M_PROGRAM_FOOT))
                    {
                        SML(M_PROGRAM_PERIOD, 1);

                        if(GML(M_FLAG_MANUAL))
                            SML(M_KEY_FOOT_PRESSED, 1);
                    }
                break;
                default:
                break;
            }
        }
    }
}
