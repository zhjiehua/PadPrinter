#include "NormalAuto.h"
#include "NormalMannual.h"
#include "NormalRestore.h"
#include "simplc_io.h"
#include "management.h"
#include "stdio.h"
#include "tm1638.h"

//�Զ�ģʽ�Ķ��������л�
void NormalAuto_ActionsChange(void)
{
    if(!GML(M_AUTO_FLAG_AUX))
    {
        do
        {
            if(man.actionPos >= man.actionCnt)
            {              
                NextPeroid();

                man.runMode = RM_RESTORE;
                SML(M_FLAG_MANUAL, 1); //��ֹ�ڸտ�ʼRESTOREʱ����ͣ�ˣ���ʱӡͷ����ǰ�棬Ȼ���ٰ���ʼ�ˣ���ʱ��������ͣ�ӡͷ����ǰ����ѹ������
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

            man.runMode = RM_RESTORE;
            SML(M_FLAG_MANUAL, 1); //��ֹ�ڸտ�ʼRESTOREʱ����ͣ�ˣ���ʱӡͷ����ǰ�棬Ȼ���ٰ���ʼ�ˣ���ʱ��������ͣ�ӡͷ����ǰ����ѹ������
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
            man.waitMSignal = M_ABSORBDOWN_FINISH;
        break;
        case ABSORB:               
            man.actHead = man.pCurAction->act;
            man.delay = man.delayAbsorb;
            SML(M_ABSORB_FINISH, 0);
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_ABSORB_FINISH;
        break;

        case PRINT_DOWN:
            man.actHead = man.pCurAction->act;
            man.delay = man.delayPrint[man.platformPos];
            SML(M_PRINTDOWN_FINISH, 0);
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_PRINTDOWN_FINISH;
        break;
        case PRINT:
            man.actHead = man.pCurAction->act;
            man.delay = man.delayPrint[man.platformPos];
            SML(M_PRINT_FINISH, 0);
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_PRINT_FINISH;
        break;

        //����Ҫ��ʱ
        case ABSORB_UP:
            SML(M_ABSORBUP_FINISH, 0);
            man.actHead = man.pCurAction->act;
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_ABSORBUP_FINISH;
        break;
        case FRONT:
            SML(M_FRONT_FINISH, 0);
            man.actHead = man.pCurAction->act;
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_FRONT_FINISH;
        break;
        case BACK:
            SML(M_BACK_FINISH, 0);
            man.actHead = man.pCurAction->act;
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_BACK_FINISH;
        break;
        case PRINT_UP:
            SML(M_PRINTUP_FINISH, 0);
            man.actHead = man.pCurAction->act;
            SML(M_ACTIONHEAD_FINISH, 0);
            man.waitMSignal = M_PRINTUP_FINISH;
        break;

        //ƽ̨����
        case SHIFT:
            if(!GML(M_MACHINE_AUX_FAULT))//���������
            {
                man.actPlatform = man.pCurAction->act;
                SML(M_SHIFT_FINISH, 0);
                SML(M_ACTIONPLATFORM_FINISH, 0);
                man.waitMSignal = M_SHIFT_FINISH;
            }
            else
            {
                NextPeroid();
                man.runMode = RM_RESTORE;
            }
        break;
        case RETURN:
            if(!GML(M_MACHINE_AUX_FAULT))//���������
            {
                man.actPlatform = man.pCurAction->act;
                SML(M_RETURN_FINISH, 0);
                SML(M_ACTIONPLATFORM_FINISH, 0);
                man.waitMSignal = M_RETURN_FINISH;
            }
            else
            {
                NextPeroid();
                man.runMode = RM_RESTORE;
            }
        break;
        case SHIFTRETURN:
            if(!GML(M_MACHINE_AUX_FAULT))//���������
            {
                man.actPlatform = man.pCurAction->act;
                SML(M_ACTIONPLATFORM_FINISH, 0);

                SML(M_MODE_RESTORE_QUIT, 1);                       
                SML(M_SHIFTRETURN_FINISH, 0);
                SML(M_ACTIONHEAD_FINISH, 0); //������һ������
                man.waitMSignal = M_WAITMSIGNAL_NONE;    
            }
            //else
            {
                NextPeroid();
                man.runMode = RM_RESTORE;   
            }
        break;
        case SHIFTRETURN2:
            if(!GML(M_MACHINE_AUX_FAULT))//���������
            {
                man.actPlatform = man.pCurAction->act;
                SML(M_ACTIONPLATFORM_FINISH, 0);

                SML(M_MODE_RESTORE_QUIT, 1);
                SML(M_SHIFTRETURN2_FINISH, 0);
                SML(M_ACTIONHEAD_FINISH, 0); //������һ������
                man.waitMSignal = M_WAITMSIGNAL_NONE;                    
            }
            //else
            {
                NextPeroid();
                man.runMode = RM_RESTORE;    
            } 
        break;
        default:
            man.waitMSignal = M_WAITMSIGNAL_NONE;
        break;
    }      
}

void NormalAuto_KeyProcess(void)
{
    if(man.keyPress != KEY_NONE)
    {
        //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

        if(GML(M_ACTION_FINISH))
        {
            //printf("The man.keyPress = %d\r\n", (int)man.keyPress);

            man.keyPressLast = man.keyPress;
            man.keyPress = KEY_NONE;

            switch(man.keyPressLast)
            {
                case KEY_STARTSTOP:

                    printf("NormalAuto+++++KEY_STARTSTOP\r\n");

                    if(!(man.headPos&UPDOWN_MASK)) //��ͣʱ����ѹ
                    {
                        man.runMode = RM_MANNUAL;

                        printf("Stop the program!\r\n");
                    }
                    else
                    {
                        man.keyPress = man.keyPressLast;//�����������ȴ�������ͣ�Ļ���
                        printf("Can not stop the program when the head is print!\r\n");
                    }
                break;
                case KEY_AUX:

                    printf("NormalAuto+++++KEY_AUX\r\n");

                    if(!GML(M_AUTO_FLAG_AUX))
                    {
                        SML(M_AUTO_FLAG_AUX, 1);
                        
                        //�������ԭʼλ�ã������¿�ʼ����
                        //����Ϊƽ̨����ԭʼλ�ã���ƽ̨��λ
                        if(man.platformPos)
                        {
                            if(!GML(M_AUTO_FLAG_OIL))//���ӡ���ˣ���ӡͷ��ƽ̨�����»�λ
                            {
                                man.runMode = RM_RESTORE;
        
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
                break;
                case KEY_UPDOWN:

                    printf("NormalAuto+++++KEY_UPDOWN\r\n");

                    man.runMode = RM_MANNUAL;

                    if(man.programNum == 2 || man.programNum == 4 || man.programNum == 6 || man.programNum == 8)
                        SML(M_AUTO_FLAG_AUX, 0);

                    man.keyPress = KEY_UPDOWN;
                    NormalMannual_KeyProcess();

                break;
                case KEY_FRONTBACK:

                    printf("NormalAuto+++++KEY_FRONTBACK\r\n");

                    man.runMode = RM_MANNUAL;
                    
                    if(man.programNum == 2 || man.programNum == 4 || man.programNum == 6 || man.programNum == 8)
                        SML(M_AUTO_FLAG_AUX, 0);

                    man.keyPress = KEY_FRONTBACK;
                    NormalMannual_KeyProcess();

                break;
                case KEY_FOOT:
                    printf("NormalAuto+++++KEY_FOOT\r\n");

                    if(!(man.headPos&UPDOWN_MASK)) //ע�⣺���ܻ���ǰ��ͣ����
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
            }
        }
    }
}
