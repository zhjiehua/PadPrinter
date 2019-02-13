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
        
        if(man.program[i].act == ACTION_NONE)//如果是最后，则退出
            break;
        
//        printf("man.program[%d].flag\r\n", (int)i, (int)(man.program[i].flag));
//        printf("man.program[%d].act\r\n", (int)i, (int)(man.program[i].act));
    }
}


//程序恢复出厂设置
void ProgramResetDefault0(void)
{
    uint16_t i, j;
    uint8_t temp;
    
    printf("Program reset default data...\r\n");
    
    WDT_CONTR = 0x3C;

    AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, 4);
    //AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, 1);
    //AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, 1);

    AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 2);//需要保存3次才能成功
    AT24CXX_WriteOneByte(EEPROM_ADDR_PRODUCTOUTPUTOFFSET, 0);

    temp = AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE_DEFAULT);
    AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, temp);
    temp = AT24CXX_ReadOneByte(EEPROM_ADDR_SENSORLEVEL_DEFAULT);
    AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL, temp);

//    AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, defaultInternalSetting[INTERNALSETTING_MACHINETYPE].val);
//    AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL, defaultInternalSetting[INTERNALSETTING_SENSORLEVEL].val);

    AT24CXX_WriteOneByte(EEPROM_ADDR_DELAYUNIT, defaultInternalSetting[INTERNALSETTING_DELAYUNIT].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_RETURNPOSDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_RETURNPOSDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_SHIFTPOSDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_SHIFTPOSDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_RETURNNONEDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_RETURNNONEDELAYFACTOR].val);
    AT24CXX_WriteOneByte(EEPROM_ADDR_SHIFTSTOPDELAYFACTOR, defaultInternalSetting[INTERNALSETTING_SHIFTSTOPDELAYFACTOR].val);

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
    printf("++++The sensorLevel is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_SENSORLEVEL));
    printf("++++The delayUnit is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAYUNIT));
    printf("++++The returnPosDelayFactor is %d\r\n", (int)(man.returnPosDelayFactor));
    printf("++++The shiftPosDelayFactor is %d\r\n", (int)(man.shiftPosDelayFactor));
    printf("++++The returnNoneDelayFactor is %d\r\n", (int)(man.returnNoneDelayFactor));
    printf("++++The shiftStopDelayFactor is %d\r\n", (int)(man.shiftStopDelayFactor));

    WDT_CONTR = 0x3C;

    printf("\r\n++++The absorb delay is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY));
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        printf("++++The print delay[%d] is %d\r\n", (int)i, (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY+1+i));

    WDT_CONTR = 0x3C;
}

void ProgramResetDefault1(void)
{
    uint16_t i, j;
 
    ProgramResetDefault0();

    for(i=0,j=0;i<SIZEOF(process1);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j, process1[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+1, process1[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process2);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j, process2[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+1, process2[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process3);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j, process3[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+1, process3[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process4);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j, process4[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+1, process4[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process5);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j, process5[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+1, process5[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process6);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j, process6[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+1, process6[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
}

void ProgramResetDefault(void)
{
    uint16_t i,j;
    
    ProgramResetDefault0();
    ProgramResetDefault1();

    for(i=0,j=0;i<SIZEOF(process7);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j, process7[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+1, process7[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process8);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j, process8[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+1, process8[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process9);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j, process9[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+1, process9[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process10);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM10+j, process10[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM10+j+1, process10[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process11);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM11+j, process11[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM11+j+1, process11[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    for(i=0,j=0;i<SIZEOF(process0);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, process0[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, process0[i].act);
        j += sizeof(ACTION_TypeDef);

        WDT_CONTR = 0x3C;
    }
    
    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);//二进制85，这句要放到最后
    
    printf("Program reset default data finish!\r\n");
}

//读取程序
void ProgramRead(void)
{
    uint16_t i, j;
    
    printf("Start reading Program...\r\n");
    
    WDT_CONTR = 0x3C;
    
    //初始化project结构体
    man.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);//读取程序序号
    man.productOutputOffset = AT24CXX_ReadOneByte(EEPROM_ADDR_PRODUCTOUTPUTOFFSET);
    man.productOutputAddr = EEPROM_ADDR_PRODUCTOUTPUT + man.productOutputOffset;
    man.productOutput = (uint16_t)AT24CXX_ReadLenByte(man.productOutputAddr, 2);
    
    WDT_CONTR = 0x3C;

#if !MACHINE_FIX    
    man.machineType = (MachineType_TypeDef)AT24CXX_ReadOneByte(EEPROM_ADDR_MACHINETYPE);
    man.sensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_SENSORLEVEL);
#else
    man.machineType = defaultInternalSetting[INTERNALSETTING_MACHINETYPE].val;
    man.sensorLevel = defaultInternalSetting[INTERNALSETTING_SENSORLEVEL].val;
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
    
    WDT_CONTR = 0x3C;
    
    man.delayAbsorb = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY);
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        man.delayPrint[i] = AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY+1+i);

    WDT_CONTR = 0x3C;

    //刷新数码管
    sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
    TM1638_SendData(0, man.segStr);
    
//    printf("EEPROM_ADDR_PROGRAM0 = %d\r\n", (int)EEPROM_ADDR_PROGRAM0);
//    printf("EEPROM_ADDR_PROGRAM1 = %d\r\n", (int)EEPROM_ADDR_PROGRAM1);
//    printf("EEPROM_ADDR_PROGRAM2 = %d\r\n", (int)EEPROM_ADDR_PROGRAM2);
//    printf("EEPROM_ADDR_PROGRAM3 = %d\r\n", (int)EEPROM_ADDR_PROGRAM3);
//    printf("EEPROM_ADDR_PROGRAM4 = %d\r\n", (int)EEPROM_ADDR_PROGRAM4);
//    printf("EEPROM_ADDR_PROGRAM5 = %d\r\n", (int)EEPROM_ADDR_PROGRAM5);
//    printf("EEPROM_ADDR_PROGRAM6 = %d\r\n", (int)EEPROM_ADDR_PROGRAM6);
//    printf("EEPROM_ADDR_PROGRAM7 = %d\r\n", (int)EEPROM_ADDR_PROGRAM7);
//    printf("EEPROM_ADDR_PROGRAM8 = %d\r\n", (int)EEPROM_ADDR_PROGRAM8);
//    printf("EEPROM_ADDR_PROGRAM9 = %d\r\n", (int)EEPROM_ADDR_PROGRAM9);
    
    man.mcuID = (char idata *)0xf1;
    for(i=0;i<7;i++)
        man.eepromMcuID[i] = AT24CXX_ReadOneByte(EEPROM_ADDR_MCUID+i);

    WDT_CONTR = 0x3C;

    //读取当前选择的程序
    switch(man.programNum)
    {
        case 0:
            man.programAddr = EEPROM_ADDR_PROGRAM0;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 1:
            man.programAddr = EEPROM_ADDR_PROGRAM1;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 2:
            man.programAddr = EEPROM_ADDR_PROGRAM2;
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 3:
            man.programAddr = EEPROM_ADDR_PROGRAM3;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 4:
            man.programAddr = EEPROM_ADDR_PROGRAM4;
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 5:
            man.programAddr = EEPROM_ADDR_PROGRAM5;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 6:
            man.programAddr = EEPROM_ADDR_PROGRAM6;
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 7:
            man.programAddr = EEPROM_ADDR_PROGRAM7;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 8:
            man.programAddr = EEPROM_ADDR_PROGRAM8;
            SML(M_AUTO_FLAG_AUX, 0);
        break;
        case 9:
            man.programAddr = EEPROM_ADDR_PROGRAM9;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 10:
            man.programAddr = EEPROM_ADDR_PROGRAM10;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        case 11:
            man.programAddr = EEPROM_ADDR_PROGRAM11;
            SML(M_AUTO_FLAG_AUX, 1);
        break;
        default:
            man.programAddr = EEPROM_ADDR_PROGRAM2;
            SML(M_AUTO_FLAG_AUX, 0);
        break;
    }

    i = 0;
    j = 0;
    man.program[j].flag = AT24CXX_ReadOneByte(man.programAddr+i);
    man.program[j].act = (ACT_TypeDef)AT24CXX_ReadOneByte(man.programAddr+i+1);
    printf("----man.program[%d].flag = %d\r\n", (int)j, (int)(man.program[j].flag));
    printf("----man.program[%d].act = %d\r\n", (int)j, (int)(man.program[j].act));
    man.actionCnt = 1;
    
    WDT_CONTR = 0x3C;
    
    while(man.program[j].act != ACTION_NONE)
    {
        i += sizeof(ACTION_TypeDef);
        j++;
        if(j >= 40) break;
        man.program[j].flag = AT24CXX_ReadOneByte(man.programAddr+i);
        man.program[j].act = (ACT_TypeDef)AT24CXX_ReadOneByte(man.programAddr+i+1);
        printf("----man.program[%d].flag = %d\r\n", (int)j, (int)(man.program[j].flag));
        printf("----man.program[%d].act = %d\r\n", (int)j, (int)(man.program[j].act));
        man.actionCnt++;
        
        WDT_CONTR = 0x3C;
    }
    
    man.actionCnt--;
    man.actionPos = 0;
    man.pCurAction = man.program;


    if(man.machineType == MACHINE_4SENSORS)
    {
        man.Shift = Shift4Sensors;
        man.Return = Return4Sensors;
    }
    else if(man.machineType == MACHINE_2SENSORS)
    {
//        if((man.programNum == 3) || (man.programNum == 4))
//            man.Shift = ShiftReturn2Sensors;
//        else
//            man.Shift = Shift2Sensors;

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
    
    printf("EEPROM_ADDR_END = %d\r\n", EEPROM_ADDR_END);
    
    printf("----The productOutput is %d\r\n", (int)(man.productOutput));
    printf("----The productOutputOffset is %d\r\n", (int)(man.productOutputOffset));
    printf("----The productOutputAddr is %d\r\n", (int)(man.productOutputAddr));
    printf("----The delayUnit is %d\r\n", (int)(man.delayUnit));
    printf("----The returnPosDelayFactor is %d\r\n", (int)(man.returnPosDelayFactor));
    printf("----The returnPosDelay is %d\r\n", (int)(man.returnPosDelay));
    printf("----The shiftPosDelayFactor is %d\r\n", (int)(man.shiftPosDelayFactor));
    printf("----The shiftPosDelay is %d\r\n", (int)(man.shiftPosDelay));
    printf("----The returnNoneDelayFactor is %d\r\n", (int)(man.returnNoneDelayFactor));
    printf("----The returnNoneDelay is %d\r\n", (int)(man.returnNoneDelay));
    printf("----The shiftStopDelayFactor is %d\r\n", (int)(man.shiftStopDelayFactor));
    printf("----The shiftStopDelay is %d\r\n", (int)(man.shiftStopDelay));

    printf("\r\n----The absorb delay is %d\r\n", (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY));
    for(i=0;i<DELAY_PRINT_COUNT;i++)
        printf("----The print delay[%d] is %d\r\n", (int)i, (int)AT24CXX_ReadOneByte(EEPROM_ADDR_DELAY+1+i));

    WDT_CONTR = 0x3C;
    
    if(man.sensorLevel) //1
        printf("++++++++This is a 12V sensors machine!!!!\r\n");
    else  //0
        printf("++++++++This is a 5V sensors machine!!!!\r\n");
    
    if(man.machineType == MACHINE_2SENSORS) //1
        printf("++++++++This is a 2 sensors machine!!!!\r\n");
    else if(man.machineType == MACHINE_3SENSORS)  //2
        printf("++++++++This is a 3 sensors machine!!!!\r\n");
    else  //0
        printf("++++++++This is a 4 sensors machine!!!!\r\n");
    
    WDT_CONTR = 0x3C;
}

void Project_Run(void)
{
    //上电开机检查特殊按键
    if(GMR(M_POWERON))
    {
        //if(GML(M_KEY_FRONTBACK) || GML(M_KEY_AUX))//选择程序
        if(GML(M_KEY_FRONTBACK))//选择程序
        {
            man.mode = PM_PROGRAM_SEL;
     
            printf("Bootloader : Program select...\r\n");

            //读取程序序号
            man.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);

            //刷新数码管
            sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
            TM1638_SendData(0, man.segStr);

            printf("The current program is %s\r\n", man.segStr);
        }
        else if(GML(M_KEY_UPDOWN))//自编程
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

            return; //等待多一个周期
        }
        else if(GML(M_KEY_FAST)) //印头检查
        {
            man.mode = PM_HEAD_CHECK;
            
            printf("Bootloader : Machine sensors head check...\r\n");
        }
        else if(GML(M_KEY_SLOW)) //平台检查
        {
            man.mode = PM_PLATFORM_CHECK;
            
            printf("Bootloader : Machine sensors platform check...\r\n");
        }
        else//正常启动
        {
            man.mode = PM_NORMAL;
            
            printf("Bootloader : Normal...\r\n");

            //读取程序
            ProgramRead();

            //检测单片机ID，如果换了单片机，则需要重新写一次MCUID进EEPROM，否则程序不能正常运行
            MCUID_Check();

            //检查机器类型，印头是否在原点
            MachineStateCheck();

            //刷新数码管
            sprintf((char*)man.segStr, "Pro%02d", (int)(man.programNum));
            TM1638_SendData(0, man.segStr);

            //数码管闪烁--显示当前选择的程序
            Segment_Flash();

            //平台检查，平台是否在原点
            MachineTypeCheck();

            TS(3, OUTPUT_FLASH_PERIOD);
            SML(M_OUTPUT_FLASH, 1);
            sprintf((char*)man.segStr, "%05d", (int)(man.productOutput));
            TM1638_SendData(0, man.segStr); 
        }
    }
    
    switch(man.mode)
    {
        case PM_NORMAL:
            Normal();
        break;
        case PM_PROGRAM_SEL: //选择程序
            ProgramSelect();
        break;
        case PM_PROGRAM_SELF: //自编程
            ProgramSelf();
        break;
        case PM_HEAD_CHECK:  //印头传感器检测
            MachineSensorsHeadCheck();
        break;
        case PM_PLATFORM_CHECK: //平台传感器检测
            MachineSensorsPlatformCheck();
        break;
        case PM_PARAMETER_SET:  //参数设置
            ParameterSet();
        break;
        default:
        break;
    }
}
