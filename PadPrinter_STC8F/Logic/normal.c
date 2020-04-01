#include "management.h"
#include "misc.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include "NormalAuto.h"
#include "NormalMannual.h"
#include "NormalRestore.h"
 
//����������İ���ʶ��
void Key_Normal(void)
{
    /*������ʱ*/
    if(TG(1))
    {
        if(GML(M_KEY_SLOW))//��  M_KEY_SLOW  M_KEY_FAST
        {
            SML(M_OUTPUT_FLASH_FLAG, 0);
            SML(M_SAVE_DELAY, 1);

            if(!(man.delayHeadPos)) //�ں���
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
        else if(GML(M_KEY_FAST))//��
        {
            SML(M_OUTPUT_FLASH_FLAG, 0);
            SML(M_SAVE_DELAY, 1);

            if(!(man.delayHeadPos)) //�ں���
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
            
    //��¼��������
    if(GMR(M_KEY_STARTSTOP))
        man.keyPress = KEY_STARTSTOP;
    else if(GMR(M_KEY_AUX))
    {
        if(man.keyPress == KEY_STARTSTOP && man.runMode == RM_MANNUAL)
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
    else if(GMR(M_KEY_FOOT_AFTERFILTER)) //M_KEY_FOOT_AFTERFILTER  M_KEY_FOOT
        man.keyPress = KEY_FOOT;  
    else if(man.runMode == RM_MANNUAL && GML(M_KEY_UPDOWN) && (GML(M_ABSORB_FINISH) || GML(M_PRINT_FINISH)))
        man.keyPress = KEY_UPDOWN;

    if(GML(M_ABSORBDOWN_FINISH) && GML(M_ABSORBUP_FINISH) && GML(M_ABSORB_FINISH)
            && GML(M_PRINTDOWN_FINISH) && GML(M_PRINTUP_FINISH) && GML(M_PRINT_FINISH)
            && GML(M_FRONT_FINISH) && GML(M_BACK_FINISH))
    {
        SML(M_ACTIONHEAD_FINISH, 1);
        //man.actHead = ACTION_NONE;

        //printf("---------M_ACTIONHEAD_FINISH\r\n");
    }
    else
        SML(M_ACTIONHEAD_FINISH, 0);
        
    if(GML(M_SHIFT_FINISH) && GML(M_RETURN_FINISH) && GML(M_SHIFTRETURN_FINISH) && GML(M_SHIFTRETURN2_FINISH))
    {
        SML(M_ACTIONPLATFORM_FINISH, 1);
        //man.actPlatform = ACTION_NONE;

        //printf("---++----M_ACTIONPLATFORM_FINISH\r\n");
    }
    else
        SML(M_ACTIONPLATFORM_FINISH, 0);  

    if(GML(M_ACTIONHEAD_FINISH) && GML(M_ACTIONPLATFORM_FINISH))
    {
        man.keyPressLast = KEY_NONE;

        SML(M_ACTION_FINISH, 1);
    }
    else
        SML(M_ACTION_FINISH, 0);

    if((GML(M_MAN_FRONTBACK) && man.keyPress == KEY_AUX)) //�ֶ�ǰ��ʱ�������ӡͷ�Ƕ�����
    {
        if(!GML(M_ACTIONPLATFORM_FINISH))
            man.keyPress = KEY_NONE;
    }
    else if((!GML(M_ACTIONHEAD_FINISH) || !GML(M_ACTIONPLATFORM_FINISH))
        && man.keyPressLast == man.keyPress && man.keyPress != KEY_NONE//���ڶ���ʱ������ͬ�ļ�
        && (man.keyPress != KEY_STARTSTOP && man.keyPress != KEY_FRONTBACK && man.keyPress != KEY_FOOT))//��3�������ظ���2��
    {
        man.keyPress = KEY_NONE;
        //printf("man.keyPress = KEY_NONE\r\n");
    } 

//    if(man.keyPressLast == man.keyPress && man.keyPressLast != KEY_NONE)
//        printf("man.keyPressLast == man.keyPress\r\n");





    if(man.runMode == RM_MANNUAL && !GML(M_RESTORE_DELAY)) //��ֹ�ڵȴ������У�ӡͷ��ǰ��ʱ���������£�������ѹ����������
        NormalMannual_KeyProcess();
    else if(man.runMode == RM_AUTO)
        NormalAuto_KeyProcess();
    else if(man.runMode == RM_RESTORE)  //�ָ������в���Ӧ������ֻ��ӡͷ���ָ����������Ӧһ�ΰ���
        NormalRestore_KeyProcess();

    //========================�ֶ���ǰ�󲻶��л�=============================== 
    if(man.runMode == RM_MANNUAL)
    {
        if(GML(M_MAN_FRONTBACK) && (GMR(M_BACK_FINISH) || GMR(M_FRONT_FINISH)))
            NormalMannual_FrontBackToggle();
        else if(GML(M_RESTORE_DELAY))  //����ִ��ǰ����ʱ����ʱһС��ʱ���ٽ���ָ�ģʽ
        {
            if(man.delay)
            {
                if(!GML(M_ACTION_DELAY))
                {
                    TS(0, man.delayUnit);//������ʱ��С��λ
                    SML(M_ACTION_DELAY, 1);
                }
                else if(TG(0))
                {
                    man.delay--;
                    SML(M_ACTION_DELAY, 0);
    
                    //printf("Wait++++man.delay = %d\r\n", (int)man.delay);
                }
            }
            else
            {
                SML(M_RESTORE_DELAY, 0);

                man.runMode = RM_RESTORE;
                NormalRestore_HeadRestore();
                NormalRestore_PlatformRestore();
   
                printf("Restore the machine!\r\n");
            }
        }
    }

    //===========================�Զ�============================
    //if(GML(M_MODE_AUTO) && !GML(M_MODE_RESTORE) && (GMR(M_ACTIONHEAD_FINISH) || (GMR(M_ACTIONPLATFORM_FINISH) && !GML(M_MODE_RESTORE_QUIT))))
    if(man.runMode == RM_AUTO)
    {
        if(GML(man.waitMSignal))
            NormalAuto_ActionsChange();
    }

    //===========================�ָ�ԭʼ״̬============================
    if(man.runMode == RM_RESTORE)
    {
        //printf("Restore \r\n");
        if(GMR(M_ACTIONHEAD_FINISH))
        {
            NormalRestore_HeadRestore();   
        }

        if(GMR(M_ACTIONPLATFORM_FINISH))
        {
            NormalRestore_PlatformRestore();
        }
    }
}

void Normal(void)
{
    //��������
    //SMR(KeyRandom(), 1);
    //SMR(M_KEY_FOOT_AFTERFILTER, 1);
//    if(man.runMode == RM_RESTORE)
//    {
//            
//    }


    //ʶ�𰴼�
    Key_Normal();

    WDT_CONTR = 0x3C;


    //������˸
    if(GML(M_OUTPUT_FLASH_FLAG))
        Output_Flash();

    //LED��
    LED_RedGreen();

    //������0
    if(GMR(M_KEY_CLEAR))
    {
        man.productOutput = 0;
        sprintf((char*)man.segStr, "%05ld", (int32_t)(man.productOutput));
        TM1638_SendData(0, man.segStr);
        
        AT24CXX_WriteLenByte(man.productOutputAddr, 0, 4);

        WDT_CONTR = 0x3C;
    }

//    printf("man.actHead = %d\r\n", (int)man.actHead);
//    printf("man.pCurAction->act = %d\r\n", (int)man.pCurAction->act);

    //==================================================================
    //=============================ִ�ж���=============================
    //==================================================================
    if(man.runMode == RM_MANNUAL) //�ֶ�
    {       
        if((man.actHead == FRONT || man.actHead == BACK) && man.delay)//�ֶ��У�ǰ������Ҫ��ʱ
        {
            if(!GML(M_ACTION_DELAY))
            {
                TS(0, man.delayUnit);//������ʱ��С��λ
                SML(M_ACTION_DELAY, 1);
            }
            else if(TG(0))
            {
                man.delay--;
                SML(M_ACTION_DELAY, 0);
    
                //printf("+++++Manual+++++Delay = %d!\r\n", (int)man.delay);
    
                
            }
        }
        else
        {
            //ǰ��������Դ�
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
    
        //ӡͷ����
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
    else if(man.runMode == RM_AUTO)  //�Զ�
    {
        if((man.actHead == ABSORB_DOWN || man.actHead == ABSORB 
            || man.actHead == PRINT_DOWN || man.actHead == PRINT) && man.delay) //�Զ������У����ͺ�ӡ����Ҫ��ʱ
        {
            if(!GML(M_ACTION_DELAY))
            {
                TS(0, man.delayUnit);//������ʱ��С��λ
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
            //���ͺ�ӡ�Ͷ�������Դ�
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
    
        //ӡͷ����
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
    else if(man.runMode == RM_RESTORE)//�ָ�ԭʼ״̬
    {
         //ӡͷ����
        switch(man.actHead)
        {
            case ABSORB_UP:
                AbsorbUp();
            break;
            case BACK:
                Back();
            break;
            case PRINT_UP:
                PrintUp();
            break;
            default:
            break;
        }        
    }



    //=============================����ƽ̨����=============================
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
