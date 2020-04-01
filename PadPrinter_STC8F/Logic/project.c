#include "management.h"
#include "misc.h"
#include "actions.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"
#include "FactoryMode.h"

const ACTION_TypeDef process0[] = {{0, ACTION_NONE}};
const ACTION_TypeDef process1[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {1, SHIFT}, {1, PRINT}, {0, ACTION_NONE}};
const ACTION_TypeDef process2[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {0, SHIFT}, {0, PRINT}, {0, ACTION_NONE}};
const ACTION_TypeDef process3[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {1, SHIFTRETURN}, {0, ACTION_NONE}};
const ACTION_TypeDef process4[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {0, SHIFTRETURN}, {0, ACTION_NONE}};
const ACTION_TypeDef process5[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {1, SHIFT}, {1, PRINT}, {1, SHIFT}, {1, PRINT}, 
                                    {1, SHIFT}, {1, PRINT}, {0, ACTION_NONE}};
const ACTION_TypeDef process6[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {0, SHIFT}, {0, PRINT}, {0, SHIFT}, {0, PRINT}, 
                                    {0, SHIFT}, {0, PRINT}, {0, ACTION_NONE}};
const ACTION_TypeDef process7[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {1, SHIFT}, {1, PRINT}, {1, SHIFT}, {1, PRINT}, {1, SHIFT}, 
                                    {1, PRINT}, {1, SHIFT}, {1, PRINT}, {1, SHIFT}, {1, PRINT}, {0, ACTION_NONE}};
const ACTION_TypeDef process8[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {0, SHIFT}, {0, PRINT}, {0, SHIFT}, {0, PRINT}, {0, SHIFT}, 
                                    {0, PRINT}, {0, SHIFT}, {0, PRINT}, {0, SHIFT}, {0, PRINT}, {0, ACTION_NONE}};
const ACTION_TypeDef process9[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT_DOWN}, {1, SHIFT}, {1, PRINT_UP}, {0, ACTION_NONE}};

const ACTION_TypeDef process10[] = {{1, ABSORB}, {1, FRONT}, {1, PRINT}, {1, SHIFTRETURN2}, {0, ACTION_NONE}};

const ACTION_TypeDef process11[] = {{1, SHIFT}, {1, FRONT}, {1, PRINT}, {1, BACK}, {1, ABSORB}, {0, ACTION_NONE}};


void Program0Read(void)
{
    uint8_t i, j;
    
    for(i=0,j=0;i<MAX_ACTIONS_PER_PROJECT;i++)
    {
        man.program[i].flag = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM0+j);
        man.program[i].act = (ACT_TypeDef)AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM0+j+1);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
}

//
void Program0Save(void)
{
    uint8_t i, j;
    
    for(i=0,j=0;i<MAX_ACTIONS_PER_PROJECT;i++)
    {
        man.program[i].flag = 1;
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, man.program[i].flag);
        
        WDT_CONTR = 0x3C;
        
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, man.program[i].act);
        
        WDT_CONTR = 0x3C;

        j += sizeof(ACTION_TypeDef);
        
        WDT_CONTR = 0x3C;
        
        if(man.program[i].act == ACTION_NONE)//�����������˳�
            break;
        
//        printf("man.program[%d].flag\r\n", (int)i, (int)(man.program[i].flag));
//        printf("man.program[%d].act\r\n", (int)i, (int)(man.program[i].act));
    }
}


//����ָ���������
void ProgramResetDefault0(void)
{
    uint16_t i, j;
    uint8_t temp;
    
    printf("Program reset default data...\r\n");
    
    WDT_CONTR = 0x3C;

    AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, 4); //���������

    //��2������Ҳ���ָ���������
    AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 4);  //productOutput
    AT24CXX_WriteOneByte(EEPROM_ADDR_PRODUCTOUTPUTOFFSET, 0); //productOutputAbsoluteSum
    AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT+4, 0, 2);  //productOutputAbsoluteSum

    temp = AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE_DEFAULT);
    AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, temp);
    temp = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL_DEFAULT);
    AT24CXX_WriteOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL, temp);
    temp = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL_DEFAULT);
    AT24CXX_WriteOneByte(EEPROM_ADDR_HEADSENSORLEVEL, temp);

    //���ָ�Ĭ�ϳ�������
//    AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, defaultInternalSetting[INTERNALSETTING_MACHINETYPE].val);
//    AT24CXX_WriteOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL, defaultInternalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val);
//    AT24CXX_WriteOneByte(EEPROM_ADDR_HEADSENSORLEVEL, defaultInternalSetting[INTERNALSETTING_HEADSENSORLEVEL].val);

    AT24CXX_WriteOneByte(EEPROM_ADDR_DELAYUNIT, defaultInternalSetting[INTERNALSETTING_DELAYUNIT].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_RETURNPOSDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_SHIFTPOSDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_RETURNNONEDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_SHIFTSTOPDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_RESTOREDELAY, defaultInternalSetting[INTERNALSETTING_RESTOREDALAY].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_FRONTBACKDELAY, defaultInternalSetting[INTERNALSETTING_FRONTBACKDALAY].val);

    WDT_CONTR = 0x3C;

    AT24CXX_WriteOneByte(EEPROM_ADDR_DELAY, 2);
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        AT24CXX_WriteOneByte(EEPROM_ADDR_DELAY+1+i, 2);
    
    WDT_CONTR = 0x3C;

    printf("++++The default data is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT));
    printf("++++The prgramNum is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM));
    printf("++++The productOutput is %d\r\n", (int)AT24CXX_ReadLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 2));
    printf("++++The productOutputOffset is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_PRODUCTOUTPUTOFFSET));
    printf("++++The machineType is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE));
    printf("++++The platfromSensorLevel is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL));
    printf("++++The headSensorLevel is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL));
    printf("++++The delayUnit is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAYUNIT));
    printf("++++The returnPosDelayFactor is %d\r\n", (int)(man.returnPosDelayFactor));
    printf("++++The shiftPosDelayFactor is %d\r\n", (int)(man.shiftPosDelayFactor));
    printf("++++The returnNoneDelayFactor is %d\r\n", (int)(man.returnNoneDelayFactor));
    printf("++++The shiftStopDelayFactor is %d\r\n", (int)(man.shiftStopDelayFactor));
    printf("++++The restoreDelay is %d\r\n", (int)(man.restoreDelay));
    printf("++++The frontBackDelay is %d\r\n", (int)(man.frontBackDelay));

    WDT_CONTR = 0x3C;

    printf("\r\n++++The absorb delay is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY));
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        printf("++++The print delay[%d] is %d\r\n", (int)i, (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY+1+i));

    WDT_CONTR = 0x3C;
}

void ProgramResetDefault(void)
{
    uint16_t i,j;
    
    ProgramResetDefault0();

    for(i=0,j=0;i<SIZEOF(process0);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, process0[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, process0[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    
    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);//������85�����Ҫ�ŵ����
    
    printf("Program reset default data finish!\r\n");
}

//��ͨ�����ȡ��������1~11��ȡ
void NormalProgramRead(const ACTION_TypeDef *process)
{
    uint8_t j = 0;
    man.program[j].flag = process[j].flag;
    man.program[j].act = process[j].act;
    man.actionCnt = 1;
    
    WDT_CONTR = 0x3C;
    
    while(man.program[j].act != ACTION_NONE)
    {
        j++;
        if(j >= 40) break;
        man.program[j].flag = process[j].flag;
        man.program[j].act = process[j].act;
        man.actionCnt++;
        
        WDT_CONTR = 0x3C;
    }

    man.actionCnt--;
}

void NormalProgram0Read(uint16_t addr)
{
    uint8_t i = 0;
    uint8_t j = 0;
    man.program[j].flag = AT24CXX_ReadOneByte(addr+i);
    man.program[j].act = (ACT_TypeDef)AT24CXX_ReadOneByte(addr+i+1);
    man.actionCnt = 1;
    
    WDT_CONTR = 0x3C;
    
    while(man.program[j].act != ACTION_NONE)
    {
        i += sizeof(ACTION_TypeDef);
        j++;
        if(j >= 40) break;
        man.program[j].flag = AT24CXX_ReadOneByte(addr+i);
        man.program[j].act = (ACT_TypeDef)AT24CXX_ReadOneByte(addr+i+1);
        man.actionCnt++;
        
        WDT_CONTR = 0x3C;
    }    
    man.actionCnt--;    
}

//��ȡ����
void ProgramRead(void)
{
    uint16_t i, j;
    
    printf("Start reading Program...\r\n");
    
    WDT_CONTR = 0x3C;
    
    //��ʼ��project�ṹ��
    man.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);//��ȡ�������
    man.productOutputOffset = AT24CXX_ReadOneByte(EEPROM_ADDR_PRODUCTOUTPUTOFFSET);
    if(man.productOutputOffset >= (EE_TYPE-EEPROM_ADDR_PRODUCTOUTPUT)/6-1)  //(511-118)/6-1 = 64��
            man.productOutputOffset = 0;
    man.productOutputAddr = EEPROM_ADDR_PRODUCTOUTPUT + man.productOutputOffset*6;
    man.productOutput = (uint32_t)AT24CXX_ReadLenByte(man.productOutputAddr, 4);
    man.productOutputAbsoluteSum = (uint16_t)AT24CXX_ReadLenByte(man.productOutputAddr+4, 2);

    //man.productOutputAbsoluteSum = 995;

    WDT_CONTR = 0x3C;

#if !MACHINE_FIX

//#if (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 5 && HARDWARE_VERSION_APPEND == 'A')//25A�汾SW8�ĳɲ��뿪����
#if 0
    if(INT_SW8)
    {
        man.headSensorLevel = 0;//5V
        man.platformSensorLevel = 0;//5V
        man.machineType = MACHINE_2SENSORS;    
    }
    else
    {
        man.headSensorLevel = 0;//5V
        man.platformSensorLevel = 1;//12V
        man.machineType = MACHINE_3SENSORS;  
    }
#else    
    man.machineType = (MachineType_TypeDef)AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE);
    man.platformSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL);
    man.headSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL);
#endif

#else
    man.machineType = defaultInternalSetting[INTERNALSETTING_MACHINETYPE].val;
    man.platformSensorLevel = defaultInternalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val;
    man.headSensorLevel = defaultInternalSetting[INTERNALSETTING_HEADSENSORLEVEL].val;
#endif

    man.delayUnit = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAYUNIT);
    
    man.returnPosDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_RETURNPOSDELAYFACTOR);
    man.shiftPosDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_SHIFTPOSDELAYFACTOR);
    man.returnNoneDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_RETURNNONEDELAYFACTOR);
    man.shiftStopDelayFactor = AT24CXX_ReadOneByte(EEPROM_ADDR_SHIFTSTOPDELAYFACTOR);
    man.returnPosDelay = ((uint16_t)man.delayUnit)*((uint16_t)man.returnPosDelayFactor);
    man.shiftPosDelay = ((uint16_t)man.delayUnit)*((uint16_t)man.shiftPosDelayFactor);
    man.returnNoneDelay = ((uint16_t)man.delayUnit)*((uint16_t)man.returnNoneDelayFactor);
    man.shiftStopDelay = ((uint16_t)man.delayUnit)*((uint16_t)man.shiftStopDelayFactor);
    
    man.restoreDelay = AT24CXX_ReadOneByte(EEPROM_ADDR_RESTOREDELAY);
    man.frontBackDelay = AT24CXX_ReadOneByte(EEPROM_ADDR_FRONTBACKDELAY);

    WDT_CONTR = 0x3C;
    
    man.delayAbsorb = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY);
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        man.delayPrint[i] = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY+1+i);

    WDT_CONTR = 0x3C;

    //ˢ�������
    sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
    TM1638_SendData(0, man.segStr);
    
    //��ȡMCUID
    man.mcuID = (char idata *)0xf1;
    for(i=0;i<7;i++)
        man.eepromMcuID[i] = AT24CXX_ReadOneByte(EEPROM_ADDR_MCUID+i);

    WDT_CONTR = 0x3C;

    //��ȡ��ǰѡ��ĳ���
    switch(man.programNum)
    {
        case 0:
            man.programAddr = EEPROM_ADDR_PROGRAM0;
            NormalProgram0Read(man.programAddr);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 1:
            NormalProgramRead(process1);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 2:
            NormalProgramRead(process2);
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 3:
            NormalProgramRead(process3);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 4:
            NormalProgramRead(process4);
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 5:
            NormalProgramRead(process5);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 6:
            NormalProgramRead(process6);
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 7:
            NormalProgramRead(process7);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 8:
            NormalProgramRead(process8);
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 9:
            NormalProgramRead(process9);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 10:
            NormalProgramRead(process10);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 11:
            NormalProgramRead(process11);
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        default:
            NormalProgramRead(process2);
            SML(M_AUTO_FLAG_AUX, 0);
        break;
    }

    man.actionPos = 0;
    man.pCurAction = man.program;


    for(j=0;j<man.actionCnt;j++)
    {
        printf("----man.program[%d].flag = %d\r\n", (int)j, (int)(man.program[j].flag));
        printf("----man.program[%d].act = %d\r\n", (int)j, (int)(man.program[j].act));        
    }

    if(man.machineType == MACHINE_4SENSORS)
    {
        man.Shift = Shift4Sensors;
        man.Return = Return4Sensors;

        man.ShiftReturn = ShiftReturn2Sensors;
        man.ShiftReturn2 = ShiftReturnShiftReturn2Sensors;
    }
    else if(man.machineType == MACHINE_2SENSORS)
    {
        man.Shift = Shift2Sensors;
        man.Return = Return2Sensors;
        man.ShiftReturn = ShiftReturn2Sensors;
        man.ShiftReturn2 = ShiftReturnShiftReturn2Sensors;
    }
    else if(man.machineType == MACHINE_3SENSORS)
    {
        man.Shift = Shift3Sensors;
        man.Return = Return3Sensors;

        man.ShiftReturn = ShiftReturn2Sensors;
        man.ShiftReturn2 = ShiftReturnShiftReturn2Sensors;
    }
    else//��ɫ�������豸Ĭ����2����������
    {
        man.Shift = Shift2Sensors;
        man.Return = Return2Sensors;
        man.ShiftReturn = ShiftReturn2Sensors;
        man.ShiftReturn2 = ShiftReturnShiftReturn2Sensors; 
    }
    
    if(man.programNum == 11)
    {
        man.Shift = ShiftProgram11;
        man.Return = ReturnProgram11;
    }

    printf("\r\n");
    printf("----The Program num is %d\r\n", (int)(man.programNum));
    printf("----The Program address is %d\r\n", (int)(man.programAddr));
    printf("----The Program action count is %d\r\n", (int)(man.actionCnt));

    WDT_CONTR = 0x3C;
    
    printf("EEPROM_ADDR_END = %d\r\n", (int)EEPROM_ADDR_END);
    
    printf("----The productOutput is %ld\r\n", (int32_t)(man.productOutput));
    printf("----The productOutputOffset is %d\r\n", (int)(man.productOutputOffset));
    printf("----The productOutputAddr is %d\r\n", (int)(man.productOutputAddr));
    printf("----The productOutputAbsoluteSum is %d\r\n", (int)(man.productOutputAbsoluteSum));

    printf("----The delayUnit is %d\r\n", (int)(man.delayUnit));
    printf("----The returnPosDelayFactor is %d\r\n", (int)(man.returnPosDelayFactor));
    printf("----The returnPosDelay is %d\r\n", (int)(man.returnPosDelay));
    printf("----The shiftPosDelayFactor is %d\r\n", (int)(man.shiftPosDelayFactor));
    printf("----The shiftPosDelay is %d\r\n", (int)(man.shiftPosDelay));
    printf("----The returnNoneDelayFactor is %d\r\n", (int)(man.returnNoneDelayFactor));
    printf("----The returnNoneDelay is %d\r\n", (int)(man.returnNoneDelay));
    printf("----The shiftStopDelayFactor is %d\r\n", (int)(man.shiftStopDelayFactor));
    printf("----The shiftStopDelay is %d\r\n", (int)(man.shiftStopDelay));
    printf("----The restoreDelay is %d\r\n", (int)man.restoreDelay);
    printf("----The frontBackDelay is %d\r\n", (int)(man.frontBackDelay));

    printf("\r\n----The absorb delay is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY));
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        printf("----The print delay[%d] is %d\r\n", (int)i, (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY+1+i));

    WDT_CONTR = 0x3C;
    
    if(man.headSensorLevel) //1
        printf("++++++++This is a 12V sensors machine--Head!!!!\r\n");
    else  //0
        printf("++++++++This is a 5V sensors machine--Head!!!!\r\n");

    if(man.platformSensorLevel) //1
        printf("++++++++This is a 12V sensors machine--Platform!!!!\r\n");
    else  //0
        printf("++++++++This is a 5V sensors machine--Platform!!!!\r\n");
    
    if(man.machineType == MACHINE_2SENSORS) //1
        printf("++++++++This is a 2 sensors machine!!!!\r\n");
    else if(man.machineType == MACHINE_3SENSORS)  //2
        printf("++++++++This is a 3 sensors machine!!!!\r\n");
    else if(man.machineType == MACHINE_4SENSORS)  //0
        printf("++++++++This is a 4 sensors machine!!!!\r\n");
    else if(man.machineType == MACHINE_0SENSORS)  //3
        printf("++++++++This is a 0 sensors machine!!!!\r\n");

    WDT_CONTR = 0x3C;
}

void Project_Run(void)
{
    //�ϵ翪��������ⰴ��
    if(GMR(M_POWERON))
    {
        //if(GML(M_KEY_FRONTBACK) || GML(M_KEY_AUX))//ѡ�����
        if(GML(M_KEY_FRONTBACK))//ѡ�����
        {
            man.mode = PM_PROGRAM_SEL;
     
            printf("Bootloader : Program select...\r\n");

            //��ȡ�������
            man.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);

            //ˢ�������
            sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
            TM1638_SendData(0, man.segStr);

            printf("The current program is %s\r\n", man.segStr);
        }
        else if(GML(M_KEY_UPDOWN))//�Ա��
        {
            man.mode = PM_PROGRAM_SELF;
            
            printf("Bootloader : Program self...\r\n");

            Program0Read();
        
            man.programSelfPos = 0;
            
            sprintf((char*)man.segStr, "%02d-", (int)(man.programSelfPos));
            TM1638_SendData(0, man.segStr);
            
            man.pCurProSelfAction = man.program + man.programSelfPos;
            
            sprintf((char*)man.segStr, "%02d", (int)(man.pCurProSelfAction->act));
            TM1638_SendData(3, man.segStr);

            return; //�ȴ���һ������
        }
        else if(GML(M_KEY_FAST)) //ӡͷ���
        {
            man.mode = PM_HEAD_CHECK;
            
#if !MACHINE_FIX

//#if (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 5 && HARDWARE_VERSION_APPEND == 'A')//25A�汾SW8�ĳɲ��뿪����
#if 0
            if(INT_SW8)
            {
                man.headSensorLevel = 0;//5V
                man.platformSensorLevel = 0;//5V    
            }
            else
            {
                man.headSensorLevel = 0;//5V
                man.platformSensorLevel = 1;//12V  
            }
#else    
            man.headSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL);
            man.platformSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL);
#endif

#else
            man.headSensorLevel = defaultInternalSetting[INTERNALSETTING_HEADSENSORLEVEL].val;
            man.platformSensorLevel = defaultInternalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val;
#endif

            printf("Bootloader : Machine sensors head check...\r\n");
        }
        else if(GML(M_KEY_SLOW)) //ƽ̨���
        {
            man.mode = PM_PLATFORM_CHECK;
            
#if !MACHINE_FIX    

//#if (HARDWARE_VERSION_MAJOR == 2 && HARDWARE_VERSION_MINOR == 5 && HARDWARE_VERSION_APPEND == 'A')//25A�汾SW8�ĳɲ��뿪����
#if 0
            if(INT_SW8)
            {
                man.headSensorLevel = 0;//5V
                man.platformSensorLevel = 0;//5V    
            }
            else
            {
                man.headSensorLevel = 0;//5V
                man.platformSensorLevel = 1;//12V  
            }
#else    
            man.headSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL);
            man.platformSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL);
#endif

#else
            man.headSensorLevel = defaultInternalSetting[INTERNALSETTING_HEADSENSORLEVEL].val;
            man.platformSensorLevel = defaultInternalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val;
#endif

            printf("Bootloader : Machine sensors platform check...\r\n");
        }
        else//��������
        {
            man.mode = PM_NORMAL;
            
            printf("Bootloader : Normal...\r\n");

            //��ȡ����
            ProgramRead();

            //��ʾ�豸����
            sprintf((char*)man.segStr, "%01d.%02d.%02d", (int)(man.machineType), (int)(man.headSensorLevel), (int)(man.platformSensorLevel));
            TM1638_SendData(0, man.segStr);
            //��ʱ800mS
            TS(1, 800);
            while(!TG(1)) WDT_CONTR = 0x3C;

            //��ⵥƬ��ID��������˵�Ƭ��������Ҫ����дһ��MCUID��EEPROM�������������������
            MCUID_Check();

            //���������ͣ�ӡͷ�Ƿ���ԭ��
            MachineStateCheck();

            //ˢ�������
            sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
            TM1638_SendData(0, man.segStr);

            //�������˸--��ʾ��ǰѡ��ĳ���
            Segment_Flash();

            //ƽ̨��飬ƽ̨�Ƿ���ԭ��
            MachineTypeCheck();

            TS(3, OUTPUT_FLASH_PERIOD);
            SML(M_OUTPUT_FLASH, 1);
            sprintf((char*)man.segStr, "%05ld", (int32_t)(man.productOutput));
            TM1638_SendData(0, man.segStr);
             
        }
    }
    
    switch(man.mode)
    {
        case PM_NORMAL:
            Normal();
        break;
        case PM_PROGRAM_SEL: //ѡ�����
            ProgramSelect();
        break;
        case PM_PROGRAM_SELF: //�Ա��
            ProgramSelf();
        break;
        case PM_HEAD_CHECK:  //ӡͷ���������
            MachineSensorsHeadCheck();
        break;
        case PM_PLATFORM_CHECK: //ƽ̨���������
            MachineSensorsPlatformCheck();
        break;
        case PM_PARAMETER_SET:  //��������
            ParameterSet();
        break;
        case PM_FACTORY:    //����ģʽ
            FactoryMode();
        break;
        default:
        break;
    }
}
