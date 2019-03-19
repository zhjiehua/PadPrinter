#include "uart.h"
#include "io.h"
#include "timer.h"
#include "simplc_io.h"
#include "tm1638.h"
#include "24cxx.h"
#include "project.h"
#include "management.h"

//��־�������ڷ�������
static bit Uart_Busy;
static bit Uart2_Busy;

/* �Զ���� */
#define USART_REC_LEN  			20

/* ȫ�ֱ������� */
	
uint8_t USART_RX_BUF[USART_REC_LEN];     //���ջ��壬���UASRT_REC_LEN���ֽ�
//����״̬
//bit15��������ɱ�־
//bit14�����յ�0x0d
//bit13~0,���յ�����Ч�ֽ���Ŀ
uint16_t USART_RX_STA=0;       //����״̬���

//���ڳ�ʼ�������ڲ����ʷ������õ��Ƕ�ʱ��1
void Uart_Init(void)
{
	/************************����1***********************************/
	PCON &= ~0x80;	//����1�����ʿ���λ��0����ģʽ�����ʲ��ӱ���1��ģʽ1��2��3�����ʼӱ�

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;            //8-bit variable UART
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;            //9-bit variable UART, parity bit initial to 1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;            //9-bit variable UART, parity bit initial to 0
#endif

	TMOD &= ~0xF0;            //��ʱ��1Ϊ��ʱ����ģʽ0����16λ�Զ���װ��ģʽ

	AUXR |= 0x40;  //��ʱ��1�ٶȿ��ƣ�0:12Tģʽ����CPUʱ��12��Ƶ��1:1Tģʽ��ֱ����CPUʱ��
	AUXR &= ~0x20;  //����1������6���٣�0�������٣�1:6����
	AUXR &= ~0x01;  //����1�����ʷ�����ѡ��0����ʱ��1��1����ʱ��2

	TL1 = (65536 - (FOSC/4/USERUART_BAUD));
	TH1 = (65536 - (FOSC/4/USERUART_BAUD))>>8;

	TR1 = 1;                //Timer1 start run

	ES = 1;                 //Enable UART1 interrupt

	/************************����2***********************************/
//	P_SW2 &= ~0x01;                              //RXD2/P1.0, TXD2/P1.1
////  P_SW2 |= 0x01;                               //RXD2_2/P4.0, TXD2_2/P4.2
//
//	S2CON = 0x50;  //8λ�ɱ䲨����
//
//	AUXR &= ~0x08; //��ʱ��2��ʱģʽ
//	AUXR |= 0x04; //��ʱ��2�ٶȿ��ƣ�0:12Tģʽ����CPUʱ��12��Ƶ��1:1Tģʽ��ֱ����CPUʱ��
//	AUXR |= 0x10; //������ʱ��2
//	T2L = (65536 - (FOSC/4/LCDUART_BAUD));
//	T2H = (65536 - (FOSC/4/LCDUART_BAUD)) >> 8;	
//	IE2 |= 0x01; //������2�ж�
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

        if((USART_RX_STA&0x8000)==0)//����δ���
        {
            if(USART_RX_STA&0x4000)//���յ���0x0d
            {
                if(Buf!=0x0a)
                    USART_RX_STA=0;//���մ������¿�ʼ
                else
                    USART_RX_STA|=0x8000;	//�������
            }
            else //��û���յ�0X0D
            {	
                if(Buf==0x0d)
                    USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Buf;
                    USART_RX_STA++;
                    if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ������¿�ʼ����  
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

//��дputchar����
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
            case 'X':
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        printf("X0 = %d\r\n", (int)X0);
                        printf("GXL(X0) = %d\r\n", (int)GXL(X_ABSORB_O));
                    break;
                    case '1':
                        printf("X1 = %d\r\n", (int)X1);
                        printf("GXL(X1) = %d\r\n", (int)GXL(X_ABSORB_L));
                    break;
                    case '2':
                        printf("X2 = %d\r\n", (int)X2);
                        printf("GXL(X2) = %d\r\n", (int)GXL(X_PRINT_O));
                    break;
                    case '3':
                        printf("X3 = %d\r\n", (int)X3);
                        printf("GXL(X3) = %d\r\n", (int)GXL(X_PRINT_L));
                    break;
                    case '4':
                        printf("X4 = %d\r\n", (int)X4);
                        printf("GXL(X4) = %d\r\n", (int)GXL(X_SHIFT_O));
                    break;
                    case '5':
                        printf("X5 = %d\r\n", (int)X5);
                        printf("GXL(X5) = %d\r\n", (int)GXL(X_POS));
                    break;
                    case '6':
                        printf("X6 = %d\r\n", (int)X6);
                        printf("GXL(X6) = %d\r\n", (int)GXL(X_SHIFT_L1));
                    break;
                    case '7':
                        printf("X7 = %d\r\n", (int)X7);
                        printf("GXL(X7) = %d\r\n", (int)GXL(X_SHIFT_L2));
                    break;
                    case '8':
                        //printf("X8 = %d\r\n", (int)INT_PD);
                    break;
                    default:
                    break;
                }
            break;
            case 'Y':
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        Y0(USART_RX_BUF[2]-0x30);
                        //MOUT2 = !!(USART_RX_BUF[2]-0x30);
                    break;
                    case '1':
                        Y1(USART_RX_BUF[2]-0x30);
                    break;
                    case '2':
                        Y2(USART_RX_BUF[2]-0x30);
                    break;
                    case '3':
                        Y3(USART_RX_BUF[2]-0x30);
                    break;
                    case '4':
                        Y4(USART_RX_BUF[2]-0x30);
                    break;
                    case '5':
                        Y5(USART_RX_BUF[2]-0x30);
                    break;
                    default:
                    break;
                }
            break;
            case 'B':
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        Timer3_Cmd(0);
                    break;
                    case '1':
                        Timer3_Cmd(1);//ENABLE
                    break;
                    default:
                    break;
                }
            break;
            case 'T':
                sprintf((char*)c, "%d", USART_RX_BUF[1]-0x30);
                TM1638_LED(5, c);
            break;
            case 'R':
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
            case 'S':
                switch(USART_RX_BUF[1])
                {
                    case '0':   //2������5V
                        AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE_DEFAULT, 1);
                        AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL_DEFAULT, 0);
                        printf("Reset default data is 2 sensors 5V!\r\n");
                    break;
                    case '1':  //3������12V
                        AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE_DEFAULT, 2);
                        AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL_DEFAULT, 1);
                        printf("Reset default data is 3 sensors 12V!\r\n");
                    break;
                    case '2':  //2������12V
                        AT24CXX_WriteOneByte(EEPROM_ADDR_MACHINETYPE_DEFAULT, 1);
                        AT24CXX_WriteOneByte(EEPROM_ADDR_SENSORLEVEL_DEFAULT, 1);
                        printf("Reset default data is 2 sensors 12V!\r\n");
                    break;
                    default:
                    break;
                }
            break;
            case 'G':
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        printf("man.actHead = %d\r\n", (int)man.actHead);
                    break;
                    case '1':
                        printf("man.actPlatform = %d\r\n", (int)man.actPlatform); 
                    break;
                    case '2':
                        printf("GML(M_MODE_AUTO) = %d, GML(M_MODE_RESTORE) = %d\r\n", (int)GML(M_MODE_AUTO), (int)GML(M_MODE_RESTORE));
                    break;
                    case '3':
                        printf("GML(M_ACTIONHEAD_FINISH) = %d, GML(M_ACTIONPLATFORM_FINISH) = %d\r\n", (int)GML(M_ACTIONHEAD_FINISH), (int)GML(M_ACTIONPLATFORM_FINISH));
                    break;
                    case '4':
                        printf("GML(M_ABSORB_FINISH) = %d, GML(M_PRINT_FINISH) = %d\r\n", (int)GML(M_ABSORB_FINISH), (int)GML(M_PRINT_FINISH));
                    break;
                    case '5':
                        printf("GML(M_FRONT_FINISH) = %d, GML(M_BACK_FINISH) = %d\r\n", (int)GML(M_FRONT_FINISH), (int)GML(M_BACK_FINISH));
                    break;
                    case '6':
                        printf("GML(M_SHIFT_FINISH) = %d, GML(M_RETURN_FINISH) = %d\r\n", (int)GML(M_SHIFT_FINISH), (int)GML(M_RETURN_FINISH));
                    break;
                    default:
                    break;
                }
                
            break;
            case 'E':
                switch(USART_RX_BUF[1])
                {
                    case '0':  //��RAM�ж�ID          
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
                    case '1': //��ROM�ж�ID           
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
                    case '2': //��EEPROM�ж�ID           
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


                    case '3': //д���ܹ���ID��EEPROM
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
                    case '4': //����mcuID����λ��
                        {
                            uint8_t i;

                            man.mcuID = (char code *)0x7ff9;

                            Uart_SendData(0xA5); //֡ͷ
                            Uart_SendData(0x5A);
                            Uart_SendData(0x08); //���� 8���ֽ�

                            Uart_SendData(0xE4); //����

                            for(i=0;i<7;i++)
                                Uart_SendData(man.mcuID[i]);
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
//**����2
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
////			if(pos!=lcd_que._tail)//����״̬
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
