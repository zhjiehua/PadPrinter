#include "management.h"
#include "misc.h"
#include "actions.h"
#include "project.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"

MAN_TypeDef man;


//固定设备类型
#define MACHINE_FIX  0

//设备类型，只能选择一种设备类型
#define MACHINETYPE_0SENSORS   0
#define MACHINETYPE_2SENSORS   1
#define MACHINETYPE_3SENSORS   0
#define MACHINETYPE_4SENSORS   0

//印头传感器类型
#define HEADSENSORSTYPE_NORMALCLOSE   1 //常闭，5V
#define HEADSENSORSTYPE_NORMALOPEN    0 //常开，12V

//平台传感器类型
#define PLATFORMSENSORSTYPE_NORMALCLOSE   1 //常闭，5V
#define PLATFORMSENSORSTYPE_NORMALOPEN    0 //常开，12V

const __InternalSetting_TypeDef defaultInternalSetting[INTERNALSETTING_COUNT] = 
{
  {0, 0, 1}, //恢复出厂设置   1:恢复
  {40, 1, 99}, //调节动作延时的最小延时单位

//设备识别  0:4sensors 1:2sensors 2:3sensors 3:0sensors
#if MACHINETYPE_0SENSORS
    {3, 0, 3},
#elif MACHINETYPE_2SENSORS
    {1, 0, 3},
#elif MACHINETYPE_3SENSORS
    {2, 0, 3},
#elif MACHINETYPE_4SENSORS
    {0, 0, 3},
#endif

    //平台传感器电平 0:5V 1:12V
#if PLATFORMSENSORSTYPE_NORMALCLOSE
    {0, 0, 1},
#elif PLATFORMSENSORSTYPE_NORMALOPEN
    {1, 0, 1},
#endif

    //印头传感器电平 0:5V 1:12V
#if HEADSENSORSTYPE_NORMALCLOSE
    {0, 0, 1},
#elif HEADSENSORSTYPE_NORMALOPEN
    {1, 0, 1},
#endif

  {0, 0, 30}, //穿梭returnPosDelay延时
  {0, 0, 50}, //穿梭shiftPosDelay延时
  {0, 0, 30}, //穿梭returnNoneDelay延时
  {1, 0, 50}, //穿梭shiftStopDelay延时
  {8, 0, 30}, //手动前后动作时，在前的过程中按开始，延时一段时间再RESTORE
  {8, 0, 30}, //手动前后动作的延时
};


//按键刷新
void Key_Refresh(void)
{
    uint8_t key = TM1638_ReadKey(0);
    uint8_t i;
    static uint8_t FootKeyPressFlag = 0;

    for(i=0;i<9;i++)
        SML(i, 0);
    SML(M_KEY_FOOT_AFTERFILTER, 0);

#if 1
    if(key)
    {
        if(man.mode != PM_NORMAL)
        {
            Delay10ms();
            Delay10ms();
        }

        key = TM1638_ReadKey(0);
        if(key)
        {
            if(key <= KEY_SW8)
            {
                SML(key-1, 1);

//                if(GML(M_KEY_FOOT))
//                    SML(M_KEY_FOOT_AFTERFILTER, 1);

                if(!FootKeyPressFlag && GMR(M_KEY_FOOT))
				{
					//TS(5, 10);
                    TS(5, 50);
					FootKeyPressFlag = 1;				
				}
				
				if(FootKeyPressFlag && TG(5))
				{
					if(GML(M_KEY_FOOT))
					{
						SML(M_KEY_FOOT_AFTERFILTER, 1);	
					}
					
					SIMPLC_Timer_ResetTimeOutFlag(5);
					SIMPLC_Timer_Stop(5);
					FootKeyPressFlag = 0;					
				}	
            }
            else
            {
                SML(M_KEY_AUX, 1);
                SML(M_KEY_SW8, 1);
            }
        }
    }
    else if(FootKeyPressFlag && TG(5))
	{
		FootKeyPressFlag = 0;
		SIMPLC_Timer_ResetTimeOutFlag(5);	
	}
#else //使用模拟按键测试程序
    
    if(man.mode == PM_NORMAL)
    {
        key = KeyRandom();
        //key = KEY_FOOT;

        if(key <= KEY_SW8)
        {
            if(key-1 != M_KEY_FRONTBACK)
                SML(key-1, 1);
    
            if(GML(M_KEY_FOOT))
    		{
    			SML(M_KEY_FOOT_AFTERFILTER, 1);	
    		}
        }
//        else
//        {
//            SML(M_KEY_AUX, 1);
//            SML(M_KEY_SW8, 1);
//        }
    }
#endif

    K_COL1 = 1;
    K_COL2 = 1;
    K_COL3 = 1;
    K_COL4 = 0;
    K_ROW1 = 1;
    K_ROW2 = 1;
    Delay10us();
    if(K_ROW2 == 0) SML(M_KEY_SW8, 1);
    else SML(M_KEY_SW8, 0);
}

void NextPeroid(void)
{
    uint32_t temp;

    man.productOutput++;
    man.productOutputAbsoluteSum++;

    //显示只有5个数码管
    if(man.productOutput > 99999)
    {
        man.productOutput = 0;
        //printf("\r\n>>>>>>>>>>man.productOutput is overflow!!!!\r\n\r\n");
    }

    if(man.productOutputAbsoluteSum >= 1000)
    {
        man.productOutputAbsoluteSum = 0;

        printf("\r\n>>>>>>>>>>man.productOutputAbsoluteSum is overflow!!!!\r\n\r\n");

        man.productOutputOffset++;
        if(man.productOutputOffset >= (EE_TYPE-EEPROM_ADDR_PRODUCTOUTPUT)/6-1)  //(511-118)/6-1 = 64次
            man.productOutputOffset = 0;
        man.productOutputAddr = EEPROM_ADDR_PRODUCTOUTPUT + man.productOutputOffset*6; 

        AT24CXX_WriteOneByte(EEPROM_ADDR_PRODUCTOUTPUTOFFSET, man.productOutputOffset);         
    }

    AT24CXX_WriteLenByte(man.productOutputAddr, man.productOutput, 4);
    AT24CXX_WriteLenByte(man.productOutputAddr+4, man.productOutputAbsoluteSum, 2);

    //保存修改过的延时参数
    if(GML(M_SAVE_DELAY))
    {   
        uint8_t i;

        printf("Save the delay!\r\n");
        AT24CXX_WriteOneByte(EEPROM_ADDR_DELAY, man.delayAbsorb);
        for(i=0;i<DELAY_PRINT_COUNT;i++)
        {
            AT24CXX_WriteOneByte(EEPROM_ADDR_DELAY+1+i, man.delayPrint[i]);    
        }

        SML(M_SAVE_DELAY, 0);
    }
    
    man.actionPos = 0;
    
    printf("##################Next Peroid\r\n");

    SML(M_ACTIONHEAD_FINISH, 0);
    SML(M_ACTIONPLATFORM_FINISH, 0);
}
