#include "uart.h"
#include "io.h"
#include "timer.h"
#include "simplc_io.h"
#include "tm1638.h"
#include "24cxx.h"
#include "project.h"
#include "management.h"

//标志串口正在发送数据
static bit Uart_Busy;
static bit Uart2_Busy;

/* 自定义宏 */
#define USART_REC_LEN  			20

/* 全局变量定义 */
	
uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲，最大UASRT_REC_LEN个字节
//接收状态
//bit15，接收完成标志
//bit14，接收到0x0d
//bit13~0,接收到的有效字节数目
uint16_t USART_RX_STA=0;       //接收状态标记

//串口初始化，串口波特率发生器用的是定时器1
void Uart_Init(void)
{
	/************************串口1***********************************/
	PCON &= ~0x80;	//串口1波特率控制位，0：各模式波特率不加倍；1：模式1、2、3波特率加倍

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;            //8-bit variable UART
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
#endif

	TMOD &= ~0xF0;            //定时器1为定时器，模式0，即16位自动重装载模式

	AUXR |= 0x40;  //定时器1速度控制，0:12T模式，即CPU时钟12分频，1:1T模式，直接用CPU时钟
	AUXR &= ~0x20;  //串口1波特率6倍速，0：不倍速；1:6倍速
	AUXR &= ~0x01;  //串口1波特率发生器选择，0：定时器1；1：定时器2

	TL1 = (65536 - (FOSC/4/USERUART_BAUD));
	TH1 = (65536 - (FOSC/4/USERUART_BAUD))>>8;

	TR1 = 1;                //Timer1 start run

	ES = 1;                 //Enable UART1 interrupt

	//中断优先级，最低级00
	PS = 0;
	IP2 &= ~PSH;

	/************************串口2***********************************/
//	P_SW2 &= ~0x01;                              //RXD2/P1.0, TXD2/P1.1
////  P_SW2 |= 0x01;                               //RXD2_2/P4.0, TXD2_2/P4.2
//
//	S2CON = 0x50;  //8位可变波特率
//
//	AUXR &= ~0x08; //定时器2定时模式
//	AUXR |= 0x04; //定时器2速度控制，0:12T模式，即CPU时钟12分频，1:1T模式，直接用CPU时钟
//	AUXR |= 0x10; //启动定时器2
//	T2L = (65536 - (FOSC/4/LCDUART_BAUD));
//	T2H = (65536 - (FOSC/4/LCDUART_BAUD)) >> 8;	
//	IE2 |= 0x01; //允许串口2中断
}

/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 using 1
{
	uint8_t Buf = SBUF;
    //qsize pos;

    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag

        if((USART_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART_RX_STA&0x4000)//接收到了0x0d
            {
                if(Buf!=0x0a)
                    USART_RX_STA=0;//接收错误，重新开始
                else
                    USART_RX_STA|=0x8000;	//接收完成
            }
            else //还没接收到0X0D
            {	
                if(Buf==0x0d)
                    USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Buf;
                    USART_RX_STA++;
                    if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误，重新开始接收  
                }		 
            }
        }
    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        Uart_Busy = 0;           //Clear transmit busy flag
    }
}

/*----------------------------
Send a byte data to UART
Input: Data (data to be sent)
Output:None
----------------------------*/
void Uart_SendData(uint8_t Data)
{
    while (Uart_Busy);           //Wait for the completion of the previous data is sent
    ACC = Data;              //Calculate the even parity bit P (PSW.0)
    if (P)                  //Set the parity bit according to P
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;            //Set parity bit to 0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;            //Set parity bit to 1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;            //Set parity bit to 0
#endif
    }
    Uart_Busy = 1;
    SBUF = ACC;             //Send data to UART buffer
}

/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void Uart_SendString(uint8_t *s)
{
    while (*s)              //Check the end of the string
    {
        Uart_SendData(*s++);     //Send current char and increment string ptr
    }
}

//重写putchar函数
char putchar(char c)
{
    Uart_SendData(c);
    return c;
}


void UartDataParse(void)
{
    uint8_t c[2];   

    if(USART_RX_STA&0x8000)
    {					   
        switch(USART_RX_BUF[0])
        {
            case 'X':    //0x58
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        printf("X0 = %d, GXL(X0) = %d\r\n", (int)X0, (int)GXL(X_ABSORB_O));
                        printf("X1 = %d, GXL(X1) = %d\r\n", (int)X1, (int)GXL(X_ABSORB_L));
                        printf("X2 = %d, GXL(X2) = %d\r\n", (int)X2, (int)GXL(X_PRINT_O));
                        printf("X3 = %d, GXL(X3) = %d\r\n", (int)X3, (int)GXL(X_PRINT_L));
                        printf("X8 = %d, GXL(X8) = %d\r\n", (int)X8, (int)GXL(X_SCRAPER));
                    break;
                    case '1':
                        printf("X4 = %d, GXL(X4) = %d\r\n", (int)X4, (int)GXL(X_SHIFT_O));
                        printf("X5 = %d, GXL(X5) = %d\r\n", (int)X5, (int)GXL(X_POS));
                        printf("X6 = %d, GXL(X6) = %d\r\n", (int)X6, (int)GXL(X_SHIFT_L1));
                        printf("X7 = %d, GXL(X7) = %d\r\n", (int)X7, (int)GXL(X_SHIFT_L2));
                    break;
                    case '2':
                        printf("INT_SW8 = %d\r\n", (int)INT_SW8);
                    break;
                    default:
                    break;
                }
            break;
//            case 'Y':
//                switch(USART_RX_BUF[1])
//                {
//                    case '0':
//                        Y0(USART_RX_BUF[2]-0x30);
//                        //MOUT2 = !!(USART_RX_BUF[2]-0x30);
//                    break;
//                    case '1':
//                        Y1(USART_RX_BUF[2]-0x30);
//                    break;
//                    case '2':
//                        Y2(USART_RX_BUF[2]-0x30);
//                    break;
//                    case '3':
//                        Y3(USART_RX_BUF[2]-0x30);
//                    break;
//                    case '4':
//                        Y4(USART_RX_BUF[2]-0x30);
//                    break;
//                    case '5':
//                        Y5(USART_RX_BUF[2]-0x30);
//                    break;
//                    default:
//                    break;
//                }
//            break;
//            case 'B':
//                switch(USART_RX_BUF[1])
//                {
//                    case '0':
//                        Timer3_Cmd(0);
//                    break;
//                    case '1':
//                        Timer3_Cmd(1);//ENABLE
//                    break;
//                    default:
//                    break;
//                }
//            break;
//            case 'T':
//                sprintf((char*)c, "%d", USART_RX_BUF[1]-0x30);
//                TM1638_LED(5, c);
//            break;
            case 'R':   //0x52
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0x55);
                    break;
                    case '1':
                        AT24CXX_WriteOneByte(EEPROM_ADDR_DEFAULT, 0xAA);
                        printf("Reset default data!\r\n");
                    break;

                    case 's':
                         while(1);
                    break;
                    default:
                    break;
                }
            break;
            case 'S':   //0x53
                switch(USART_RX_BUF[1])
                {
                    case '0':   //设备类型
                        AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE_DEFAULT, USART_RX_BUF[2]-0x30);
                        printf("Reset default data--machinetype is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;
                    case '1':  //印头传感器电平
                        AT24CXX_WriteOneByte(EEPROM_ADDR_HEADSENSORLEVEL_DEFAULT, USART_RX_BUF[2]-0x30);
                        printf("Reset default data--headsensorlevel is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;
                    case '2':  //平台传感器电平
                        AT24CXX_WriteOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL_DEFAULT, USART_RX_BUF[2]-0x30);
                        printf("Reset default data--platformsensorlevel is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;

                    case '5':   //设备类型
                        AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE, USART_RX_BUF[2]-0x30);
                        printf("Reset current eeprom--machinetype is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;
                    case '6':  //印头传感器电平
                        AT24CXX_WriteOneByte(EEPROM_ADDR_HEADSENSORLEVEL, USART_RX_BUF[2]-0x30);
                        printf("Reset current eeprom--headsensorlevel is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;
                    case '7':  //平台传感器电平
                        AT24CXX_WriteOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL, USART_RX_BUF[2]-0x30);
                        printf("Reset current eeprom--platformsensorlevel is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;
                    case '8':  //程序编号
                        AT24CXX_WriteOneByte(EEPROM_ADDR_PROGRAM_NUM, USART_RX_BUF[2]-0x30);
                        printf("Reset current eeprom--programnumber is %d!\r\n", (int)(USART_RX_BUF[2]-0x30));
                    break;
                    default:
                    break;
                }
            break;
            case 'G':  //0x47
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        printf("man.actHead = %d\r\n", (int)man.actHead);
                        printf("man.actPlatform = %d\r\n", (int)man.actPlatform);
                    break;
                    case '1':
                        //printf("GML(M_MODE_AUTO) = %d, GML(M_MODE_RESTORE) = %d\r\n", (int)GML(M_MODE_AUTO), (int)GML(M_MODE_RESTORE));
                        printf("man.runMode = %d\r\n", (int)man.runMode);
                        printf("GML(M_ACTIONHEAD_FINISH) = %d, GML(M_ACTIONPLATFORM_FINISH) = %d\r\n", (int)GML(M_ACTIONHEAD_FINISH), (int)GML(M_ACTIONPLATFORM_FINISH));
                        printf("GML(M_MODE_RESTORE_QUIT) = %d\r\n", (int)GML(M_MODE_RESTORE_QUIT));
                    break;
                    case '2':
                        printf("GML(M_ABSORB_FINISH) = %d, GML(M_PRINT_FINISH) = %d\r\n", (int)GML(M_ABSORB_FINISH), (int)GML(M_PRINT_FINISH));
                        printf("GML(M_FRONT_FINISH) = %d, GML(M_BACK_FINISH) = %d\r\n", (int)GML(M_FRONT_FINISH), (int)GML(M_BACK_FINISH));
                        printf("GML(M_SHIFT_FINISH) = %d, GML(M_RETURN_FINISH) = %d\r\n", (int)GML(M_SHIFT_FINISH), (int)GML(M_RETURN_FINISH));
                    break;
                    case '3':
                        printf("man.headState = %d\r\n", (int)man.headState);
                        printf("man.platformState = %d\r\n", (int)man.platformState);
                        printf("man.headPos = %d\r\n", (int)man.headPos);
                        printf("man.platformPos = %d\r\n", (int)man.platformPos);  
                    break;
                    case '4':
                        printf("man.keyPress = %d\r\n", (int)man.keyPress);
                        printf("man.delay = %d\r\n", (int)man.delay);
                        
                    break;
                    default:
                    break;
                }
                
            break;
            case 'E':  //0x45
                switch(USART_RX_BUF[1])
                {
                    case '0':  //从RAM中读ID          
                        man.mcuID = (char idata *)0xf1;
                        {
                            uint8_t i;
                            printf("man.mcuID = 0x");
                            for(i=0;i<7;i++)
                            {
                               printf("%0x", (int)man.mcuID[i]);
                               AT24CXX_WriteOneByte(EEPROM_ADDR_MCUID+i, man.mcuID[i]); 
                            }
                            printf("\r\n");
                        }                       
                    break;
                    case '1': //从ROM中读ID           
                        man.mcuID = (char code *)0x7ff9;	// STC8A8K32S4A10
                        {
                            uint8_t i;
                            printf("man.mcuID = 0x");
                            for(i=0;i<7;i++)
                            {
                               printf("%0x", (int)man.mcuID[i]); 
                            }
                            printf("\r\n");
                        }                       
                    break;
                    case '2': //从EEPROM中读ID           
                        {
                            uint8_t i;
                            printf("man.eepromMcuID = 0x");
                            for(i=0;i<7;i++)
                            {
                               man.eepromMcuID[i] = AT24CXX_ReadOneByte(EEPROM_ADDR_MCUID+i);
                               printf("%0x", (int)man.eepromMcuID[i]); 
                            }
                            printf("\r\n");
                        }                       
                    break;


                    case '3': //写加密过的ID进EEPROM
                        {
                            uint8_t i;

                            for(i=0;i<7;i++)
                                man.eepromMcuID[i] = USART_RX_BUF[2+i];

                            for(i=0;i<7;i++)
                            {
                                AT24CXX_WriteOneByte(EEPROM_ADDR_MCUID+i, man.eepromMcuID[i]);    
                            }

                            printf("Write McuID sucess!\r\n");
                        }
                    break;
                    case '4': //发送mcuID给上位机
                        {
                            uint8_t i;

                            man.mcuID = (char code *)0x7ff9;

                            Uart_SendData(0xA5); //帧头
                            Uart_SendData(0x5A);
                            Uart_SendData(0x08); //长度 8个字节

                            Uart_SendData(0xE4); //命令

                            for(i=0;i<7;i++)
                                Uart_SendData(man.mcuID[i]);
                        }
                    break;
                    default:
                    break;
                }
            break;
            case 'P':  //0x50, 读写eeprom
                switch(USART_RX_BUF[1])
                {
                    case '0':  //读
                    {         
                        uint16_t addr = (uint16_t)USART_RX_BUF[2]<<8|(uint16_t)USART_RX_BUF[3];
                        uint8_t len = USART_RX_BUF[4];
                        uint8_t i, temp;

                        printf("eeprom %d = ", addr);
                        for(i=0;i<len;i++)
                        {
                           temp = AT24CXX_ReadOneByte(addr+i);
                           printf("0x%0x, ", (int)temp); 
                        }
                        printf("\r\n");
                    }                      
                    break;
                    case '1': //写      
                    {         
                        uint16_t addr = (uint16_t)USART_RX_BUF[2]<<8|(uint16_t)USART_RX_BUF[3];
                        uint8_t len = USART_RX_BUF[4];
                        uint8_t i;

                        for(i=0;i<len;i++)
                        {
                           AT24CXX_WriteOneByte(addr+i, USART_RX_BUF[5+i]); 
                        }
                        printf("eeprom write success!\r\n");
                    }                       
                    break;
                    case '2':
                        printf("The Hardware Version is %s\r\n", HARDWARE_VERSION_STRING);
                    break;
                    case '3':
                        printf("The Software Version is %s\r\n", SOFTWARE_VERSION_STRING);
                    break;
                    default:
                    break;
                }
            break;
            case 'M':   //模式设置
                switch(USART_RX_BUF[1])
                {
                    case '0':  //工厂模式
                    {  
                        man.mode = PM_FACTORY;
        
#if !MACHINE_FIX    
                        man.platformSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_PLATFORMSENSORLEVEL);
                        man.headSensorLevel = AT24CXX_ReadOneByte(EEPROM_ADDR_HEADSENSORLEVEL);
#else
                        man.platformSensorLevel = defaultInternalSetting[INTERNALSETTING_PLATFORMSENSORLEVEL].val;
                        man.headSensorLevel = defaultInternalSetting[INTERNALSETTING_HEADSENSORLEVEL].val;
#endif
                                
                        printf("Bootloader : Factory Mode...\r\n");
                    }
                    break;
                    default:
                    break;
                }
            break;
            default:
            break;
        }
        USART_RX_STA=0;
    }
}







//
///****************************************************
//**
//**串口2
//**
//*****************************************************/
//void Uart2Isr() interrupt 8 using 2
//{
//    if (S2CON & 0x02)
//    {
//        S2CON &= ~0x02;
//        Uart2_Busy = 0;
//    }
//    if (S2CON & 0x01)
//    {
//        S2CON &= ~0x01;
//        //buffer[wptr++] = S2BUF;
//        //wptr &= 0x0f;
//
////		{
////			qsize pos = (lcd_que._head+1)%QUEUE_MAX_SIZE;
////			if(pos!=lcd_que._tail)//非满状态
////			{
////				lcd_que._data[lcd_que._head] = S2BUF;
////				lcd_que._head = pos;
////			}
////		}
//    }
//}
//
//void Uart2Send(char dat)
//{
//    while (Uart2_Busy);
//    Uart2_Busy = 1;
//    S2BUF = dat;
//}
//
//void Uart2SendStr(char *p)
//{
//    while (*p)
//    {
//        Uart2Send(*p++);
//    }
//}
