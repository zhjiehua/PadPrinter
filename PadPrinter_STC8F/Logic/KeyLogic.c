#include "management.h"
#include "misc.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"

#include "simplc_io.h"
#include "simplc_timer.h"



void KeyLogic(void)
{
    if(man.keyPress != KEY_NONE)
    {
        //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

        if(man.delay)
        {
            man.delay = 0;
            SML(M_ACTION_DELAY, 0);
        }

        //if(GML(M_ACTIONHEAD_FINISH) && GML(M_ACTIONPLATFORM_FINISH))//�ȵ���һ����ִ����
        //if(GML(M_ACTION_FINISH) || GMF(M_ACTION_DELAY))
        if(GML(M_ACTION_FINISH))
        {
            //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

            if(man.programNum == 0 || man.programNum == 9)
            {
                if(man.headPos&UPDOWN_MASK) //��ͣʱ����ѹ
                {
                    return;
                }
            }

            man.keyPressLast = man.keyPress;
            man.keyPress = KEY_NONE;

            switch(man.keyPressLast)
            {
                case KEY_STARTSTOP:

                    printf("+++++KEY_STARTSTOP\r\n");

                    SML(M_PROGRAM_FOOT, 1);

                    if(GML(M_MODE_AUTO))//�������ִ���Զ���������ͣ
                    {
                        if(!(man.headPos&UPDOWN_MASK)) //��ͣʱ����ѹ
                        {
                            SML(M_MODE_AUTO, 0);
                            //SML(M_MODE_RESTORE, 0);

                            printf("Stop the program!\r\n");
                        }
                        else
                            printf("Can not stop the program when the head is print!\r\n");
                    }
                    else
                    {
                        if(GML(M_MAN_FRONTBACK))//ֹͣ�ֶ�ǰ��
                            SML(M_MAN_FRONTBACK, 0);

                        //if(!GML(M_FLAG_MANUAL))
                        //if(!GML(M_FLAG_MANUAL) || (!(man.headPos&FRONTBACK_MASK) && !(man.headPos&UPDOWN_MASK))) //�����ǰ��ͣ��������Restore
                        if(!GML(M_FLAG_MANUAL) && !GML(M_MODE_RESTORE))
                        {
                            SML(M_MODE_AUTO, 1);
                            SML(M_MODE_RESTORE, 0);

                            printf("Start the program!\r\n");

                            SML(M_ACTION_FINISH, 0);
                            SML(M_ACTIONHEAD_FINISH, 0);
                            SML(M_ACTIONPLATFORM_FINISH, 0);

                            //man.actionPos = 0;
                        }
                        else
                        {
                            SML(M_MODE_AUTO, 1);
                            SML(M_MODE_RESTORE, 1);

                            SML(M_ACTION_FINISH, 0);

                            SML(M_ACTIONHEAD_FINISH, 0);
                            if(GML(M_AUTO_FLAG_AUX))
                                SML(M_ACTIONPLATFORM_FINISH, 0);

                            printf("Restore the machine!\r\n");
                        }
                    }
                break;
                case KEY_AUX:

                    printf("+++++KEY_AUX\r\n");

                    if(GML(M_MODE_AUTO))
                    {
                        if(!GML(M_AUTO_FLAG_AUX))
                        {
                            SML(M_AUTO_FLAG_AUX, 1);
                            
                            //�������ԭʼλ�ã������¿�ʼ����
                            //����Ϊƽ̨����ԭʼλ�ã���ƽ̨��λ
                            if(man.platformPos)
                            {
                                if(!GML(M_AUTO_FLAG_OIL))//���ӡ���ˣ���ӡͷ��ƽ̨�����»�λ
                                {
                                    SML(M_MODE_AUTO, 1);
                                    SML(M_MODE_RESTORE, 1);
            
                                    SML(M_ACTION_FINISH, 0);
                                    SML(M_ACTIONHEAD_FINISH, 0);
                                    SML(M_ACTIONPLATFORM_FINISH, 0);
            
                                    printf("Key  Restore the machine!\r\n");
                                }
                                else//���ûӡ������ֻ��ƽ̨
                                {    
                                    man.actPlatform = RETURN;
                                    SML(M_RETURN_FINISH, 0);
                                    SML(M_ACTIONPLATFORM_FINISH, 0);
    
                                    SML(M_MODE_RESTORE_QUIT, 1);
    
                                    printf("Key  Restore====Return!\r\n");
                                }
                            }
                        }    
                    }
                    else
                    {
                        if(man.programNum == 3 || man.programNum == 4)
                        {
                            man.actPlatform = SHIFTRETURN;
    
                            SML(M_ACTIONPLATFORM_FINISH, 0);
                        }
                        else if(man.programNum == 10)
                        {
                            man.actPlatform = SHIFTRETURN2;
    
                            SML(M_ACTIONPLATFORM_FINISH, 0);
                        }
                        else
                        {
                            man.actPlatform = SHIFT;
    
                            SML(M_ACTIONPLATFORM_FINISH, 0);
    
                            SML(M_FLAG_MANUAL, 1);
                        }
                    }

                break;
                case KEY_UPDOWN:

                    printf("+++++KEY_UPDOWN\r\n");

                    if(GML(M_MODE_AUTO))
                    {
                        SML(M_MODE_AUTO, 0);
                        SML(M_MODE_RESTORE, 0);

                        if(man.programNum == 2 || man.programNum == 4 || man.programNum == 6 || man.programNum == 8)
                            SML(M_AUTO_FLAG_AUX, 0);
                    }
                    else 
                    {
                        if(GML(M_MODE_RESTORE)) //�ڻָ�ʱ��ͣ�� M_MODE_RESTORE����1����ʱӦ����0�������ֶ�ģʽ
                        {
                            SML(M_MODE_RESTORE, 0);
                        }

                        if(GML(M_MAN_FRONTBACK))//ֹͣǰ����
                            SML(M_MAN_FRONTBACK, 0);
                    }

                    SML(M_FLAG_MANUAL, 1);

                    if(man.programNum == 9)
                    {
                        if(man.headPos&FRONTBACK_MASK)
                        {
                            if(man.headPos&UPDOWN_MASK)
                                man.actHead = PRINT_UP;
                            else
                                man.actHead = PRINT_DOWN;
                        }
                        else
                            man.actHead = ABSORB;
                    }
                    else
                    {
                        if(man.headPos&FRONTBACK_MASK)
                            man.actHead = PRINT;
                        else
                            man.actHead = ABSORB;
                    }
                           
                    SML(M_ACTIONHEAD_FINISH, 0);

                break;
                case KEY_FRONTBACK:

                    printf("+++++KEY_FRONTBACK\r\n");

                    if(GML(M_MODE_AUTO))
                    {
                        SML(M_MODE_AUTO, 0);
                        SML(M_MODE_RESTORE, 0);

                        if(man.programNum == 2 || man.programNum == 4 || man.programNum == 6 || man.programNum == 8)
                            SML(M_AUTO_FLAG_AUX, 0);
                    }
                    else if(GML(M_MODE_RESTORE)) //�ڻָ�ʱ��ͣ�� M_MODE_RESTORE����1����ʱӦ����0�������ֶ�ģʽ
                    {
                        SML(M_MODE_RESTORE, 0);
                    }

                    SML(M_FLAG_MANUAL, 1);

                    if(!GML(M_MAN_FRONTBACK) && !(man.headPos&UPDOWN_MASK))//ӡͷ�����治��ִ��ǰ��
                    {
                        if(man.headPos&FRONTBACK_MASK)
                        {
                            man.actHead = BACK;
                            SML(M_BACK_FINISH, 0);
                        }
                        else
                        {
                            man.actHead = FRONT;
                            SML(M_FRONT_FINISH, 0);
                        }
        
                        SML(M_MAN_FRONTBACK, 1);

                        SML(M_ACTIONHEAD_FINISH, 0);
                    }
                    else
                    {
                        SML(M_MAN_FRONTBACK, 0);

                    }
                break;
                case KEY_FOOT:
                    printf("+++++KEY_FOOT\r\n");

                    if(GML(M_MODE_AUTO))//�������ִ���Զ���������ͣ
                    {
                        if(!(man.headPos&UPDOWN_MASK)) //ע�⣺���ܻ���ǰ��ͣ����
                        //if(!(man.headPos&UPDOWN_MASK) && !(man.headPos&FRONTBACK_MASK))
                        {
                            SML(M_MODE_AUTO, 0);
                            //SML(M_MODE_RESTORE, 0);

                            printf("Stop the program!\r\n");
                        }
                    }
                    else
                    {
                        if(GML(M_MAN_FRONTBACK))
                            SML(M_MAN_FRONTBACK, 0);

                        //if(!GML(M_FLAG_MANUAL) || (!(man.headPos&FRONTBACK_MASK) && !(man.headPos&UPDOWN_MASK))) //�����ǰ��ͣ��������Restore
                        if(!GML(M_FLAG_MANUAL) && !GML(M_MODE_RESTORE))
                        {
                            SML(M_MODE_AUTO, 1);
                            SML(M_MODE_RESTORE, 0);

                            printf("Start the program!\r\n");

                            SML(M_ACTION_FINISH, 0);
                            //if(man.pCurAction->act <= PRINT)
                                SML(M_ACTIONHEAD_FINISH, 0);
                            //else
                                SML(M_ACTIONPLATFORM_FINISH, 0);
                        }
                        else
                        {
                            SML(M_MODE_AUTO, 1);
                            SML(M_MODE_RESTORE, 1);

                            SML(M_ACTION_FINISH, 0);

                            SML(M_ACTIONHEAD_FINISH, 0);
                            if(GML(M_AUTO_FLAG_AUX))
                                SML(M_ACTIONPLATFORM_FINISH, 0);

                            printf("Restore the machine!\r\n");
                        }
                    }

                    if(!GML(M_PROGRAM_FOOT))
                    {
                        SML(M_PROGRAM_PERIOD, 1);

                        if(GML(M_FLAG_MANUAL))
                            SML(M_KEY_FOOT_PRESSED, 1);
                    }
                break;
            }
        }//if(GML(M_ACTION_FINISH))
    }
}

void KeyLogicBetweenDelay(void)
{
    if(!GML(M_MODE_AUTO) && !GML(M_MODE_RESTORE))//�ֶ�
    {
        if(man.delay && man.keyPress == KEY_FRONTBACK)
        {
            SML(M_FLAG_MANUAL, 1);

            if(GML(M_MAN_FRONTBACK))//ӡͷ�����治��ִ��ǰ��
            {
                SML(M_MAN_FRONTBACK, 0);
            }
        }

        //
        if(!GML(M_MAN_FRONTBACK)) //ǰ�������й���������ʱʱ����ֹͣ
        {
            if(man.actHead == BACK)
            {
                SML(M_BACK_FINISH, 1);
            }
            else
            {
                SML(M_FRONT_FINISH, 1);
            }

            man.actHead == ACTION_NONE;
            SML(M_ACTIONHEAD_FINISH, 1);
            man.delay = 0;
        }
    }    
}

