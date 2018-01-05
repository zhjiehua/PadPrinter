#include "management.h"
#include "project.h"

#include "simplc_io.h"
#include "simplc_timer.h"

#include <stm8s.h>
#include "io.h"
#include "uart1.h"
#include "tm1638.h"
#include "flash_eeprom.h"
#include "24cxx.h"
#include "tim5.h"

const ACTION_TypeDef process0[] = {{0, ACTION_NONE, 0}};
const ACTION_TypeDef process1[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, HOME, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process2[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT, 10}, {1, HOME, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process3[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PUSH, 10}, {1, PRINT_UP, 0}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process4[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {0, PUSH, 10}, {1, PRINT_UP, 0}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process5[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT,  0}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, HOME, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process6[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT,  0}, {0, SHIFT, 10}, {0, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT, 10}, {1, HOME, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process7[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT,  0}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, SHIFT, 10}, {1, PRINT, 10}, {1, HOME, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process8[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT,  0}, {0, SHIFT, 10}, {0, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT, 10}, {0, SHIFT, 10}, {0, PRINT, 10}, {1, HOME, 10}, {0, ACTION_NONE, 0}};
const ACTION_TypeDef process9[] = {{1, ABSORB, 10}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, SHIFT, 10}, {1, PRINT_UP,  0}, {1, HOME, 10}, {0, ACTION_NONE, 0}};

//const ACTION_TypeDef process1[PROCESS1_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process2[PROCESS2_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process3[PROCESS3_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, PUSH, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process4[PROCESS4_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {0, PUSH, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process5[PROCESS5_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process6[PROCESS6_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process7[PROCESS7_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, SHIFT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {1, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process8[PROCESS8_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, SHIFT, 10}, {0, PRINT_DOWN, 10}, {0, PRINT_UP,  0}, {0, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};
//const ACTION_TypeDef process9[PROCESS9_SIZE] = {{1, ABSORB_DOWN, 10}, {1, ABSORB_UP,  0}, {1, FRONT, 10}, {1, PRINT_DOWN, 10}, {1, SHIFT, 10}, {1, PRINT_UP,  0}, {1, RETURN, 10}, {1, BACK, 10}, {0, ACTION_NONE, 0}};


PROJECT_TypeDef project;

void Program0Read(void)
{
    uint8_t i, j;
    
    for(i=0,j=0;i<MAX_ACTIONS_PER_PROJECT;i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, project.program[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, project.program[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, project.program[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, project.program[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, project.program[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, project.program[i].act);
        project.program[i].delay = 1;
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, project.program[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, project.program[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, project.program[i].delay);
        j += 3;
    }
}

//
void Program0Save(void)
{
    uint8_t i, j;
    
    for(i=0,j=0;i<MAX_ACTIONS_PER_PROJECT;i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, project.program[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, project.program[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, project.program[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, project.program[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, project.program[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, project.program[i].act);
        project.program[i].delay = 1;
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, project.program[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, project.program[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, project.program[i].delay);
        j += 3;
    }
}

//����ָ���������
void ProgramResetDefault(void)
{
    uint16_t i, j;
    //uint8_t temp;
    
    UART1_printf("Program reset default data...\r\n");

    AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, 1);
    AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 2);//��Ҫ����3�β��ܳɹ�
    AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 2);
    AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 2);
    //AT24CXX_WriteLenByte(EEPROM_ADDR_RETURNFILTERTIME, RETURN_FILTER_TIME, 2);
    //AT24CXX_WriteLenByte(EEPROM_ADDR_RETURNPOSTIME, RETURN_POS_TIME, 2);
    //AT24CXX_WriteLenByte(EEPROM_ADDR_PUSHTIME, PUSH_TIME, 2);
    AT24CXX_WriteLenByte(EEPROM_ADDR_ACTIONUNITDELAY, ACTIONUNIT_DELAY, 2);
    AT24CXX_WriteLenByte(EEPROM_ADDR_ACTIONUNITDELAY, ACTIONUNIT_DELAY, 2);
    AT24CXX_WriteLenByte(EEPROM_ADDR_ACTIONUNITDELAY, ACTIONUNIT_DELAY, 2);
    
    UART1_printf("++++The default data is %d\r\n", AT24CXX_ReadOneByte(EEPROM_ADDR_DEFAULT));
    UART1_printf("++++The prgramNum is %d\r\n", AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM));
    UART1_printf("++++The productOutput is %d\r\n", (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 2));
//    UART1_printf("++++The returnFilterTime is %d\r\n", (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_RETURNFILTERTIME, 2));
//    UART1_printf("++++The returnPosTime is %d\r\n", (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_RETURNPOSTIME, 2));
//    UART1_printf("++++The pushTime is %d\r\n", (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_PUSHTIME, 2));
    UART1_printf("++++The actionUnitDelay is %d\r\n", (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_ACTIONUNITDELAY, 2));
    
    for(i=0,j=0;i<SIZEOF(process1);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j, process1[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j, process1[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j, process1[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+1, process1[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+1, process1[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+1, process1[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+2, process1[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+2, process1[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM1+j+2, process1[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process2);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j, process2[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j, process2[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j, process2[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+1, process2[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+1, process2[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+1, process2[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+2, process2[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+2, process2[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM2+j+2, process2[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process3);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j, process3[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j, process3[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j, process3[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+1, process3[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+1, process3[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+1, process3[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+2, process3[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+2, process3[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM3+j+2, process3[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process4);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j, process4[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j, process4[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j, process4[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+1, process4[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+1, process4[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+1, process4[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+2, process4[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+2, process4[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM4+j+2, process4[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process5);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j, process5[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j, process5[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j, process5[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+1, process5[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+1, process5[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+1, process5[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+2, process5[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+2, process5[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM5+j+2, process5[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process6);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j, process6[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j, process6[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j, process6[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+1, process6[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+1, process6[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+1, process6[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+2, process6[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+2, process6[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM6+j+2, process6[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process7);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j, process7[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j, process7[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j, process7[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+1, process7[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+1, process7[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+1, process7[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+2, process7[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+2, process7[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM7+j+2, process7[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process8);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j, process8[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j, process8[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j, process8[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+1, process8[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+1, process8[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+1, process8[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+2, process8[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+2, process8[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM8+j+2, process8[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process9);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j, process9[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j, process9[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j, process9[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+1, process9[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+1, process9[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+1, process9[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+2, process9[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+2, process9[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM9+j+2, process9[i].delay);
        j += 3;
    }
    for(i=0,j=0;i<SIZEOF(process0);i++)
    {
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, process0[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, process0[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j, process0[i].flag);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, process0[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, process0[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+1, process0[i].act);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, process0[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, process0[i].delay);
        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM0+j+2, process0[i].delay);
        j += 3;
    }
    
    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);//������85�����Ҫ�ŵ����
    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
    AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
    
    UART1_printf("Program reset default data finish!\r\n");
}

//��ȡ����
void ProgramRead(void)
{
    uint16_t i, j;
    
    UART1_printf("Start reading Program...\r\n");
    
    //��ʼ��project�ṹ��
    project.programNum = AT24CXX_ReadOneByte(EEPROM_ADDR_PROGRAM_NUM);//��ȡ�������
    project.productOutput =  (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 2);
    project.returnFilterTime = (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_RETURNFILTERTIME, 2);
    project.returnPosTime = (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_RETURNPOSTIME, 2);
    project.pushTime = (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_PUSHTIME, 2); 
    project.actionUnitDelay = (uint16_t)AT24CXX_ReadLenByte(EEPROM_ADDR_ACTIONUNITDELAY, 2);
    
    //project.returnPosTime = RETURN_POS_TIME;
    //project.actionUnitDelay = 1;
      
    //ˢ�������
    sprintf((char*)project.segStr, "Pro%2d", project.programNum);
    TM1638_SendData(0, project.segStr);
    
//    UART1_printf("EEPROM_ADDR_PROGRAM0 = %d\r\n", EEPROM_ADDR_PROGRAM0);
//    UART1_printf("EEPROM_ADDR_PROGRAM1 = %d\r\n", EEPROM_ADDR_PROGRAM1);
//    UART1_printf("EEPROM_ADDR_PROGRAM2 = %d\r\n", EEPROM_ADDR_PROGRAM2);
//    UART1_printf("EEPROM_ADDR_PROGRAM3 = %d\r\n", EEPROM_ADDR_PROGRAM3);
//    UART1_printf("EEPROM_ADDR_PROGRAM4 = %d\r\n", EEPROM_ADDR_PROGRAM4);
//    UART1_printf("EEPROM_ADDR_PROGRAM5 = %d\r\n", EEPROM_ADDR_PROGRAM5);
//    UART1_printf("EEPROM_ADDR_PROGRAM6 = %d\r\n", EEPROM_ADDR_PROGRAM6);
//    UART1_printf("EEPROM_ADDR_PROGRAM7 = %d\r\n", EEPROM_ADDR_PROGRAM7);
//    UART1_printf("EEPROM_ADDR_PROGRAM8 = %d\r\n", EEPROM_ADDR_PROGRAM8);
//    UART1_printf("EEPROM_ADDR_PROGRAM9 = %d\r\n", EEPROM_ADDR_PROGRAM9);
    
    //��ȡ��ǰѡ��ĳ���
    switch(project.programNum)
    {
        case 0:
            project.programAddr = EEPROM_ADDR_PROGRAM0;
            project.platformHomeFlag = 0;
        break;
        case 1:
            project.programAddr = EEPROM_ADDR_PROGRAM1;
            project.platformHomeFlag = 1;
        break;
        case 2:
            project.programAddr = EEPROM_ADDR_PROGRAM2;
            project.platformHomeFlag = 0;
        break;
        case 3:
            project.programAddr = EEPROM_ADDR_PROGRAM3;
            project.platformHomeFlag = 1;
        break;
        case 4:
            project.programAddr = EEPROM_ADDR_PROGRAM4;
            project.platformHomeFlag = 0;
        break;
        case 5:
            project.programAddr = EEPROM_ADDR_PROGRAM5;
            project.platformHomeFlag = 1;
        break;
        case 6:
            project.programAddr = EEPROM_ADDR_PROGRAM6;
            project.platformHomeFlag = 0;
        break;
        case 7:
            project.programAddr = EEPROM_ADDR_PROGRAM7;
            project.platformHomeFlag = 1;
        break;
        case 8:
            project.programAddr = EEPROM_ADDR_PROGRAM8;
            project.platformHomeFlag = 0;
        break;
        case 9:
            project.programAddr = EEPROM_ADDR_PROGRAM9;
            project.platformHomeFlag = 1;
        break;
        default:
            project.programAddr = EEPROM_ADDR_PROGRAM2;
            project.platformHomeFlag = 0;
        break;
    }

    i = 0;
    j = 0;
    project.program[j].flag = AT24CXX_ReadOneByte(project.programAddr+i);
    project.program[j].act = (ACT_TypeDef)AT24CXX_ReadOneByte(project.programAddr+i+1);
    project.program[j].delay = AT24CXX_ReadOneByte(project.programAddr+i+2);
    UART1_printf("----project.program[%d].flag = %d\r\n", j, project.program[j].flag);
    UART1_printf("----project.program[%d].act = %d\r\n", j, project.program[j].act);
    UART1_printf("----project.program[%d].delay = %d\r\n", j, project.program[j].delay);
    project.actionCnt = 1;
    while(project.program[j].act != ACTION_NONE)
    {
        i += 3;
        j++;
        if(j >= 40) break;
        project.program[j].flag = AT24CXX_ReadOneByte(project.programAddr+i);
        project.program[j].act = (ACT_TypeDef)AT24CXX_ReadOneByte(project.programAddr+i+1);
        project.program[j].delay = AT24CXX_ReadOneByte(project.programAddr+i+2);
        UART1_printf("----project.program[%d].flag = %d\r\n", j, project.program[j].flag);
        UART1_printf("----project.program[%d].act = %d\r\n", j, project.program[j].act);
        UART1_printf("----project.program[%d].delay = %d\r\n", j, project.program[j].delay);
        project.actionCnt++;
    }
    
    project.actionCnt--;
    project.actionPos = 0;
    project.pCurAction = project.program;
    project.delay = project.pCurAction->delay;
    
    UART1_printf("\r\n");
    UART1_printf("----The Program num is %d\r\n", project.programNum);
    UART1_printf("----The Program address is %d\r\n", project.programAddr);
    UART1_printf("----The Program action count is %d\r\n", project.actionCnt);

    UART1_printf("----The productOutput is %d\r\n", project.productOutput);
//    UART1_printf("----The returnFilterTime is %d\r\n", project.returnFilterTime);
//    UART1_printf("----The returnPosTime is %d\r\n", project.returnPosTime);
//    UART1_printf("----The pushTime is %d\r\n", project.pushTime);
    UART1_printf("----The actionUnitDelay is %d\r\n", project.actionUnitDelay);
    
    UART1_printf("EEPROM_ADDR_END = %d\r\n", EEPROM_ADDR_END);
}

void Project_Run(void)
{
    //������0
    if(GMR(M_KEY_CLEAR))
    {
        project.productOutput = 0;
        sprintf((char*)project.segStr, "%5d", project.productOutput);
        TM1638_SendData(0, project.segStr);
        
        AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 2);
        AT24CXX_WriteLenByte(EEPROM_ADDR_PRODUCTOUTPUT, 0, 2);
    }
    
    //��������
    if(GML(M_KEY_FAST) || GML(M_KEY_SLOW) || GML(M_KEY_AUX) || GML(M_KEY_UPDOWN) || GML(M_KEY_FRONTBACK)
       || GML(M_KEY_STARTSTOP) || GML(M_KEY_FOOT) || GML(M_KEY_CLEAR))
    {
        TIM2_Cmd(ENABLE);
        SML(M_BEEPER, 1);
        TS(2, 100);
    }
    if(GML(M_BEEPER) && TG(2))
    {
        SML(M_BEEPER, 0);
        TIM2_Cmd(DISABLE);
    }
    
    //LED��
    GYL(Y_UPDOWN) ? (project.led |= 0x02) : (project.led &= ~0x02); 
    GYL(Y_FRONTBACK) ? (project.led |= 0x01) : (project.led &= ~0x01);
    GYL(Y_SHIFT) ? (project.led |= 0x04) : (project.led &= ~0x04);  
    sprintf((char*)project.segStr, "%d", project.led);
    TM1638_LED(5, project.segStr);
      
    //�ϵ�
    if(GMR(M_POWERON))
    {
        if(GML(M_KEY_FRONTBACK))//ѡ�����
        {
            project.mode = PROGRAM_SEL;
            SML(M_PROGRAM_SEL, 1);
            
            UART1_printf("Bootloader : Program select...\r\n");
        }
        else//��������
        {
            project.mode = NORMAL;
            SML(M_PROGRAM_READ, 1);
            
            UART1_printf("Bootloader : Normal...\r\n");
        }
    }
    
    //ģʽѡ��
    if(project.mode == PROGRAM_SEL)//ѡ�����
        ProgramSelect();
    else if(project.mode == PROGRAM_SELF)//�Ա��
    {
        ProgramSelf();
    }
    else if(project.mode == NORMAL)//��������
    {
        //static uint8_t flashCnt = 0;
        
        //��ȡ������
        if(GML(M_PROGRAM_READ))
        {
            ProgramRead();
            
            SML(M_PROGRAM_READ, 0);
            
            SML(M_SEG_FLASH, 1);
        }
        
        //��˸
        if(GML(M_SEG_FLASH))
        {
            SegFlash();
        }
        
        //����ʶ��
        if(GML(M_PROGRAM_READY))
        {
            KeyDistinguish();
        }
        
        //���ƽ̨����Ƿ��λ��λ
        if(GML(M_HOME_CHECK))
        {
            if(project.platformState == 1)
            {
                if(GXF(X_SHIFT_L2))
                {
                    SYL(Y_POS, 0);//�ٽ���λ�����ջ���
                    
                    project.platformState = 2;
                }
            }
            else
            {
                if(GXL(X_SHIFT_O))
                {
                    SML(M_HOME_FINISH, 1);
                    SML(M_HOME_CHECK, 0);
                    
                    project.platformPos = 0;
                    
                    project.platformState = 0;
                }
            }
        }
        
/***********************************ִ�ж���***********************************/
        if(GML(M_MODE_AUTO))
        {
            //ÿ���������е�һ����ʱ
            if(project.delay)//��Ҫ��ʱ
            {
                if(!GML(M_ACTION_DELAY))
                {
                    TS(0, project.actionUnitDelay);//������ʱ��С��λ
                    SML(M_ACTION_DELAY, 1);
                }
                else if(TG(0))
                {
                    project.delay--;
                    SML(M_ACTION_DELAY, 0);
                }
            }
            else//��ʱʱ�䵽
            {
                switch(project.pCurAction->act)//����ʶ��
                {
                    case ABSORB_DOWN://������
                        AbsorbDown();
                    break;
                    case ABSORB_UP://������
                        AbsorbUp();
                    break;
                    case ABSORB://����
                        Absorb();
                    break;
                    case FRONT://ǰ��
                        Front();
                    break;
                    case BACK://����
                        Back();
                    break;
                    case PRINT_DOWN://ӡ����
                        PrintDown();
                    break;
                    case PRINT_UP://ӡ����
                        PrintUp();
                    break;
                    case PRINT://ӡ��
                        Print();
                    break;
                    case SHIFT://��λ
                        Shift();
                    break;
                    case RETURN://��λ
                        Return();
                    break;
                    case PUSH://����
                        Push();
                    break;
                    case HOME://��ԭ��
                        Home();
                    break;
                    default:
                    break;
                }//����ʶ��
            }//��ʱʱ�䵽
        }
        
 /***************************************ִ���ֶ�����******************************/
        if(GML(M_MODE_MANUAL))
        {
            if(GML(M_MAN_UPDOWN))
            {
                if(project.programNum == 9)
                {
                    if(project.printHeadPos&FRONTBACK_MASK)
                    {
                        if(project.printHeadPos&UPDOWN_MASK)
                            PrintUp();
                        else
                            PrintDown();
                    }
                    else
                        Absorb();
                }
                else
                {
                    if(project.printHeadPos&FRONTBACK_MASK)
                        Print();
                    else
                        Absorb();
                }
            }
            if(GML(M_MAN_FRONTBACK))
            {
                if(project.printHeadPos&FRONTBACK_MASK)
                    Back();
                else
                    Front();
            }
            if(GML(M_MAN_AUX))
            {
                if(GML(M_MAN_AUX_RETURN))
                    Return();
                else
                    Shift();
                
                SML(M_MAN_FLAG_AUX, 1);
            }
            
            if(!GML(M_MAN_UPDOWN) && !GML(M_MAN_FRONTBACK) && !GML(M_MAN_AUX))
                SML(M_MODE_MANUAL, 0);
        }
      
/**************************************�ָ��Զ�״̬************************************/        
        else if(GML(M_MODE_RESTORE))
        {
            //UART1_printf("Restore mode.\r\n");
          
            if(GML(M_RESTORE_UP))
            {
                if(project.printHeadPos&FRONTBACK_MASK)
                    PrintUp();
                else
                    AbsorbUp();
            }
            
            if(!GML(M_RESTORE_UP) && GML(M_RESTORE_HOME))
            {
//                if(project.platformHomeFlag)
                    Home();
//                else
//                {
//                    if(GML(M_PROGRAM_AUX))
//                        Home();
//                    else
//                        PrintHeadHome();
//                }
            }
            
            if(!GML(M_RESTORE_UP) && !GML(M_RESTORE_HOME))
            {
                SML(M_MODE_RESTORE, 0);
                SML(M_MODE_AUTO, 1);
                
                project.actionPos = 0;
                project.pCurAction = project.program + project.actionPos;
                project.delay = project.pCurAction->delay;
                
                UART1_printf("Restore finish.\r\n\r\n");
            }
        }
    }
}