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

MAN_TypeDef man;


//固定设备类型
#define MACHINE_FIX  0

//设备类型
//#define MACHINE_TYPE_3SENSORS_12V
#define MACHINE_TYPE_2SENSORS_5V
//#define MACHINE_TYPE_2SENSORS_12V

#ifdef MACHINE_TYPE_3SENSORS_12V

const InternalSetting_TypeDef defaultInternalSetting[INTERNALSETTING_COUNT] = 
{
  {0, 0, 1}, //恢复出厂设置   1:恢复
  {40, 1, 99}, //调节动作延时的最小延时单位
  {2, 0, 2}, //设备识别  0:4sensors 1:2sensors 2:3sensors
  {1, 0, 1}, //传感器电平 0:5V 1:12Vs
  {0, 0, 30}, //穿梭returnPosDelay延时
  {0, 0, 50}, //穿梭shiftPosDelay延时
  {0, 0, 30}, //穿梭returnNoneDelay延时
  {1, 0, 50}, //穿梭shiftStopDelay延时
};

#endif
#ifdef MACHINE_TYPE_2SENSORS_5V

const InternalSetting_TypeDef defaultInternalSetting[INTERNALSETTING_COUNT] = 
{
  {0, 0, 1}, //恢复出厂设置   1:恢复
  {40, 1, 99}, //调节动作延时的最小延时单位
  {1, 0, 2}, //设备识别  0:4sensors 1:2sensors 2:3sensors
  {0, 0, 1}, //传感器电平 0:5V 1:12Vs
  {0, 0, 30}, //穿梭returnPosDelay延时
  {0, 0, 50}, //穿梭shiftPosDelay延时
  {0, 0, 30}, //穿梭returnNoneDelay延时
  {1, 0, 50}, //穿梭shiftStopDelay延时
};

#endif
#ifdef MACHINE_TYPE_2SENSORS_12V

const InternalSetting_TypeDef defaultInternalSetting[INTERNALSETTING_COUNT] = 
{
  {0, 0, 1}, //恢复出厂设置   1:恢复
  {40, 1, 99}, //调节动作延时的最小延时单位
  {1, 0, 2}, //设备识别  0:4sensors 1:2sensors 2:3sensors
  {1, 0, 1}, //传感器电平 0:5V 1:12Vs
  {0, 0, 30}, //穿梭returnPosDelay延时
  {0, 0, 50}, //穿梭shiftPosDelay延时
  {0, 0, 30}, //穿梭returnNoneDelay延时
  {1, 0, 50}, //穿梭shiftStopDelay延时
};
#endif


//按键刷新
void Key_Refresh(void)
{
    uint8_t key = TM1638_ReadKey(0);
    uint8_t i;

    for(i=0;i<8;i++)
        SML(i, 0);

    if(key)
    {
        if(man.mode != PM_NORMAL)
            Delay10ms();

        key = TM1638_ReadKey(0);
        if(key)
        {
            SML(key-1, 1);
            //SML(TM1638_ReadKey(0)-1, 1);
        }
//        sprintf((char*)man.segStr, "%02d", (int)key);
//        TM1638_SendData(0, man.segStr);
//        printf("key = %02d\r\n", (int)key);
    }
}

void NextPeroid(void)
{
    man.productOutput++;

    AT24CXX_WriteLenByte(man.productOutputAddr, man.productOutput, 2);
    
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
