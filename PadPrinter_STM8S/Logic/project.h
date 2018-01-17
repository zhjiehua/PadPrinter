
#ifndef __PROJECT_H_
#define __PROJECT_H_

#include "stm8s.h"

#define PROJECTNUM  10
#define MAX_ACTIONS_PER_PROJECT 40

//ʱ��Ĭ�ϲ���
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

#define EEPROM_ADDR_PROGRAM1            EEPROM_ADDR_ACTIONUNITDELAY+20//�Ӵ����
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

//�豸����
typedef enum
{
    MACHINE_4SENSORS = 0,
    MACHINE_2SENSORS = 1,
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

//��Ŀģʽ
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

//��Ŀ�ṹ��
typedef struct
{
    //����ģʽ
    //0����������
    //1������ѡ��
    //2���Ա��
    PROJECTMODE_TypeDef mode;
    
    uint16_t programAddr;//������EEPROM�еĵ�ַ

    ACTION_TypeDef program[MAX_ACTIONS_PER_PROJECT];//��������

    ACTION_TypeDef *pCurAction;//��ǰ����ָ��
    uint8_t actionCnt;//��ǰ�����м�������
    uint8_t actionPos;//��ǰִ�е��ڼ�������
    uint8_t delay;//��ǰִ�ж�������ʱ
    
    //ACT_TypeDef actionTemp;//��ʱ��¼��������Ҫ������λ������ʱ����Ҫ��ִ��RETURN����
    
    uint8_t segFlashTimes;//ѡ���������������˸����
    uint8_t segFlashCnt;
    uint8_t segStr[6];//�������ʾ
    
    //��¼ӡͷλ�á�
    //bit0: 1:��ʾ�������׳�����0:��ʾ�������׷����� 
    //bit1: 1:��ʾǰ�����׳�����0:��ʾǰ�����׷����� 
    uint8_t printHeadPos;
    uint8_t platformPos;//��¼ƽ̨λ��
    
    uint8_t printHeadPosBackUp;//�ֶ�ʱ���ڼ�¼�Զ���λ��
    uint8_t platformPosBackUp;
    
    uint8_t programNum;//ѡ���˵ڼ�������
    uint16_t productOutput;//����
    
    //ƽ̨����ʱ��Ҫ���ƽ̨ԭ�㴫������
    //�ô������ھ����м䵲Ƭʱ�ᴥ������
    //������Ҫ�ڴ���ʱ��ʱһ��ʱ�����Ƿ�ֻ�Ǿ����м䵲Ƭ��
    //�����ʱ�󴫸������Ǳ���ס����˵���Ѿ��ص�ԭ�㡣
    uint16_t returnFilterTime; 
    uint16_t returnPosTime;//��Ҫ��ʱһС��ʱ�䶨λ���ײ����ջ���
    uint16_t pushTime;//PUSH��������ʱʱ��
    
    uint16_t actionUnitDelay;//��������ʱ����С��λ
    
    uint8_t intSetAuxCnt;//�����ڲ�����
    uint8_t intSetPos;
    uint8_t intSetVal;
    
    uint8_t printHeadState;//ӡͷ�����������õ�״̬ת�Ʊ���
    uint8_t platformState;//ƽ̨�����������õ�״̬ת�Ʊ���
    
    PROGRAMMODE_TypeDef programMode;//����ģʽ
    PROGRAMMODE_TypeDef programModeFlag;//����ģʽ
    
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