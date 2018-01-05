/********************************************************************************************** 
**Program Assignment: Driver for TM1638 digital tube 
**Author        : Wuwang 
**Date              : 2014.8.26 9:00 
**Description       : This is a driver for the board which is controled by thechip of tm1638.  
              The board has eight digital tubes which have eight segments and eight keys. 
***********************************************************************************************/  
#include <stm8s.h>
#include "tm1638.h"
#include "uart1.h"

#include <stdio.h>

//共阴，如果数码管是共阴的，则开这个宏，否则注释掉这个宏
#define COMMON_GROND

/*********************define and global variables*********************************************/
//the char and its segment code 
uint16_t const tm_dat[2][17]={
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '_', ' ', 'P', 'r', 'o'},  
  {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x80,0x40,0x08,0x00,0x73,0x50,0x5C},
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
    //GPIO_DeInit(STB_PORT);
    GPIO_Init(STB_PORT, STB_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    //GPIO_DeInit(CLK_PORT);
    GPIO_Init(CLK_PORT, CLK_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    //GPIO_DeInit(DIO_PORT);
    GPIO_Init(DIO_PORT, DIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
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
        GPIO_WriteLow(CLK_PORT, CLK_PIN);
        if(byte&0x01){  
            GPIO_WriteHigh(DIO_PORT, DIO_PIN);
        }else{  
            GPIO_WriteLow(DIO_PORT, DIO_PIN);
        }  
        GPIO_WriteHigh(CLK_PORT, CLK_PIN);
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
        GPIO_WriteHigh(CLK_PORT, CLK_PIN);
        temp>>=1;
        //if(GPIO_ReadInputPin(DIO_PORT, DIO_PIN)==SET){  //该函数读不了
        if(GPIO_ReadInputData(DIO_PORT)&DIO_PIN){
            temp|=0x80;
        }
        GPIO_WriteLow(CLK_PORT, CLK_PIN);
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
    GPIO_WriteHigh(STB_PORT, STB_PIN);
    GPIO_WriteLow(STB_PORT, STB_PIN);
    Write_Byte(cmd);  
}  
  
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
    GPIO_WriteLow(STB_PORT, STB_PIN);
    Write_Cmd(0x42);
    
    GPIO_WriteHigh(DIO_PORT, DIO_PIN);//this is obligatory, check the data sheet,GPIO
    GPIO_Init(DIO_PORT, DIO_PIN, GPIO_MODE_IN_PU_NO_IT);//GPIO_MODE_IN_PU_NO_IT   GPIO_MODE_OUT_PP_HIGH_FAST
    for(i=0;i<4;i++){  
        k[i]=Read_Byte();
        //key |= (((uint32_t)k[i])<<(i*8));
        //UART1_printf("k[%d] = %d\r\n", i, k[i]);
    }
    //key = (uint16_t)k[3]<<8|k[2];
    key = ((uint16_t)k[3]>>1)<<8|k[2];
    GPIO_WriteHigh(STB_PORT, STB_PIN); 
    GPIO_Init(DIO_PORT, DIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    return key;  
}  
//int8_t Read_Key(void){  
//    uint8_t i=0;  
//    uint8_t key1=0x00;  
//    uint16_t key2=0x00;  
//    Write_Cmd(0x42);                       
//    GPIO_WriteHigh(DIO_PORT, DIO_PIN);//this is obligatory, check the data sheet,GPIO 
//    for(i=0;i<4;i++){  
//        key1=Read_Byte();  
//        key2|=(key1<<i);  
//    }  
//    key2>>=1;  
//    for(i=0;i<8;i++){  
//        if(0x01<<i==key2)return i+1;  
//    }  
//    return 0;  
//}
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

/*
mode:
    0:支持连按
    1:上升沿有效
    2:下降沿有效
*/
uint8_t TM1638_ReadKey(uint8_t mode)
{
    uint8_t ret;
    static uint16_t lastKey = 0;
    uint16_t key = Read_Key();
    uint16_t temp;
    
    //UART1_printf("KEY = %d\r\n", key);
    
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
    Write_Cmd(0x8f);  
    Write_Cmd(0x40);
    //Write_Cmd(0xc0);
    for(i=0;i<16;i++){  
        Write_Byte(0x00);  
    }
}
