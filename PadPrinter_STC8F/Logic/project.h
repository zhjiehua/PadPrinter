
#ifndef __PROJECT_H_
#define __PROJECT_H_

#include "common.h"

#define ARRAY_KEYBOARD 1 //1���þ�����̣�0����1668�ļ���

//#define PROJECTNUM  12
#define PROJECTNUM  11
#define MAX_ACTIONS_PER_PROJECT 40

#define DELAY_PRINT_COUNT 14  //�Զ�����£�ӡ����ʱ����������Ҳ��ζ�Ŵ���λ�õ�����

#define DELAY_MANUAL_FB  6 //�ֶ�����£�ǰ�����Ĺ̶���ʱ

#define INTERNALSETTING_COUNT                   8
#define INTERNALSETTING_DEFAULT                 0
#define INTERNALSETTING_DELAYUNIT               1
#define INTERNALSETTING_MACHINETYPE             2
#define INTERNALSETTING_SENSORLEVEL             3
#define INTERNALSETTING_RETURNPOSDELAYFACTOR    4
#define INTERNALSETTING_SHIFTPOSDELAYFACTOR     5
#define INTERNALSETTING_RETURNNONEDELAYFACTOR   6
#define INTERNALSETTING_SHIFTSTOPDELAYFACTOR    7


#define MAX_ACTIONUNIT_DELAY    50  //������ʱ�����ֵ

#define SIZEOF(x) sizeof(x)/sizeof(x[0])



//�豸�ڲ���������
typedef struct
{
    uint8_t val;//ֵ
    
    uint8_t min;//��Сֵ
    uint8_t max;//���ֵ
}InternalSetting_TypeDef;

//�豸����
typedef enum
{
    MACHINE_4SENSORS = 0,
    MACHINE_2SENSORS = 1,
    MACHINE_3SENSORS = 2,
}MachineType_TypeDef;

//������
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



//eeprom��ַ
#define EEPROM_ADDR_BASE                0x0000
#define EEPROM_ADDR_DEFAULT             EEPROM_ADDR_BASE
#define EEPROM_ADDR_PROGRAM_NUM         EEPROM_ADDR_DEFAULT+1

#define EEPROM_ADDR_MACHINETYPE_DEFAULT       EEPROM_ADDR_PROGRAM_NUM+1     //�ָ��������ò�����
#define EEPROM_ADDR_SENSORLEVEL_DEFAULT       EEPROM_ADDR_MACHINETYPE_DEFAULT+1   //�ָ��������ò�����

#define EEPROM_ADDR_MACHINETYPE         EEPROM_ADDR_SENSORLEVEL_DEFAULT+1
#define EEPROM_ADDR_SENSORLEVEL         EEPROM_ADDR_MACHINETYPE+1
#define EEPROM_ADDR_PRODUCTOUTPUTOFFSET EEPROM_ADDR_SENSORLEVEL+1
#define EEPROM_ADDR_DELAYUNIT           EEPROM_ADDR_PRODUCTOUTPUTOFFSET+1
#define EEPROM_ADDR_RETURNPOSDELAYFACTOR       EEPROM_ADDR_DELAYUNIT+1
#define EEPROM_ADDR_SHIFTPOSDELAYFACTOR        EEPROM_ADDR_RETURNPOSDELAYFACTOR+1
#define EEPROM_ADDR_RETURNNONEDELAYFACTOR      EEPROM_ADDR_SHIFTPOSDELAYFACTOR+1
#define EEPROM_ADDR_SHIFTSTOPDELAYFACTOR       EEPROM_ADDR_RETURNNONEDELAYFACTOR+1

#define EEPROM_ADDR_DELAY               EEPROM_ADDR_SHIFTSTOPDELAYFACTOR+1

#define EEPROM_ADDR_MCUID               EEPROM_ADDR_DELAY+15    //�ָ��������ò�����

//#define EEPROM_ADDR_PROGRAM1            EEPROM_ADDR_MCUID+10//�Ӵ����
//#define EEPROM_ADDR_PROGRAM2            EEPROM_ADDR_PROGRAM1+sizeof(process1)
//#define EEPROM_ADDR_PROGRAM3            EEPROM_ADDR_PROGRAM2+sizeof(process2)
//#define EEPROM_ADDR_PROGRAM4            EEPROM_ADDR_PROGRAM3+sizeof(process3)
//#define EEPROM_ADDR_PROGRAM5            EEPROM_ADDR_PROGRAM4+sizeof(process4)
//#define EEPROM_ADDR_PROGRAM6            EEPROM_ADDR_PROGRAM5+sizeof(process5)
//#define EEPROM_ADDR_PROGRAM7            EEPROM_ADDR_PROGRAM6+sizeof(process6)
//#define EEPROM_ADDR_PROGRAM8            EEPROM_ADDR_PROGRAM7+sizeof(process7)
//#define EEPROM_ADDR_PROGRAM9            EEPROM_ADDR_PROGRAM8+sizeof(process8)
//#define EEPROM_ADDR_PROGRAM10           EEPROM_ADDR_PROGRAM9+sizeof(process9)
//#define EEPROM_ADDR_PROGRAM11           EEPROM_ADDR_PROGRAM10+sizeof(process10)
//#define EEPROM_ADDR_PROGRAM0            EEPROM_ADDR_PROGRAM11+sizeof(process11)+5
//#define EEPROM_ADDR_END                 EEPROM_ADDR_PROGRAM0+MAX_ACTIONS_PER_PROJECT*sizeof(ACTION_TypeDef)

#define EEPROM_ADDR_PROGRAM0            EEPROM_ADDR_MCUID+10
#define EEPROM_ADDR_END                 EEPROM_ADDR_PROGRAM0+MAX_ACTIONS_PER_PROJECT*sizeof(ACTION_TypeDef)

#define EEPROM_ADDR_PRODUCTOUTPUT       EEPROM_ADDR_END+1




void Key_Refresh(void);
void Program0Save(void);
void ProgramResetDefault(void);
void Project_Run(void);


void Normal(void);

#endif