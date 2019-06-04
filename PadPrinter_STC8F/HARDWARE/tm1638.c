/********************************************************************************************** 
**Program Assignment: Driver for TM1638 digital tube 
**Author        : Wuwang 
**Date              : 2014.8.26 9:00 
**Description       : This is a driver for the board which is controled by thechip of tm1638.  
              The board has eight digital tubes which have eight segments and eight keys. 
***********************************************************************************************/  
#include "io.h"
#include "tm1638.h"
#include "uart.h"
#include "project.h"
#include "management.h"

#include <stdio.h>

//共阴，如果数码管是共阴的，则开这个宏，否则注释掉这个宏
#define COMMON_GROND

/*********************define and global variables*********************************************/
//the char and its segment code 
uint16_t const tm_dat[2][18]={
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '_', ' ', 'P', 'r', 'o', 'A'},  
  {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x80,0x40,0x08,0x00,0x73,0x50,0x5C,0x77},
}; 

uint16_t const tm_led_dat[2][8]={
  {'0', '1', '2', '3', '4', '5', '6', '7'},  
  {0x15,0x16,0x19,0x1A,0x25,0x26,0x29,0x2A},
}; 

uint16_t seg_dat[8];

/*********************************************************************************************** 
*Function Name: GPIO_Config      
*Purpose      : Configration GPIO 
***********************************************************************************************/  
static void GPIO_Config(void){  
    DIS_STB = 1;
    DIS_CLK = 1;
    DIS_DIO = 1;
}

/*********************************************************************************************** 
*Function Name: Write_Byte       
*Purpose      : Write one byte to the data port 
*params       : byte  -------8-bits byte   
*return       : none 
***********************************************************************************************/  
void Write_Byte(uint8_t byte){  
    uint8_t i=0;  
    for(i=0;i<8;i++){  
        DIS_CLK = 0;
        if(byte&0x01){  
            DIS_DIO = 1;
        }else{  
            DIS_DIO = 0;
        }  
        DIS_CLK = 1;
        byte>>=1;  
    }  
}  
  
/*********************************************************************************************** 
*Function Name: Read_Byte        
*Purpose      : Read one byte from data port 
*params       : none 
*return       : the 8-bits byte which is read from data port 
***********************************************************************************************/  
uint8_t Read_Byte(void){  
    uint8_t i=0;  
    uint8_t temp=0x00;
    for(i=0;i<8;i++){  
        DIS_CLK = 1;
        temp>>=1;
        if(DIS_DIO){
            temp|=0x80;
        }
        DIS_CLK = 0;
    }
    return temp;
}
  
/*********************************************************************************************** 
*Function Name: Write_Cmd        
*Purpose      : Write a conmand to the data port 
*params       : cmd  -------8-bits byte,the conmand,check the data sheet to find the conmand  
*return       : none 
***********************************************************************************************/  
void Write_Cmd(uint8_t cmd){  
    DIS_STB = 1;
    DIS_STB = 0;
    Write_Byte(cmd);  
}  

#if 0 //TM1638  
/*********************************************************************************************** 
*Function Name: Read_Key         
*Purpose      : Read the key number which has been pressed 
*params       : none 
*return       : the number of the key. 0-8.  "return 0" represents no key has been pressed. 
***********************************************************************************************/  
uint16_t Read_Key(void){  
    uint8_t i=0;  
    uint8_t k[4];  
    uint32_t key=0x00;
    DIS_STB = 0;
    Write_Cmd(0x42);
    
    DIS_DIO = 1;//this is obligatory, check the data sheet,GPIO
    for(i=0;i<4;i++){  
        k[i]=Read_Byte();
        //key |= (((uint32_t)k[i])<<(i*8));
        //printf("k[%d] = %d\r\n", (int)i, (int)k[i]);
    }
    //key = (uint16_t)k[3]<<8|k[2];
    key = ((uint16_t)k[3]>>1)<<8|k[2];
    DIS_STB = 1; 
    return key;  
}

#else //AIP1668
  
uint16_t Read_Key(void)
{  
    uint8_t i=0;  
    uint8_t k[5];  
    uint32_t key=0x00;
    DIS_STB = 0;
    Write_Cmd(0x42);
    
    DIS_DIO = 1;//this is obligatory, check the data sheet,GPIO
    for(i=0;i<5;i++){  
        k[i]=Read_Byte();
        //printf("k[%d] = %d\r\n", (int)i, (int)k[i]);
    }
    key = ((uint16_t)k[4]>>1)<<8|k[3];
    DIS_STB = 1; 
    return key;
}
#endif

/*********************************************************************************************** 
*Function Name: Write_Dat        
*Purpose      : Write data to the location specified 
*params       : addr  ------the address,0x00 to 0x0f 
                dat   ------the data,segment code 
*return       : none 
***********************************************************************************************/  
void Write_Dat(uint8_t addr,uint8_t dat){  
    Write_Cmd(0x44);  
    Write_Cmd(0xc0|addr);  
    Write_Byte(dat);  
}  

#if !ARRAY_KEYBOARD  //没用矩阵键盘
/*
mode:
    0:支持连按
    1:上升沿有效
    2:下降沿有效
*/
#if 0 //TM1638
uint8_t TM1638_ReadKey(uint8_t mode)
{
    uint8_t ret;
    static uint16_t lastKey = 0;
    uint16_t key = Read_Key();
    uint16_t temp;
    
//    printf("KEY = %d\r\n", (int)key);
//    sprintf((char*)project.segStr, "%04d", (int)key);
//    TM1638_SendData(0, project.segStr);

    switch(mode)
    {
        case 0:
            temp = key;
        break;
        case 1:
            temp = (lastKey^key)&key;
        break;
        case 2:
            temp = (lastKey^key)&lastKey;
        break;
        default:
        break;
    }
    switch(temp)
    {
        case 0x0080: //key1
            ret = KEY_FAST;
        break;
        case 0x0040: //key2
            ret = KEY_SLOW;
        break;
        case 0x0020: //key3
            ret = KEY_AUX;
        break;
        case 0x0400: //key4
            ret = KEY_UPDWON;
        break;
        case 0x0200: //key5
            ret = KEY_FRONTBACK;
        break;
        case 0x0100: //key6
            ret = KEY_STARTSTOP;
        break;
        case 0x4000: //key7
            ret = KEY_FOOT;
        break;
        case 0x00C0: //key1&key2
            ret = KEY_CLEAR;
        break;
        default:
            ret = KEY_NONE;
        break;
    }
    lastKey = key;
    
    return ret;
}
#else //AIP1668

uint8_t TM1638_ReadKey(uint8_t mode)
{
    uint8_t ret;
    static uint16_t lastKey = 0;
    uint16_t key = Read_Key();
    uint16_t temp;
    
//    printf("KEY = %d\r\n", (int)key);
//    sprintf((char*)project.segStr, "%04d", (int)key);
//    TM1638_SendData(0, project.segStr);

    switch(mode)
    {
        case 0:
            temp = key;
        break;
        case 1:
            temp = (lastKey^key)&key;
        break;
        case 2:
            temp = (lastKey^key)&lastKey;
        break;
        default:
        break;
    }
    switch(temp)
    {
        case 0x0002: //key1
            ret = KEY_FAST;
        break;
        case 0x0010: //key2
            ret = KEY_SLOW;
        break;
        case 0x0020: //key3
            ret = KEY_AUX;
        break;
        case 0x0100: //key4
            ret = KEY_UPDOWN;
        break;
        case 0x0200: //key5
            ret = KEY_FRONTBACK;
        break;
        case 0x1000: //key6
            ret = KEY_STARTSTOP;
        break;
        case 0x0800: //key7
            ret = KEY_FOOT;
        break;
        case 0x0012: //key1&key2
            ret = KEY_CLEAR;
        break;
        default:
            ret = KEY_NONE;
        break;
    }
    lastKey = key;
    
    return ret;
}




#endif

#else

uint8_t TM1638_ReadKey(uint8_t mode)
{
    K_COL1 = 0;
    K_COL2 = 1;
    K_COL3 = 1;
    K_COL4 = 1;
    K_ROW1 = 1;
    K_ROW2 = 1;
    Delay10us();
    if(K_ROW1 == 0 && K_ROW2 == 0) return KEY_CLEAR;  
    else if(K_ROW1 == 0) return KEY_FAST;
    else if(K_ROW2 == 0) return KEY_SLOW;

    K_COL1 = 1;
    K_COL2 = 0;
    K_COL3 = 1;
    K_COL4 = 1;
    K_ROW1 = 1;
    K_ROW2 = 1;
    Delay10us();
    if(K_ROW1 == 0)
    {
//        K_COL1 = 1;
//        K_COL2 = 1;
//        K_COL3 = 1;
//        K_COL4 = 0;
//        K_ROW1 = 1;
//        K_ROW2 = 1;
//        Delay10us();
//        if(K_ROW2 == 0) return KEY_SW8_AUX;
//        else 
            return KEY_AUX;
    }
    else if(K_ROW2 == 0) return KEY_STARTSTOP;

    K_COL1 = 1;
    K_COL2 = 1;
    K_COL3 = 0;
    K_COL4 = 1;
    K_ROW1 = 1;
    K_ROW2 = 1;
    Delay10us();
    if(K_ROW1 == 0) return KEY_UPDOWN;
    else if(K_ROW2 == 0) return KEY_FRONTBACK;

    K_COL1 = 1;
    K_COL2 = 1;
    K_COL3 = 1;
    K_COL4 = 0;
    K_ROW1 = 1;
    K_ROW2 = 1;
    Delay10us();
    if(K_ROW1 == 0) return KEY_FOOT;
//    else if(K_ROW2 == 0) return KEY_SW8;


    return KEY_NONE;

}
#endif

void TM1638_LED(uint8_t i, uint8_t *str){  
    int j=0,k=0;
    unsigned char chr;  
    for(;i<8;i++){  
        k=0;  
        for(j=0;j<sizeof(tm_led_dat)/2;j++){  
            if(*str==tm_led_dat[0][j]){  
                chr=tm_led_dat[1][j];  
                k=1;  
                break;  
            }  
        }  
          
        if(k==0){  
            chr=0x00;  
        }  

        if(*(str+1)=='.'){  
            chr|=0x80;  
            Write_Dat(i*2,chr);  
            str++;  
        }else{  
            Write_Dat(i*2,chr);  
        }
        
        str++;  
        if(*str=='\0')break;
    }
}  

void TM1638_LEDIndependentControl(uint8_t i, uint16_t str)
  {  
    int j=0,k=0;
    unsigned char chr;  
    for(;i<8;i++){  
        k=0;  
//        for(j=0;j<sizeof(tm_led_dat)/2;j++){  
//            if(*str==tm_led_dat[0][j]){  
//                chr=tm_led_dat[1][j];  
				chr=str; 
                k=1;  
//                break;  
//            }  
//        }  
          
        if(k==0){  
            chr=0x00;  
        }  

//        if(*(str+1)=='.'){  
//            chr|=0x80;  
//            Write_Dat(i*2,chr);  
//            str++;  
//        }else{  
            Write_Dat(i*2,chr);  
//        }
        
        str++;  
//        if(*str=='\0')break;
    }
} 

/*********************************************************************************************** 
*Function Name: TM1638_SendData      
*Purpose      : Write data to the location specified 
*params       : i     ------the bit code of digtal tube,0 to 7 
                str   ------the string,the char which was not in tm_data will be replace with "''". 
*return       : none 
***********************************************************************************************/  
void TM1638_SendData(uint8_t i, uint8_t *str){  
    int j=0,k=0;
#ifndef COMMON_GROND
    int l=0;
#endif
    unsigned char chr;  
    for(;i<8;i++){  
        k=0;  
        for(j=0;j<sizeof(tm_dat)/2;j++){  
            if(*str==tm_dat[0][j]){  
                chr=tm_dat[1][j];  
                k=1;  
                break;  
            }  
        }  
          
        if(k==0){  
            chr=0x00;  
        }  

#ifdef COMMON_GROND
        if(*(str+1)=='.'){  
            chr|=0x80;  
            Write_Dat(i*2,chr);  
            str++;  
        }else{  
            Write_Dat(i*2,chr);  
        }
#else        
        if(*(str+1)=='.'){  
            chr|=0x80;  
            //Write_Dat(i*2,chr);
            str++;  
        }else{  
            //Write_Dat(i*2,chr);  
        }
        
        //更新内存
        for(l=0;l<8;l++)
        {
            if(chr&0x01)
              seg_dat[l] |= (0x01<<i);
            else
              seg_dat[l] &= ~(0x01<<i);
            
            chr>>=1;
        }
#endif
        
        str++;  
        if(*str=='\0')break;
    }

#ifndef COMMON_GROND    
    //刷新数码管
    for(l=0;l<8;l++)
      Write_Dat(l*2,seg_dat[l]);
#endif
}  
  
/*********************************************************************************************** 
*Function Name: TM1638_Init      
*Purpose      : the initialization of tm1638 
*params       : none 
*return       : none 
***********************************************************************************************/  
void TM1638_Init(void){  
    int i=0;    
    GPIO_Config();  
    //Write_Cmd(0x8f);
    Write_Cmd(0x89); //修改亮度 
    Write_Cmd(0x40);
    //Write_Cmd(0xc0);
    for(i=0;i<16;i++){  
        Write_Byte(0x00);  
    }
}
