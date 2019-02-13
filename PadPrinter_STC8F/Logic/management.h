
#ifndef __MANAGEMENT_H_
#define __MANAGEMENT_H_

#include "project.h"

//�����12V3�������豸����2�������豸����ú���Ϊ1��������Ϊ0
#define MACHINE3SENSOR_SIM_2SENSOR 1

//����
//#define KEY_NONE        0
//#define KEY_STARTSTOP   1
//#define KEY_AUX         2
//#define KEY_UPDOWN      3
//#define KEY_FRONTBACK   4
//#define KEY_FOOT        5

//��¼����λ����
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


//��Ŀģʽ
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
    //����ģʽ
    PROJECTMODE_TypeDef mode;

    //�豸����
    MachineType_TypeDef machineType;   //0:MACHINE_4SENSORS     1:MACHINE_2SENSORS     2:MACHINE_3SENSORS
    uint8_t sensorLevel; //1:12V,������Ӧ���� �� 0:5V����翪��

    //ʵ�ʶ�����ӡͷ��ƽ̨Ҫ�ֿ�����
    ACT_TypeDef actHead;
    ACT_TypeDef actPlatform;

    //��¼ӡͷλ�á�
    //bit0: 1:��ʾ�������׳�����0:��ʾ�������׷�����  #define UPDOWN_MASK         0x01
    //bit1: 1:��ʾǰ�����׳�����0:��ʾǰ�����׷�����  #define FRONTBACK_MASK      0x02
    uint8_t headPos;
    uint8_t platformPos;//��¼ƽ̨λ��

    uint8_t headState;//ӡͷ�����������õ�״̬ת�Ʊ���
    uint8_t platformState;//ƽ̨�����������õ�״̬ת�Ʊ���

    uint8_t delayHeadPos;//������ʱ����ʱ��ֻҪ����BACK��������ʱ�����޸ĵ������͵���ʱ��ͬ��ֻҪ����FRONT��������ʱ�����޸ĵ���ӡ�͵���ʱ
    uint8_t delayPlatformPos;
    uint8_t delayAbsorb; //������ʱ
    uint8_t delayPrint[DELAY_PRINT_COUNT];  //ӡ����ʱ

    uint8_t led;
    uint8_t segFlashTimes;//ѡ���������������˸����
    uint8_t segFlashCnt;
    uint8_t segStr[6];//�������ʾ

    uint8_t programNum;//ѡ���˵ڼ�������
    uint16_t programAddr;//������EEPROM�еĵ�ַ
    uint16_t productOutput;//����
    uint8_t productOutputOffset;//����������EEPROM�Ĳ�������ַ��ƫ��
    uint16_t productOutputAddr;

    ACTION_TypeDef program[MAX_ACTIONS_PER_PROJECT];//��������

    ACTION_TypeDef *pCurAction;//��ǰ����ָ��
    uint8_t actionCnt;//��ǰ�����м�������
    uint8_t actionPos;//��ǰִ�е��ڼ�������
    uint8_t delay;//��ǰִ�ж�������ʱ

    //�Ա��
    uint8_t programSelfPos;
    ACTION_TypeDef *pCurProSelfAction;


    //�ڲ���������
    InternalSetting_TypeDef internalSetting[INTERNALSETTING_COUNT];  //
    uint8_t intSetAuxCnt;//�����ڲ�����
    uint8_t intSetPos;






    uint16_t returnPosDelay;//shift����ʱ����Ҫ��ʱһС��ʱ�䶨λ���ײ����ջ���
    uint16_t returnNoneDelay;//shift����ʱ����λ�����ջ�����ƽ̨���ҵ�2�����ײ�����һ��ʱ��
    uint16_t shiftPosDelay;//shift����ʱ����Ҫ��ʱһС��ʱ�䶨λ���ײ��ܳ���
    uint16_t shiftStopDelay;//shift����ʱ���ڿ�Ҫֹͣʱ��SHIFT2�����ȷ��������һ��ʱ�䣬����ײ��������

    uint8_t returnPosDelayFactor;
    uint8_t returnNoneDelayFactor;
    uint8_t shiftPosDelayFactor;
    uint8_t shiftStopDelayFactor;

    uint8_t delayUnit; //��ʱ��С��λ
    
    uint8_t platformHomeFlag;

    uint8_t platformPushFlag;  //��¼�Ƿ�����ƽ̨


    //�Զ����ֶ�ʱ������������
    uint8_t keyPress;
    uint8_t keyPressLast;

    //��ͬ�豸���ò�ͬƽ̨����
    void (*Shift)(void);
    void (*Return)(void);
    void (*ShiftReturn)(void);
    void (*ShiftReturn2)(void);

    //оƬID
    uint8_t *mcuID;
    uint8_t eepromMcuID[7];

}MAN_TypeDef;

extern MAN_TypeDef man;
extern const InternalSetting_TypeDef defaultInternalSetting[INTERNALSETTING_COUNT];

void NextPeroid(void);

#endif