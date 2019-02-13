
#ifndef __MANAGEMENT_H_
#define __MANAGEMENT_H_

#include "project.h"

//如果用12V3传感器设备测试2传感器设备，则该宏设为1，否则设为0
#define MACHINE3SENSOR_SIM_2SENSOR 1

//按键
//#define KEY_NONE        0
//#define KEY_STARTSTOP   1
//#define KEY_AUX         2
//#define KEY_UPDOWN      3
//#define KEY_FRONTBACK   4
//#define KEY_FOOT        5

//记录气缸位置用
#define UPDOWN_MASK         0x01
#define FRONTBACK_MASK      0x02

void Key_Refresh(void);
void ProgramResetDefault(void);
void ProgramSelf(void);
void ProgramSelect(void);

void ProgramRead(void);
void Key_Distinguish(void);

void NextAction(void);

void Home_Check(void);
void Push_Check(void);
void Push2_Check(void);

void Project_Run(void);

void KeyLogic(void);
void KeyLogicBetweenDelay(void);


//项目模式
typedef enum
{
    PM_NONE = 0,
    PM_NORMAL = 1,
    PM_PROGRAM_SEL,
    PM_PROGRAM_SELF,
    PM_HEAD_CHECK,
    PM_PLATFORM_CHECK,
    PM_PARAMETER_SET,
}PROJECTMODE_TypeDef;


typedef struct
{
    //工作模式
    PROJECTMODE_TypeDef mode;

    //设备类型
    MachineType_TypeDef machineType;   //0:MACHINE_4SENSORS     1:MACHINE_2SENSORS     2:MACHINE_3SENSORS
    uint8_t sensorLevel; //1:12V,金属感应开关 ； 0:5V，光电开关

    //实际动作，印头和平台要分开控制
    ACT_TypeDef actHead;
    ACT_TypeDef actPlatform;

    //记录印头位置。
    //bit0: 1:表示上下气缸充气；0:表示上下气缸放气。  #define UPDOWN_MASK         0x01
    //bit1: 1:表示前后气缸充气；0:表示前后气缸放气。  #define FRONTBACK_MASK      0x02
    uint8_t headPos;
    uint8_t platformPos;//记录平台位置

    uint8_t headState;//印头动作流程中用的状态转移变量
    uint8_t platformState;//平台动作流程中用的状态转移变量

    uint8_t delayHeadPos;//调节延时参数时，只要启动BACK动作，延时参数修改的是吸油的延时，同理只要启动FRONT动作，延时参数修改的是印油的延时
    uint8_t delayPlatformPos;
    uint8_t delayAbsorb; //吸油延时
    uint8_t delayPrint[DELAY_PRINT_COUNT];  //印油延时

    uint8_t led;
    uint8_t segFlashTimes;//选择完程序后，数码管闪烁次数
    uint8_t segFlashCnt;
    uint8_t segStr[6];//数码管显示

    uint8_t programNum;//选择了第几个程序
    uint16_t programAddr;//程序在EEPROM中的地址
    uint16_t productOutput;//产量
    uint8_t productOutputOffset;//产量保存在EEPROM的产量基地址的偏移
    uint16_t productOutputAddr;

    ACTION_TypeDef program[MAX_ACTIONS_PER_PROJECT];//程序流程

    ACTION_TypeDef *pCurAction;//当前动作指针
    uint8_t actionCnt;//当前程序共有几个动作
    uint8_t actionPos;//当前执行到第几个动作
    uint8_t delay;//当前执行动作的延时

    //自编程
    uint8_t programSelfPos;
    ACTION_TypeDef *pCurProSelfAction;


    //内部参数设置
    InternalSetting_TypeDef internalSetting[INTERNALSETTING_COUNT];  //
    uint8_t intSetAuxCnt;//进入内部设置
    uint8_t intSetPos;






    uint16_t returnPosDelay;//shift动作时，需要延时一小段时间定位气缸才能收回来
    uint16_t returnNoneDelay;//shift动作时，定位气缸收回来后，平台左右的2个气缸不动作一段时间
    uint16_t shiftPosDelay;//shift动作时，需要延时一小段时间定位气缸才能出来
    uint16_t shiftStopDelay;//shift动作时，在快要停止时，SHIFT2气缸先反方向充气一段时间，减轻撞击作用力

    uint8_t returnPosDelayFactor;
    uint8_t returnNoneDelayFactor;
    uint8_t shiftPosDelayFactor;
    uint8_t shiftStopDelayFactor;

    uint8_t delayUnit; //延时最小单位
    
    uint8_t platformHomeFlag;

    uint8_t platformPushFlag;  //记录是否推了平台


    //自动或手动时，按键缓冲区
    uint8_t keyPress;
    uint8_t keyPressLast;

    //不同设备调用不同平台函数
    void (*Shift)(void);
    void (*Return)(void);
    void (*ShiftReturn)(void);
    void (*ShiftReturn2)(void);

    //芯片ID
    uint8_t *mcuID;
    uint8_t eepromMcuID[7];

}MAN_TypeDef;

extern MAN_TypeDef man;
extern const InternalSetting_TypeDef defaultInternalSetting[INTERNALSETTING_COUNT];

void NextPeroid(void);

#endif