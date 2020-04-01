#include "management.h"
#include "man.h"
#include "misc.h"
#include "actions.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"

//=================================ӡͷ����=================================

//������
void AbsorbDown(void)
{
    switch(man.headState)
    {
        case 0:
            if(GXL(X_ABSORB_O))
            {
                printf("Action----Absorb Down\r\n");
                SYL(Y_UPDOWN, 1);
                man.headState = 1;

                SML(M_ABSORBDOWN_FINISH, 0);

                SML(M_AUTO_FLAG_OIL, 1);
            }                      
        break;
        case 1:
            if(GXL(X_ABSORB_L))//������λ
            {
                printf("Action----Absorb Down----Finish\r\n");
              
                SML(M_ABSORBDOWN_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos |= UPDOWN_MASK;
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

void AbsorbUp(void)
{
    switch(man.headState)
    {
        case 0:
            SYL(Y_UPDOWN, 0);
            man.headState = 1;
            
            printf("Action----Absorb Up\r\n");

            SML(M_ABSORBUP_FINISH, 0);
        break;
        case 1:
            if(GXL(X_ABSORB_O))//������λ
            {
                printf("Action----Absorb Up----Finish\r\n");
              
                SML(M_ABSORBUP_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos &= ~UPDOWN_MASK;               
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

//����
void Absorb(void)
{
    switch(man.headState)
    {
        case 0:
            if(GXL(X_ABSORB_O))
            {
                SYL(Y_UPDOWN, 1);
                man.headState = 1;
                
                printf("Action----Absorb\r\n");
    
                SML(M_ABSORB_FINISH, 0);
    
                SML(M_AUTO_FLAG_OIL, 1);
            }
        break;
        case 1:
            if(GXL(X_ABSORB_L))//������λ
            {
                SYL(Y_UPDOWN, 0);
                man.headState = 2;
            }
        break;
        case 2:
            if(GXL(X_ABSORB_O))//������λ
            {
                printf("Action----Absorb----Finish\r\n");
              
                SML(M_ABSORB_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos &= ~UPDOWN_MASK;                
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

void PrintDown(void)
{
    switch(man.headState)
    {
        case 0:
            if(GXL(X_PRINT_O))
            {
                SYL(Y_UPDOWN, 1);
                man.headState = 1;
                
                printf("Action----Print Down\r\n");
    
                SML(M_PRINTDOWN_FINISH, 0);
    
                SML(M_AUTO_FLAG_OIL, 0);
            }
        break;
        case 1:
            if(GXL(X_PRINT_L))//������λ
            {
                printf("Action----Print Down----Finish\r\n");
              
                SML(M_PRINTDOWN_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos |= UPDOWN_MASK;
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

void PrintUp(void)
{
    switch(man.headState)
    {
        case 0:
            SYL(Y_UPDOWN, 0);
            man.headState = 1;
            
            printf("Action----Print Up\r\n");

            SML(M_PRINTUP_FINISH, 0);
        break;
        case 1:
            if(GXL(X_PRINT_O))//������λ
            {
                printf("Action----Print Up----Finish\r\n");
              
                SML(M_PRINTUP_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos &= ~UPDOWN_MASK;
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

//ӡ��
void Print(void)
{
    switch(man.headState)
    {
        case 0:
            if(GXL(X_PRINT_O))
            {
                SYL(Y_UPDOWN, 1);
                man.headState = 1;
                
                printf("Action----Print\r\n");
    
                SML(M_PRINT_FINISH, 0);
    
                SML(M_AUTO_FLAG_OIL, 0);
            }
        break;
        case 1:
            if(GXL(X_PRINT_L))//������λ
            {
                SYL(Y_UPDOWN, 0);
                man.headState = 2;
            }
        break;
        case 2:
            if(GXL(X_PRINT_O))//������λ
            {
                printf("Action----Print----Finish\r\n");
              
                SML(M_PRINT_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos &= ~UPDOWN_MASK;
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

//ǰ
void Front(void)
{
    switch(man.headState)
    {
        case 0:
            SYL(Y_FRONTBACK, 1);
            man.headState = 1;

            man.delayHeadPos = 1;

            printf("Action----Front\r\n");

            SML(M_FRONT_FINISH, 0);
        break;
        case 1:
            if(GXL(X_PRINT_O))//������λ
            {
                printf("Action----Front----Finish\r\n");

                SYL(Y_SCRAPER, 1);

                SML(M_FRONT_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos |= FRONTBACK_MASK;   
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

//��
void Back(void)
{
    switch(man.headState)
    {
        case 0:
            SYL(Y_FRONTBACK, 0);
            man.headState = 1;
            
            man.delayHeadPos = 0;

            printf("Action----Back\r\n");

            SML(M_BACK_FINISH, 0);
        break;
        case 1:
            if(GXL(X_SCRAPER))
                SYL(Y_SCRAPER, 0);

            if(GXL(X_ABSORB_O))//������λ
            {
                printf("Action----Back----Finish\r\n");
                
                SML(M_BACK_FINISH, 1);
                man.actHead = ACTION_NONE;

                man.headPos &= ~FRONTBACK_MASK;
                man.headState = 0;
            }
        break;
        default:
        break;
    }
}

//=================================ƽ̨����=================================

void Shift3Sensors(void)
{
    switch(man.platformState)
    {
        case 0:
            if(GXL(X_SHIFT_L1))//��ǰ���ѵ����޵㣬�����˻ص�ԭ��
            {
                if(man.platformPos == 0)//ֻ��һ��λ�ã��������˲��衣��������²�����ָ����
                {
                    printf("Action----Shift3Sensors----Finish----%d\r\n", (int)man.platformPos);
                    
                    SML(M_SHIFT_FINISH, 1);
                    man.actPlatform = ACTION_NONE;
                }
                else//�˻ص�λ��0
                {
                    man.actPlatform = RETURN;

                    man.delayPlatformPos = 0;

                    printf("Action----Shift3Sensors++++Return3Sensors\r\n");

                    SML(M_SHIFT_FINISH, 1);
                    SML(M_RETURN_FINISH, 0);
                    man.waitMSignal = M_RETURN_FINISH;
                }
            }
            else//δ�����޵�
            {
                SML(M_SHIFT_FINISH, 0);
                                    
                if(man.platformPos == 0)//λ��0ֱ����λ
                {
                    SYL(Y_POS, 1);

                    SYL(Y_SHIFT2, 0);
                    SYL(Y_SHIFT, 1);

                    printf("Action----Shift3Sensors0----%d\r\n", (int)man.platformPos);
                    
                    man.platformState = 6;
                }
                else//�Ȼ���һ��
                {
                    SYL(Y_SHIFT, 0);//����һ��
                    SYL(Y_SHIFT2, 1);

                    man.platformState = 20;
                    
                    printf("Action----Shift3Sensors++++%d\r\n", (int)man.platformPos);

                    //SYL(Y_POS, 0);
                }

                man.delayPlatformPos++;
            }
        break;
        case 20:
            if(!GXL(X_POS))
              man.platformState = 1;
              //man.platformState = 2; 
        break;
        case 1:
            {
                static uint16_t cnt = 0;

                if(cnt++ >= man.returnPosDelay)
                {
                    cnt = 0;
                    SYL(Y_SHIFT2, 0);
                    SYL(Y_SHIFT, 0);
                    
                    //printf("Action----Shift3Sensors----man.returnPosDelay----%d\r\n", (int)man.returnPosDelay);
                    
                    man.platformState = 2;
                }
            }
        break;
        case 2:
            SYL(Y_POS, 0);
            man.platformState = 3;
        break;

        case 3:
            {
                static uint16_t cnt = 0;

                if(cnt++ >= man.returnNoneDelay)
                {
                    cnt = 0;
                    SYL(Y_SHIFT2, 0);
                    SYL(Y_SHIFT, 1);
                    
                    //printf("Action----Shift3Sensors----man.returnNoneDelay----%d\r\n", (int)man.returnNoneDelay);

                    man.platformState = 4;
                }
            }
        break;

        case 4:
            if(GXL(X_POS))
            {
                man.platformState = 5;
                
            }
        break;

        case 5:
            if(!GXL(X_POS)) //�Ƴ���λ������
            {
                static uint16_t cnt = 0;

                if(cnt++ >= man.shiftPosDelay)
                {
                    cnt = 0;
                    SYL(Y_POS, 1);
                    
                    //printf("Action----Shift3Sensors----man.shiftPosDelay----%d\r\n", (int)man.shiftPosDelay);

                    man.platformState = 6;
                }
            }
        break;
        case 6:
            if(GXL(X_POS) || GXL(X_SHIFT_L1))
            {
                SYL(Y_SHIFT2, 1); //����һ��
                SYL(Y_SHIFT, 0);

                man.platformState = 7;
            }
        break;
        case 7:
            {
                static uint8_t cnt = 0;

                if(cnt++ > man.shiftStopDelay)
                {
                    cnt = 0;

                    SYL(Y_SHIFT2, 0);
                    SYL(Y_SHIFT, 1);

                    //printf("Action----Shift3Sensors----man.shiftStopDelay----%d\r\n", (int)man.shiftStopDelay);

                    man.platformState = 8;
                }    
            }
        break;
        case 8:
            if(GXL(X_POS) || GXL(X_SHIFT_L1))
            {     
                man.platformPos++;
                
                printf("Action----Shift3Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_SHIFT_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void Shift4Sensors(void)
{
    switch(man.platformState)
    {
        case 0:
            if(GXL(X_SHIFT_L1) && GXL(X_SHIFT_L2))//��ǰ���ѵ����޵㣬�����˻ص�ԭ��
            {
                if(man.platformPos == 0)//ֻ��һ��λ�ã��������˲��衣��������²�����ָ����
                {
                    printf("Action----Shift4Sensors----Finish----%d\r\n", (int)man.platformPos);
                    
                    SML(M_SHIFT_FINISH, 1);
                    man.actPlatform = ACTION_NONE;
                }
                else//�˻ص�λ��0
                {
                    man.actPlatform = RETURN;

                    man.delayPlatformPos = 0;

                    printf("Action----Shift4Sensors++++Return4Sensors\r\n");

                    SML(M_SHIFT_FINISH, 1);
                    SML(M_RETURN_FINISH, 0);
                    man.waitMSignal = M_RETURN_FINISH;
                }
            }
            else//δ�����޵�
            {
                SML(M_SHIFT_FINISH, 0);
                                      
                if(man.platformPos == 0)//λ��0ֱ����λ
                {
                    //if(GML(X_SHIFT_O))
                    {
                        SYL(Y_POS, 1);
                        SYL(Y_SHIFT, 1);
                        
                        printf("Action----Shift4Sensors0----%d\r\n", (int)man.platformPos);
                        
                        man.platformState = 3;
                    }
                }
                else//�Ȼ���һ��
                {
                    //if(GML(X_SHIFT_L1))
                    {
                        SYL(Y_SHIFT, 0);//����һ��
                        
                        man.platformState = 1;
                        
                        printf("Action----Shift4Sensors++++%d\r\n", (int)man.platformPos);
                    }
//                    else
//                        UART1_printf("Action----Shift++++Waitinf for X_SHIFT_L1\r\n");
                }

                man.delayPlatformPos++;
            }
        break;
        case 1:
            if(GXF(X_SHIFT_L2))//���˵�λ
            {
                SYL(Y_POS, 0);
                SYL(Y_SHIFT, 1);
                
                //printf("Action----Shift----%d\r\n", (int)man.platformPos);
                
                man.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_POS))//������λ����
            {
                SYL(Y_POS, 1);
                
                man.platformState = 3;
            }
        break;
        case 3:
            if(GXL(X_SHIFT_L1))
            {     
                man.platformPos++;
                
                printf("Action----Shift4Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_SHIFT_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void Shift2Sensors(void)
{
//#if MACHINE3SENSOR_SIM_2SENSOR 
//    uint8_t sensor = X_POS;      
//#else
//    uint8_t sensor = X_SHIFT_L1;
//#endif
    uint8_t sensor = X_POS;

    switch(man.platformState)
    {
        case 0:
            if(GXL(sensor))//��ǰ���ѵ����޵㣬�����˻ص�ԭ��
            {
                if(man.platformPos == 0)//ֻ��һ��λ�ã��������˲���
                {
                    printf("Action----Shift2Sensors----Finish----%d\r\n", (int)man.platformPos);
                    
                    SML(M_SHIFT_FINISH, 1);
                    man.actPlatform = ACTION_NONE;
                }
                else//�˻ص�λ��0
                {
                    man.actPlatform = RETURN;

                    man.delayPlatformPos = 0;

                    printf("Action----Shift2Sensors++++Return2Sensors\r\n");

                    SML(M_SHIFT_FINISH, 1);
                    SML(M_RETURN_FINISH, 0);
                    man.waitMSignal = M_RETURN_FINISH;
                }
            }
            else//δ�����޵�
            {
                SML(M_SHIFT_FINISH, 0);
                                      
                if(man.platformPos == 0)//λ��0ֱ����λ
                {
                    //if(GML(X_SHIFT_O))
                    {
                        SYL(Y_POS, 1);
                        SYL(Y_SHIFT, 1);
                        
                        printf("Action----Shift2Sensors0----%d\r\n", (int)man.platformPos);
                        
                        man.platformState = 1;

                        man.delayPlatformPos++;
                    }
                }
                else//�˻ص�λ��0
                {
                    man.actPlatform = RETURN;

                    man.delayPlatformPos = 0;

                    printf("Action----Shift2Sensors++++Return2Sensors\r\n");

                    SML(M_SHIFT_FINISH, 1);
                    SML(M_RETURN_FINISH, 0);
                    man.waitMSignal = M_RETURN_FINISH;
                }
            }
        break;
        case 1:
            if(GXL(sensor))
            {     
                man.platformPos++;
                
                printf("Action----Shift2Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_SHIFT_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }    
}

void Return3Sensors(void)
{
    switch(man.platformState)
    {
        case 0://�Ȼ�λһ��

            SML(M_RETURN_FINISH, 0);

            if(GXL(X_SHIFT_L1) || GXL(X_SHIFT_O)) //��ԭ������λ��ֱ���ջض�λ����
                man.platformState = 2; 
            else //���м�λ����Ҫ�ȵ��˹���λ�����ٰѶ�λ�����ջ���
                man.platformState = 1;

            //if(GXL(X_SHIFT_L1))
            {
                SYL(Y_SHIFT, 0);
                //SYL(Y_SHIFT2, 1);

//                man.platformState = 1;
                
                man.delayPlatformPos = 0;

                printf("Action----Return3Sensors\r\n");
            }
        break;
        case 1:
            if(GXF(X_POS))
                man.platformState = 2;    
        break;
        case 2:
            SYL(Y_POS, 0);//�ٽ���λ�����ջ���

            man.platformState = 3;
        break;
        case 3:
            if(GXL(X_SHIFT_O))//ȷ����λ
            {
                man.platformPos = 0;

                printf("Action----Return3Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_RETURN_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void Return4Sensors(void)
{
    switch(man.platformState)
    {
        case 0://�Ȼ�λһ��

            SML(M_RETURN_FINISH, 0);

            //if(GML(X_SHIFT_L1))
            {
                SYL(Y_SHIFT, 0);

                man.platformState = 1;
                
                man.delayPlatformPos = 0;

                printf("Action----Return4Sensors\r\n");
            }
        break;
        case 1:
            if(!GXL(X_SHIFT_L2))
            {
                SYL(Y_POS, 0);//�Ƚ���λ�����ջ���

                man.platformState = 2;
            }
        break;
        case 2:
            if(GXL(X_SHIFT_O))//ȷ����λ
            {
                man.platformPos = 0;

                printf("Action----Return4Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_RETURN_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void Return2Sensors(void)
{
//#if MACHINE3SENSOR_SIM_2SENSOR 
//    uint8_t sensor = X_SHIFT_O;
//#else
//    uint8_t sensor = X_POS;
//#endif
    uint8_t sensor = X_SHIFT_O;

    switch(man.platformState)
    {
        case 0://�Ȼ�λһ��

            SML(M_RETURN_FINISH, 0);

            //if(GML(X_SHIFT_L1))
            {
                SYL(Y_SHIFT, 0);

                man.platformState = 1;

                man.delayPlatformPos = 0;

                printf("Action----Return2Sensors\r\n");
            }
        break;
        case 1:
            if(GXL(sensor))//ȷ����λ
            {
                man.platformPos = 0;
                
                printf("Action----Return2Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_RETURN_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void ShiftReturn2Sensors(void)
{
//#if MACHINE3SENSOR_SIM_2SENSOR 
//    uint8_t sensor0 = X_SHIFT_O;
//    uint8_t sensor1 = X_POS;
//#else
//    uint8_t sensor0 = X_POS;
//    uint8_t sensor1 = X_SHIFT_L1;
//#endif

    uint8_t sensor0 = X_SHIFT_O;
    uint8_t sensor1 = X_POS;

    switch(man.platformState)
    {
        case 0:
            SML(M_SHIFTRETURN_FINISH, 0);

            SYL(Y_POS, 1);
            SYL(Y_SHIFT, 1);
            
            printf("Action----ShiftReturn2Sensors0----%d\r\n", (int)man.platformPos);
            
            man.platformState = 1;
        break;
        case 1:
            if(GXL(sensor1))
            {
                man.platformState = 2;
            }
        break;

        case 2://�Ȼ�λһ��

            SYL(Y_SHIFT, 0);

            man.platformState = 3;

        break;
        case 3:
            if(GXL(sensor0))//ȷ����λ
            {
                printf("Action----ShiftReturn2Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_SHIFTRETURN_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}

void ShiftReturnShiftReturn2Sensors(void)
{
//#if MACHINE3SENSOR_SIM_2SENSOR 
//    uint8_t sensor0 = X_SHIFT_O;
//    uint8_t sensor1 = X_POS;
//#else
//    uint8_t sensor0 = X_POS;
//    uint8_t sensor1 = X_SHIFT_L1;
//#endif

    uint8_t sensor0 = X_SHIFT_O;
    uint8_t sensor1 = X_POS;

    switch(man.platformState)
    {
        case 0:
            SML(M_SHIFTRETURN2_FINISH, 0);
            
            SYL(Y_POS, 1);
            SYL(Y_SHIFT, 1);
            
            printf("Action----ShiftReturnShiftReturn2Sensors0----%d\r\n", (int)man.platformPos);
            
            man.platformState = 1;
        break;
        case 1:
            if(GXL(sensor1))
            {
                man.platformState = 2;
            }
        break;

        case 2:

            SYL(Y_SHIFT, 0);

            man.platformState = 3;

        break;
        case 3:
            if(GXL(sensor0))//ȷ����λ
            {
                man.platformState = 4;
            }
        break;

        case 4:
            SYL(Y_POS, 1);
            SYL(Y_SHIFT, 1);

            man.platformState = 5;
        break;
        case 5:
            if(GXL(sensor1))
            {
                man.platformState = 6;
            }
        break;

        case 6:

            SYL(Y_SHIFT, 0);

            man.platformState = 7;

        break;
        case 7:
            if(GXL(sensor0))//ȷ����λ
            {
                printf("Action----ShiftReturnShiftReturn2Sensors----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_SHIFTRETURN2_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        
        default:
        break;
    }
}




//
void ShiftProgram11(void)
{
    switch(man.platformState)
    {
        case 0:
            SML(M_SHIFT_FINISH, 0);

            SYL(Y_POS, 1);

            SYL(Y_SHIFT2, 0);
            SYL(Y_SHIFT, 1);

            printf("Action----ShiftProgram11----%d\r\n", (int)man.platformPos);
            
            man.platformState = 1;

            man.delayPlatformPos++;
        break;
        case 1:  
            man.platformPos++;
            
            printf("Action----ShiftProgram11----Finish----%d\r\n", (int)man.platformPos);
            
            SML(M_SHIFT_FINISH, 1);
            man.actPlatform = ACTION_NONE;

            man.platformState = 0;
        break;
        default:
        break;
    }
}

void ReturnProgram11(void)
{
    switch(man.platformState)
    {
        case 0://�Ȼ�λһ��

            SML(M_RETURN_FINISH, 0);

            //if(GML(X_SHIFT_L1))
            {
                SYL(Y_SHIFT, 0);
                //SYL(Y_SHIFT2, 1);

                man.platformState = 1;
                
                man.delayPlatformPos = 0;

                printf("Action----ReturnProgram11\r\n");
            }
        break;
        case 1:
            SYL(Y_POS, 0);//�Ƚ���λ�����ջ���

            man.platformState = 2;
        break;
        case 2:
            //if(GXL(X_SHIFT_O))//ȷ����λ
            {
                man.platformPos = 0;

                printf("Action----ReturnProgram11----Finish----%d\r\n", (int)man.platformPos);
                
                SML(M_RETURN_FINISH, 1);
                man.actPlatform = ACTION_NONE;

                man.platformState = 0;
            }
        break;
        default:
        break;
    }
}




