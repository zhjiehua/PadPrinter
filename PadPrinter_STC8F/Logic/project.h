
#ifndef __PROJECT_H_
#define __PROJECT_H_

#include "common.h"

//#define PROJECTNUM  12
#define PROJECTNUM  11
#define MAX_ACTIONS_PER_PROJECT 40

#define DELAY_PRINT_COUNT 14  //自动情况下，印油延时的数量，这也意味着穿梭位置的数量

#define DELAY_MANUAL_FB  6 //手动情况下，前后动作的固定延时

#define INTERNALSETTING_COUNT                   11
#define INTERNALSETTING_DEFAULT                 0
#define INTERNALSETTING_DELAYUNIT               1
#define INTERNALSETTING_MACHINETYPE             2
#define INTERNALSETTING_PLATFORMSENSORLEVEL     3
#define INTERNALSETTING_HEADSENSORLEVEL         4
#define INTERNALSETTING_RETURNPOSDELAYFACTOR    5
#define INTERNALSETTING_SHIFTPOSDELAYFACTOR     6
#define INTERNALSETTING_RETURNNONEDELAYFACTOR   7
#define INTERNALSETTING_SHIFTSTOPDELAYFACTOR    8
#define INTERNALSETTING_RESTOREDALAY            9
#define INTERNALSETTING_FRONTBACKDALAY          10


#define MAX_ACTIONUNIT_DELAY    50  //调节延时的最大值

#define SIZEOF(x) sizeof(x)/sizeof(x[0])



//设备内部参数设置
typedef struct
{
    uint8_t val;//值
    
    uint8_t min;//最小值
    uint8_t max;//最大值
}__InternalSetting_TypeDef;

typedef struct
{
    uint8_t val;//值
}InternalSetting_TypeDef;

//设备类型
typedef enum
{
    MACHINE_4SENSORS = 0,
    MACHINE_2SENSORS = 1,
    MACHINE_3SENSORS = 2,
    MACHINE_0SENSORS = 3,
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
    SHIFTRETURN = 11,
    SHIFTRETURN2 = 12,
}ACT_TypeDef;

typedef struct
{
    uint8_t flag;
    ACT_TypeDef act;
}ACTION_TypeDef;

typedef enum
{
    READY = 0,
    RUNNING,
    PAUSE,
    MANUAL,
}PROGRAMMODE_TypeDef;



//eeprom地址
#define EEPROM_ADDR_BASE                0x0000
#define EEPROM_ADDR_DEFAULT             EEPROM_ADDR_BASE
#define EEPROM_ADDR_PROGRAM_NUM         EEPROM_ADDR_DEFAULT+1

#define EEPROM_ADDR_MACHINETYPE_DEFAULT         EEPROM_ADDR_PROGRAM_NUM+1     //恢复出厂设置不更新
#define EEPROM_ADDR_PLATFORMSENSORLEVEL_DEFAULT EEPROM_ADDR_MACHINETYPE_DEFAULT+1   //恢复出厂设置不更新
#define EEPROM_ADDR_HEADSENSORLEVEL_DEFAULT     EEPROM_ADDR_PLATFORMSENSORLEVEL_DEFAULT+1   //恢复出厂设置不更新

#define EEPROM_ADDR_MACHINETYPE                 EEPROM_ADDR_HEADSENSORLEVEL_DEFAULT+1
#define EEPROM_ADDR_PLATFORMSENSORLEVEL         EEPROM_ADDR_MACHINETYPE+1
#define EEPROM_ADDR_HEADSENSORLEVEL             EEPROM_ADDR_PLATFORMSENSORLEVEL+1
#define EEPROM_ADDR_PRODUCTOUTPUTOFFSET         EEPROM_ADDR_HEADSENSORLEVEL+1
#define EEPROM_ADDR_DELAYUNIT                   EEPROM_ADDR_PRODUCTOUTPUTOFFSET+1
#define EEPROM_ADDR_RETURNPOSDELAYFACTOR        EEPROM_ADDR_DELAYUNIT+1
#define EEPROM_ADDR_SHIFTPOSDELAYFACTOR         EEPROM_ADDR_RETURNPOSDELAYFACTOR+1
#define EEPROM_ADDR_RETURNNONEDELAYFACTOR       EEPROM_ADDR_SHIFTPOSDELAYFACTOR+1
#define EEPROM_ADDR_SHIFTSTOPDELAYFACTOR        EEPROM_ADDR_RETURNNONEDELAYFACTOR+1
#define EEPROM_ADDR_RESTOREDELAY        EEPROM_ADDR_SHIFTSTOPDELAYFACTOR+1
#define EEPROM_ADDR_FRONTBACKDELAY      EEPROM_ADDR_RESTOREDELAY+1

#define EEPROM_ADDR_DELAY               EEPROM_ADDR_FRONTBACKDELAY+1

#define EEPROM_ADDR_MCUID               EEPROM_ADDR_DELAY+15    //恢复出厂设置不更新

#define EEPROM_ADDR_PROGRAM0            EEPROM_ADDR_MCUID+8
#define EEPROM_ADDR_DATAOFPRODUCTION    EEPROM_ADDR_PROGRAM0+MAX_ACTIONS_PER_PROJECT*sizeof(ACTION_TypeDef)  //出厂日期 
#define EEPROM_ADDR_DATAOFLASTREPAIR    EEPROM_ADDR_DATAOFPRODUCTION+3  //上次维修日期
#define EEPROM_ADDR_END                 EEPROM_ADDR_DATAOFLASTREPAIR+3

#define EEPROM_ADDR_PRODUCTOUTPUT       EEPROM_ADDR_END+1




void Key_Refresh(void);
void Program0Save(void);
void ProgramResetDefault(void);
void Project_Run(void);


void Normal(void);

#endif