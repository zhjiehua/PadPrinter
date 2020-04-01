#include "uart.h"
#include "timer.h"
#include "io.h"

#include "management.h"
#include "misc.h"

#include <string.h>
//-----------------------------------------------
#include "tm1638.h"
#include "io.h"
#include "24cxx.h"
#include "simplc_io.h"
#include "simplc_timer.h"

#include "project.h"
#include "misc.h"

/* main program */
int main(void)
{
    uint16_t cnt = 0;

    WDT_CONTR = 0x3C;

    //�����ֶ���ʼ��ȫ�ֱ�����51��������û���ϵ��Զ���ʼ��ȫ�ֱ�������    
    memset((char*)&man, 0, sizeof(man));

    WDT_CONTR = 0x3C;

    //=======================================================
    SIMPLC_IO_Init();
    SIMPLC_Timer_Init();
    SIMPLC_Timer_Create(0, 1000, 0);//ACTIN DELAY
    SIMPLC_Timer_Create(1, 100, 0);//KEY DELAY FAST SLOW
    SIMPLC_Timer_Create(2, 100, 0);//KEY BEEP
    SIMPLC_Timer_Create(3, 100, 0);//OUTPUT FLASH
    SIMPLC_Timer_Create(4, 100, 0);//ACTION SHIFT
    SIMPLC_Timer_Create(5, 100, 0);//KEY FOOT FILTER

    Uart_Init();//���ڳ�ʼ��
  
	Timer3_Init();   //��������
    
    AT24CXX_Init();
    
    WDT_CONTR = 0x3C;

    TM1638_Init();
    TM1638_SendData(0, "88888");
    
    EA = 1;//����ȫ���ж�
    printf("\r\nHello PadPrinter!\r\n");

    //��ʱ200mS�鿴�������ʾ�Ƿ�����
    TS(1, 200);
    while(!TG(1)) WDT_CONTR = 0x3C;

    EEPROM_Check();

    //��������һ��
    Timer3_Cmd(1);
    TS(0, 300);
    while(!TG(0))
        WDT_CONTR = 0x3C;
    Timer3_Cmd(0);
    

    SML(M_POWERON, 1);
    SML(M_HOME_FINISH, 1);

    SML(M_ABSORBDOWN_FINISH, 1);
    SML(M_ABSORBUP_FINISH, 1);
    SML(M_ABSORB_FINISH, 1);
    SML(M_PRINTDOWN_FINISH, 1);
    SML(M_PRINTUP_FINISH, 1);
    SML(M_PRINT_FINISH, 1);
    SML(M_FRONT_FINISH, 1);
    SML(M_BACK_FINISH, 1);
    SML(M_SHIFT_FINISH, 1);
    SML(M_RETURN_FINISH, 1);
    SML(M_SHIFTRETURN_FINISH, 1);
    SML(M_SHIFTRETURN2_FINISH, 1);

    TS(1, 100);
    
    SYL(Y_SHIFT2, 1);
    SYL(Y_SHIFT, 0);

    man.waitMSignal = M_WAITMSIGNAL_NONE;
    man.runMode = RM_MANNUAL;

    WDT_CONTR = 0x3C;//�����Ź������ʱ��1.1377s

	while (1)
	{
        //ι��
		WDT_CONTR = 0x3C;
        
        //ˢ�°���
        Key_Refresh();
        Key_Sound();//��������

        WDT_CONTR = 0x3C;

        //������
//        if(man.mode == PM_NORMAL)
//        {
//            static uint8_t i = 0;
//            i++;
//            if(i > 5)
//            {
//                i = 0;
//                if(GML(M_KEY_FOOT))
//                    SML(M_KEY_FOOT, 0);
//                else
//                    SML(M_KEY_FOOT, 1);
//            }
//        }

        //ˢ��IO��
		SIMPLC_IO_Refresh();
        
        WDT_CONTR = 0x3C;

        //ִ�й���
        Project_Run();
 
        WDT_CONTR = 0x3C;

        UartDataParse();

        //LEDM = !LEDM;

//        Delay100ms();
//        WDT_CONTR = 0x3C;
//        Delay100ms();
//        WDT_CONTR = 0x3C;
//        Delay100ms();
//        WDT_CONTR = 0x3C;
//        Delay100ms();
//        WDT_CONTR = 0x3C;
//        Delay100ms();
//        WDT_CONTR = 0x3C;
//        Delay100ms();
//
//        printf("\r\nPadPrinter is running!\r\n");
    }
    return 0;
}
