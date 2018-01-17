
#ifndef __PROJECT_H_
#define __PROJECT_H_

#include "stm8s.h"

#define PROJECTNUM  10
#define MAX_ACTIONS_PER_PROJECT 40

//时间默认参数
#define RETURN_FILTER_TIME      300
#define RETURN_POS_TIME         200
#define PUSH_TIME               1000
#define ACTIONUNIT_DELAY        40 //200

#define MAX_ACTIONUNIT_DELAY    30

#define SIZEOF(x) sizeof(x)/sizeof(x[0])

#define PROCESS0_SIZE   1
#define PROCESS1_SIZE   11
#define PROCESS2_SIZE   11
#define PROCESS3_SIZE   8
#define PROCESS4_SIZE   8
#define PROCESS5_SIZE   17
#define PROCESS6_SIZE   17
#define PROCESS7_SIZE   23
#define PROCESS8_SIZE   23
#define PROCESS9_SIZE   9

#define EEPROM_ADDR_BASE                0x0000
#define EEPROM_ADDR_DEFAULT             EEPROM_ADDR_BASE
#define EEPROM_ADDR_PROGRAM_NUM         EEPROM_ADDR_DEFAULT+1
#define EEPROM_ADDR_PRODUCTOUTPUT       EEPROM_ADDR_PROGRAM_NUM+1

#define EEPROM_ADDR_RETURNFILTERTIME    EEPROM_ADDR_PRODUCTOUTPUT+2
#define EEPROM_ADDR_RETURNPOSTIME       EEPROM_ADDR_RETURNFILTERTIME+2
#define EEPROM_ADDR_PUSHTIME            EEPROM_ADDR_RETURNPOSTIME+2
#define EEPROM_ADDR_ACTIONUNITDELAY     EEPROM_ADDR_PUSHTIME+2

#define EEPROM_ADDR_PROGRAM1            EEPROM_ADDR_ACTIONUNITDELAY+20//加大距离
//#define EEPROM_ADDR_PROGRAM2            EEPROM_ADDR_PROGRAM1+PROCESS1_SIZE*3
//#define EEPROM_ADDR_PROGRAM3            EEPROM_ADDR_PROGRAM2+PROCESS2_SIZE*3
//#define EEPROM_ADDR_PROGRAM4            EEPROM_ADDR_PROGRAM3+PROCESS3_SIZE*3
//#define EEPROM_ADDR_PROGRAM5            EEPROM_ADDR_PROGRAM4+PROCESS4_SIZE*3
//#define EEPROM_ADDR_PROGRAM6            EEPROM_ADDR_PROGRAM5+PROCESS5_SIZE*3
//#define EEPROM_ADDR_PROGRAM7            EEPROM_ADDR_PROGRAM6+PROCESS6_SIZE*3
//#define EEPROM_ADDR_PROGRAM8            EEPROM_ADDR_PROGRAM7+PROCESS7_SIZE*3
//#define EEPROM_ADDR_PROGRAM9            EEPROM_ADDR_PROGRAM8+PROCESS8_SIZE*3
//#define EEPROM_ADDR_PROGRAM0            EEPROM_ADDR_PROGRAM9+PROCESS9_SIZE*3
#define EEPROM_ADDR_PROGRAM2            EEPROM_ADDR_PROGRAM1+sizeof(process1)
#define EEPROM_ADDR_PROGRAM3            EEPROM_ADDR_PROGRAM2+sizeof(process2)
#define EEPROM_ADDR_PROGRAM4            EEPROM_ADDR_PROGRAM3+sizeof(process3)
#define EEPROM_ADDR_PROGRAM5            EEPROM_ADDR_PROGRAM4+sizeof(process4)
#define EEPROM_ADDR_PROGRAM6            EEPROM_ADDR_PROGRAM5+sizeof(process5)
#define EEPROM_ADDR_PROGRAM7            EEPROM_ADDR_PROGRAM6+sizeof(process6)
#define EEPROM_ADDR_PROGRAM8            EEPROM_ADDR_PROGRAM7+sizeof(process7)
#define EEPROM_ADDR_PROGRAM9            EEPROM_ADDR_PROGRAM8+sizeof(process8)
#define EEPROM_ADDR_PROGRAM0            EEPROM_ADDR_PROGRAM9+sizeof(process9)+5
#define EEPROM_ADDR_END                 EEPROM_ADDR_PROGRAM0+MAX_ACTIONS_PER_PROJECT*3

//设备类型
typedef enum
{
    MACHINE_4SENSORS = 0,
    MACHINE_2SENSORS = 1,
}MachineType_TypeDef;

//程序动作
typedef enum
{
    ACTION_NONE = 0,
    ABSORB_DOWN = 1,
    ABSORB_UP = 2,
    ABSORB = 3,
    FRONT = 4,
    BACK = 5,
    PRINT_DOWN = 6,
    PRINT_UP = 7,
    PRINT = 8,
    SHIFT = 9,
    RETURN = 10,
    PUSH = 11,
    PUSHHOME = 12,
    HOME = 13,
}ACT_TypeDef;

typedef struct
{
    uint8_t flag;
    ACT_TypeDef act;
    uint8_t delay;
}ACTION_TypeDef;

//项目模式
typedef enum
{
    NONE = 0,
    NORMAL = 1,
    PROGRAM_SEL,
    PROGRAM_SELF,
}PROJECTMODE_TypeDef;

typedef enum
{
    READY = 0,
    RUNNING,
    PAUSE,
    MANUAL,
}PROGRAMMODE_TypeDef;

//项目结构体
typedef struct
{
    //工作模式
    //0：正常工作
    //1：程序选择
    //2：自编程
    PROJECTMODE_TypeDef mode;
    
    uint16_t programAddr;//程序在EEPROM中的地址

    ACTION_TypeDef program[MAX_ACTIONS_PER_PROJECT];//程序流程

    ACTION_TypeDef *pCurAction;//当前动作指针
    uint8_t actionCnt;//当前程序共有几个动作
    uint8_t actionPos;//当前执行到第几个动作
    uint8_t delay;//当前执行动作的延时
    
    //ACT_TypeDef actionTemp;//临时记录动作，主要用于移位到极限时，需要先执行RETURN动作
    
    uint8_t segFlashTimes;//选择完程序后，数码管闪烁次数
    uint8_t segFlashCnt;
    uint8_t segStr[6];//数码管显示
    
    //记录印头位置。
    //bit0: 1:表示上下气缸充气；0:表示上下气缸放气。 
    //bit1: 1:表示前后气缸充气；0:表示前后气缸放气。 
    uint8_t printHeadPos;
    uint8_t platformPos;//记录平台位置
    
    uint8_t printHeadPosBackUp;//手动时用于记录自动的位置
    uint8_t platformPosBackUp;
    
    uint8_t programNum;//选择了第几个程序
    uint16_t productOutput;//产量
    
    //平台返回时需要检测平台原点传感器，
    //该传感器在经过中间挡片时会触发程序，
    //所以需要在触发时延时一段时间检测是否只是经过中间挡片，
    //如果延时后传感器还是被挡住，则说明已经回到原点。
    uint16_t returnFilterTime; 
    uint16_t returnPosTime;//需要延时一小段时间定位气缸才能收回来
    uint16_t pushTime;//PUSH动作的延时时间
    
    uint16_t actionUnitDelay;//动作间延时的最小单位
    
    uint8_t intSetAuxCnt;//进入内部设置
    uint8_t intSetPos;
    uint8_t intSetVal;
    
    uint8_t printHeadState;//印头动作流程中用的状态转移变量
    uint8_t platformState;//平台动作流程中用的状态转移变量
    
    PROGRAMMODE_TypeDef programMode;//程序模式
    PROGRAMMODE_TypeDef programModeFlag;//程序模式
    
    uint8_t platformHomeFlag;
    
    uint8_t programSelfPos;
    ACTION_TypeDef *pCurProSelfAction;
    
    uint8_t led;
    
    MachineType_TypeDef machineType;
    uint8_t platformPushFlag;
    
}PROJECT_TypeDef;

extern PROJECT_TypeDef project;

void Key_Refresh(void);
void Program0Save(void);
void ProgramResetDefault(void);
void Project_Run(void);

#endif