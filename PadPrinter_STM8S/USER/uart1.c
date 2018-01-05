
/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��uart1.c
 * ����    ��UART1�����ļ�   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 *�޸�ʱ�� ��2012-10-28

  ���STM8������Ӳ������
    |------------------------|
    |  TXD - PA4(UART1->RX)  |
    |  RXD - PA5(UART1->TX)  |
    |------------------------|

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include <stdarg.h>

/* �����Զ���ͷ�ļ� */
#include "stm8s.h"
#include "uart1.h"
#include "simplc_io.h"

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
 * ����: UART1��ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void Uart1_Init(uint32_t baudrate)
{
    UART1_DeInit();		/* ���Ĵ�����ֵ��λ */
	
	/*
	 * ��UART1����Ϊ��
	 * ������ = 115200
	 * ����λ = 8
	 * 1λֹͣλ
	 * ��У��λ
	 * ʹ�ܽ��պͷ���
	 * ʹ�ܽ����ж�
	 */
    UART1_Init((u32)baudrate, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, \
    	UART1_PARITY_NO , UART1_SYNCMODE_CLOCK_DISABLE , UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    UART1_Cmd(ENABLE);
}

/*******************************************************************************
 * ����: UART1_SendByte
 * ����: UART1����һ���ֽ�
 * �β�: data -> Ҫ���͵��ֽ�
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void UART1_SendByte(u8 data)
{
	UART1_SendData8((unsigned char)data);
	
	/* �ȴ�������� */
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

/*******************************************************************************
 * ����: UART1_SendString
 * ����: UART1����len���ַ�
 * �β�: data -> ָ��Ҫ���͵��ַ���
 *       len -> Ҫ���͵��ֽ���
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void UART1_SendString(u8* Data,u16 len)
{
	u16 i=0;
	for(; i < len; i++)
		UART1_SendByte(Data[i]);	/* ѭ�����÷���һ���ַ����� */
}

/*******************************************************************************
 * ����: UART1_ReceiveByte
 * ����: UART1����һ���ַ�
 * �β�: ��
 * ����: ���յ����ַ�
 * ˵��: �� 
 ******************************************************************************/
u8 UART1_ReceiveByte(void)
{
	u8 USART1_RX_BUF; 
	
	/* �ȴ�������� */
	while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET)
	  	;
	
	USART1_RX_BUF = UART1_ReceiveData8();
	
	return  USART1_RX_BUF;
}

/*******************************************************************************
 * ����: fputc
 * ����: �ض���c�⺯��printf��UART1
 * �β�: ��
 * ����: Ҫ��ӡ���ַ�
 * ˵��: ��printf���� 
 ******************************************************************************/
//int fputc(int ch, FILE *f)
//{  
//	/* ��Printf���ݷ������� */
//
//	UART1_SendByte(ch);
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
 * ��������UART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-UARTx ����ͨ��������ֻ�õ��˴���1����UART1
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��	 UART1_printf( "\r\n this is a demo \r\n" );
 *            		 UART1_printf(  "\r\n %d \r\n", i );
 *            		 UART1_printf(  "\r\n %s \r\n", j );
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
					UART1_SendData8(0x0d);
					Data ++;
				break;
	
				case 'n':							          //���з�
					UART1_SendData8(0x0a);	
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
					UART1_SendData8(*s);
					while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
				}
				Data++;
				break;
	
				case 'd':	//ʮ����
					d = va_arg(ap, int);
					itoa(d, buf, 10);
				for (s = buf; *s; s++) 
				{
					UART1_SendData8(*s);
					while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
				}
				Data++;
				break;
				
				default:
					Data++;
				break;
			}		 
		} /* end of else if */
		else UART1_SendData8(*Data++);
	
		while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
	}
}

/**
  * @brief UART1 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
   
   	u8 Res;
	/* ��ȡ���յ�������,���������ݺ��Զ�ȡ��RXNE���жϱ�־λ */
	Res =UART1_ReceiveData8();
	
	/* ����ȡ���ַ����͵�PC */
	//UART1_SendData8(Res);

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
    if(USART_RX_STA&0x8000)
    {					   
        switch(USART_RX_BUF[0])
        {
            case 'X':
                switch(USART_RX_BUF[1])
                {
                    case '0':
                        UART1_printf("X0 = %d\r\n", X0);
                    break;
                    case '1':
                        UART1_printf("X1 = %d\r\n", X1);
                    break;
                    case '2':
                        UART1_printf("X2 = %d\r\n", X2);
                    break;
                    case '3':
                        UART1_printf("X3 = %d\r\n", X3);
                    break;
                    case '4':
                        UART1_printf("X4 = %d\r\n", X4);
                    break;
                    case '5':
                        UART1_printf("X5 = %d\r\n", X5);
                    break;
                    case '6':
                        UART1_printf("X6 = %d\r\n", X6);
                    break;
                    case '7':
                        UART1_printf("X7 = %d\r\n", X7);
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
                        TIM5_Cmd(DISABLE);
                    break;
                    case '1':
                        TIM5_Cmd(ENABLE);//ENABLE
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

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/