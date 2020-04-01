#include "NormalMannual.h"
#include "NormalRestore.h"
#include "NormalAuto.h"
#include "simplc_io.h"
#include "management.h"
#include "stdio.h"
#include "tm1638.h"

//手动模式的前后动作切换
void NormalMannual_FrontBackToggle(void)
{
    if((man.headPos&FRONTBACK_MASK))//在前面
    {
        man.actHead = BACK;
        man.delay = man.frontBackDelay; //8          
        SML(M_BACK_FINISH, 0);

        //printf("Manual====Back!\r\n");
    }
    else//在后面
    {
        man.actHead = FRONT;
        man.delay = man.frontBackDelay; //8
        SML(M_FRONT_FINISH, 0);

        //printf("Manual====Front!\r\n");
    }
}

void NormalMannual_KeyProcess(void)
{
    if(man.keyPress != KEY_NONE)
    {
        //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

        if(GML(M_MAN_FRONTBACK) && man.delay && man.keyPress != KEY_AUX)
        {
            man.delay = 0;
            SML(M_ACTION_DELAY, 0);

            SML(M_FRONT_FINISH, 1);
            SML(M_BACK_FINISH, 1);
        }

        if(GML(M_MAN_FRONTBACK)) //手动前后正在执行，不影响AUX动作
        {
            if(man.keyPress == KEY_AUX && GML(M_ACTIONPLATFORM_FINISH))
            {
                man.keyPressLast = man.keyPress;
                man.keyPress = KEY_NONE;
                
                printf("NormalMannual+++++KEY_AUX+++++FRONTBACK\r\n");

                if(man.programNum == 3 || man.programNum == 4)
                {
                    man.actPlatform = SHIFTRETURN;
                    man.waitMSignal = M_SHIFTRETURN_FINISH;
                    SML(M_SHIFTRETURN_FINISH, 0);
                }
                else if(man.programNum == 10)
                {
                    man.actPlatform = SHIFTRETURN2;
                    man.waitMSignal = M_SHIFTRETURN2_FINISH;
                    SML(M_SHIFTRETURN2_FINISH, 0);
                }
                else
                {
                    man.actPlatform = SHIFT;
                    man.waitMSignal = M_SHIFT_FINISH;
                    SML(M_SHIFT_FINISH, 0);

                    SML(M_FLAG_MANUAL, 1);
                }

                return;
            }
        }

        if(GML(M_ACTION_FINISH))
        {
            //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

            man.keyPressLast = man.keyPress;
            man.keyPress = KEY_NONE;

            switch(man.keyPressLast)
            {
                case KEY_STARTSTOP:

                    printf("NormalMannual+++++KEY_STARTSTOP\r\n");

                    SML(M_PROGRAM_FOOT, 1);

                    if(!GML(M_FLAG_MANUAL))
                    {
                        man.runMode = RM_AUTO;
                        NormalAuto_ActionsChange();

                        printf("Start the program!\r\n");
                    }
                    else
                    {
                        if(GML(M_MAN_FRONTBACK) && (man.headPos&FRONTBACK_MASK))
                        {
                            SML(M_RESTORE_DELAY, 1);
                            man.delay = man.restoreDelay;

                            printf("Wait sometime to Restore the machine!\r\n");
                        }
                        else
                        {
                            man.runMode = RM_RESTORE;
                            NormalRestore_HeadRestore();
                            NormalRestore_PlatformRestore();
                            printf("Restore the machine!\r\n");
                        }
                    }

                    if(GML(M_MAN_FRONTBACK))//停止手动前后
                        SML(M_MAN_FRONTBACK, 0);
                break;
                case KEY_AUX:

                    printf("NormalMannual+++++KEY_AUX\r\n");

                    if(GML(M_MACHINE_AUX_FAULT)) //單色不能用這功能
                        break;

                    if(man.programNum == 3 || man.programNum == 4)
                    {
                        man.actPlatform = SHIFTRETURN;
                        man.waitMSignal = M_SHIFTRETURN_FINISH;
                        SML(M_SHIFTRETURN_FINISH, 0);
                    }
                    else if(man.programNum == 10)
                    {
                        man.actPlatform = SHIFTRETURN2;
                        man.waitMSignal = M_SHIFTRETURN2_FINISH;
                        SML(M_SHIFTRETURN2_FINISH, 0);
                    }
                    else
                    {
                        man.actPlatform = SHIFT;
                        man.waitMSignal = M_SHIFT_FINISH;
                        SML(M_SHIFT_FINISH, 0);

                        SML(M_FLAG_MANUAL, 1);
                    }
                break;
                case KEY_UPDOWN:

                    printf("NormalMannual+++++KEY_UPDOWN\r\n");

                    //KeyRandom();

                    if(GML(M_MAN_FRONTBACK))//停止前后动作
                        SML(M_MAN_FRONTBACK, 0);

                    SML(M_FLAG_MANUAL, 1);

                    if(man.programNum == 9)
                    {
                        if(man.headPos&FRONTBACK_MASK)
                        {
                            if(man.headPos&UPDOWN_MASK)
                            {
                                man.actHead = PRINT_UP;
                                SML(M_PRINTUP_FINISH, 0);
                                man.waitMSignal = M_PRINTUP_FINISH;
                            }
                            else
                            {
                                man.actHead = PRINT_DOWN;
                                SML(M_PRINTDOWN_FINISH, 0);
                                man.waitMSignal = M_PRINTDOWN_FINISH;
                            }
                        }
                        else
                        {
                            man.actHead = ABSORB;
                            SML(M_ABSORB_FINISH, 0);
                            man.waitMSignal = M_ABSORB_FINISH;
                        }
                    }
                    else //不是程序9
                    {
                        if(man.headPos&FRONTBACK_MASK)
                        {
                            man.actHead = PRINT;
                            SML(M_PRINT_FINISH, 0);
                            man.waitMSignal = M_PRINT_FINISH;
                        }
                        else
                        {
                            man.actHead = ABSORB;
                            SML(M_ABSORB_FINISH, 0);
                            man.waitMSignal = M_ABSORB_FINISH;
                        }
                    }

                break;
                case KEY_FRONTBACK:

                    printf("NormalMannual+++++KEY_FRONTBACK\r\n");

                    SML(M_FLAG_MANUAL, 1);

                    if(!GML(M_MAN_FRONTBACK) && !(man.headPos&UPDOWN_MASK))//印头在下面不能执行前后
                    {
                        if(man.headPos&FRONTBACK_MASK)
                        {
                            man.actHead = BACK;
                            SML(M_BACK_FINISH, 0);
                            man.waitMSignal = M_BACK_FINISH;
                        }
                        else
                        {
                            man.actHead = FRONT;
                            SML(M_FRONT_FINISH, 0);
                            man.waitMSignal = M_FRONT_FINISH;
                        }
        
                        SML(M_MAN_FRONTBACK, 1);
                    }
                    else
                    {
                        SML(M_MAN_FRONTBACK, 0);
                    }
                break;
                case KEY_FOOT:
                    printf("NormalMannual+++++KEY_FOOT\r\n");

                    if(GML(M_MAN_FRONTBACK))
                        SML(M_MAN_FRONTBACK, 0);

                    if(!GML(M_FLAG_MANUAL))
                    {
                        man.runMode = RM_AUTO;
                        NormalAuto_ActionsChange();

                        printf("Start the program!\r\n");
                    }
                    else
                    {
                        man.runMode = RM_RESTORE;
                        NormalRestore_HeadRestore();
                        NormalRestore_PlatformRestore();
                        printf("Restore the machine!\r\n");
                    }

                    if(!GML(M_PROGRAM_FOOT))
                    {
                        SML(M_PROGRAM_PERIOD, 1);

                        if(GML(M_FLAG_MANUAL))
                            SML(M_KEY_FOOT_PRESSED, 1);
                    }
                break;
            }
        }
    }
}
