
/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��uart2.c
 * ����    ��UART2�����ļ�   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 *�޸�ʱ�� ��2012-10-28

  ���STM8������Ӳ������
    |------------------------|
    |  TXD - PA4(UART2->RX)  |
    |  RXD - PA5(UART2->TX)  |
    |------------------------|

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include <stdarg.h>

/* �����Զ���ͷ�ļ� */
#include "stm8s.h"
#include "uart2.h"
#include "simplc_io.h"
#include "tm1638.h"
#include "project.h"
#include "24cxx.h"

#include "project.h"

/* �Զ��������� */

/* �Զ���� */
#define USART_REC_LEN  			20

/* ȫ�ֱ������� */
	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ��壬���UASRT_REC_LEN���ֽ�
//����״̬
//bit15��������ɱ�־
//bit14�����յ�0x0d
//bit13~0,���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���

/*******************************************************************************
 * ����: Uart_Init
 * ����: UART2��ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Uart2_Init(uint32_t baudrate)
{
    UART2_DeInit();		/* ���Ĵ�����ֵ��λ */
	
	/*
	 * ��UART2����Ϊ��
	 * ������ = 115200
	 * ����λ = 8
	 * 1λֹͣλ
	 * ��У��λ
	 * ʹ�ܽ��պͷ���
	 * ʹ�ܽ����ж�
	 */
    UART2_Init((u32)baudrate, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, \
    	UART2_PARITY_NO , UART2_SYNCMODE_CLOCK_DISABLE , UART2_MODE_TXRX_ENABLE);
    UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
    UART2_Cmd(ENABLE);
}

/*******************************************************************************
 * ����: UART2_SendByte
 * ����: UART2����һ���ֽ�
 * �β�: data -> Ҫ���͵��ֽ�
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void UART2_SendByte(u8 data)
{
	UART2_SendData8((unsigned char)data);
	
	/* �ȴ�������� */
	while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
}

/*******************************************************************************
 * ����: UART2_SendString
 * ����: UART2����len���ַ�
 * �β�: data -> ָ��Ҫ���͵��ַ���
 *       len -> Ҫ���͵��ֽ���
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void UART2_SendString(u8* Data,u16 len)
{
	u16 i=0;
	for(; i < len; i++)
		UART2_SendByte(Data[i]);	/* ѭ�����÷���һ���ַ����� */
}

/*******************************************************************************
 * ����: UART2_ReceiveByte
 * ����: UART2����һ���ַ�
 * �β�: ��
 * ����: ���յ����ַ�
 * ˵��: �� 
 ******************************************************************************/
u8 UART2_ReceiveByte(void)
{
	u8 USART2_RX_BUF; 
	
	/* �ȴ�������� */
	while (UART2_GetFlagStatus(UART2_FLAG_RXNE) == RESET)
	  	;
	
	USART2_RX_BUF = UART2_ReceiveData8();
	
	return  USART2_RX_BUF;
}

/*******************************************************************************
 * ����: fputc
 * ����: �ض���c�⺯��printf��UART2
 * �β�: ��
 * ����: Ҫ��ӡ���ַ�
 * ˵��: ��printf���� 
 ******************************************************************************/
//int fputc(int ch, FILE *f)
//{  
//	/* ��Printf���ݷ������� */
//
//	UART2_SendByte(ch);
//	
//	return (ch);
//}

/********************************************************
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART1_printf()����
 *******************************************************/
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/************************************************************************
 * ��������UART2_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-UARTx ����ͨ��������ֻ�õ��˴���1����UART2
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��	 UART2_printf( "\r\n this is a demo \r\n" );
 *            		 UART2_printf(  "\r\n %d \r\n", i );
 *            		 UART2_printf(  "\r\n %s \r\n", j );
 ***************************************************************************/
void UART1_printf( uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];
	va_list ap;
	va_start(ap, Data);
	
	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					UART2_SendData8(0x0d);
					Data ++;
				break;
	
				case 'n':							          //���з�
					UART2_SendData8(0x0a);	
					Data ++;
				break;
	
				default:
					Data ++;
				break;
			}			 
		}
		else if ( *Data == '%')
		{					//
			switch ( *++Data )
			{				
				case 's':						//�ַ���
				s = va_arg(ap, const char *);
				for ( ; *s; s++) 
				{
					UART2_SendData8(*s);
					while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
				}
				Data++;
				break;
	
				case 'd':	//ʮ����
					d = va_arg(ap, int);
					itoa(d, buf, 10);
				for (s = buf; *s; s++) 
				{
					UART2_SendData8(*s);
					while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
				}
				Data++;
				break;
				
				default:
					Data++;
				break;
			}		 
		} /* end of else if */
		else UART2_SendData8(*Data++);
	
		while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
	}
}

/**
  * @brief UART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
   
   	u8 Res;
	/* ��ȡ���յ�������,���������ݺ��Զ�ȡ��RXNE���жϱ�־λ */
	Res =UART2_ReceiveData8();
	
	/* ����ȡ���ַ����͵�PC */
	//UART2_SendData8(Res);

    if((USART_RX_STA&0x8000)==0)//����δ���
    {
        if(USART_RX_STA&0x4000)//���յ���0x0d
        {
            if(Res!=0x0a)
                USART_RX_STA=0;//���մ������¿�ʼ
            else
                USART_RX_STA|=0x8000;	//�������
        }
        else //��û���յ�0X0D
        {	
            if(Res==0x0d)
                USART_RX_STA|=0x4000;
            else
            {
                USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                USART_RX_STA++;
                if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ������¿�ʼ����  
            }		 
        }
    }	
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
                        UART1_printf("X0 = %d\r\n", X0);
                        UART1_printf("GXL(X0) = %d\r\n", GXL(X_ABSORB_O));
                    break;
                    case '1':
                        UART1_printf("X1 = %d\r\n", X1);
                        UART1_printf("GXL(X1) = %d\r\n", GXL(X_ABSORB_L));
                    break;
                    case '2':
                        UART1_printf("X2 = %d\r\n", X2);
                        UART1_printf("GXL(X2) = %d\r\n", GXL(X_PRINT_O));
                    break;
                    case '3':
                        UART1_printf("X3 = %d\r\n", X3);
                        UART1_printf("GXL(X3) = %d\r\n", GXL(X_PRINT_L));
                    break;
                    case '4':
                        UART1_printf("X4 = %d\r\n", X4);
                        UART1_printf("GXL(X4) = %d\r\n", GXL(X_SHIFT_O));
                    break;
                    case '5':
                        UART1_printf("X5 = %d\r\n", X5);
                        UART1_printf("GXL(X5) = %d\r\n", GXL(X_POS));
                    break;
                    case '6':
                        UART1_printf("X6 = %d\r\n", X6);
                        UART1_printf("GXL(X6) = %d\r\n", GXL(X_SHIFT_L1));
                    break;
                    case '7':
                        UART1_printf("X7 = %d\r\n", X7);
                        UART1_printf("GXL(X7) = %d\r\n", GXL(X_SHIFT_L2));
                    break;
                    case '8':
                        UART1_printf("X8 = %d\r\n", GPIO_ReadInputPin(IN_SENSOR_LEVEL_PORT, IN_SENSOR_LEVEL_PIN));
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
                    default:
                    break;
                }
            break;
            case 'B':
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        TIM2_Cmd(DISABLE);
                    break;
                    case '1':
                        TIM2_Cmd(ENABLE);//ENABLE
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
                        UART1_printf("Reset default data!\r\n");
                    break;
                    default:
                    break;
                }
            break;
            case 'G':
//                switch(USART_RX_BUF[1])
//                {
//                    case '0':
//                        UART1_printf("EEPROM_ADDR_PROGRAM0 = %d\r\n", EEPROM_ADDR_PROGRAM0);
//                    break;
//                    case '1':
//                        UART1_printf("EEPROM_ADDR_PROGRAM1 = %d\r\n", EEPROM_ADDR_PROGRAM1);
//                    break;
//                    case '2':
//                        UART1_printf("EEPROM_ADDR_PROGRAM2 = %d\r\n", EEPROM_ADDR_PROGRAM2);
//                    break;
//                    case '3':
//                        UART1_printf("EEPROM_ADDR_PROGRAM3 = %d\r\n", EEPROM_ADDR_PROGRAM3);
//                    break;
//                    default:
//                    break;
//                }
            break;
            default:
            break;
        }
        USART_RX_STA=0;
    }
}

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/