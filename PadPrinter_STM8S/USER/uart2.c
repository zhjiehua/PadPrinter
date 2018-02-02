
/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：uart2.c
 * 描述    ：UART2设置文件   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-10-28

  风驰STM8开发板硬件连接
    |------------------------|
    |  TXD - PA4(UART2->RX)  |
    |  RXD - PA5(UART2->TX)  |
    |------------------------|

*******************************************************************************/

/* 包含系统头文件 */
#include <stdarg.h>

/* 包含自定义头文件 */
#include "stm8s.h"
#include "uart2.h"
#include "simplc_io.h"
#include "tm1638.h"
#include "project.h"
#include "24cxx.h"

#include "project.h"

/* 自定义新类型 */

/* 自定义宏 */
#define USART_REC_LEN  			20

/* 全局变量定义 */
	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲，最大UASRT_REC_LEN个字节
//接收状态
//bit15，接收完成标志
//bit14，接收到0x0d
//bit13~0,接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记

/*******************************************************************************
 * 名称: Uart_Init
 * 功能: UART2初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Uart2_Init(uint32_t baudrate)
{
    UART2_DeInit();		/* 将寄存器的值复位 */
	
	/*
	 * 将UART2配置为：
	 * 波特率 = 115200
	 * 数据位 = 8
	 * 1位停止位
	 * 无校验位
	 * 使能接收和发送
	 * 使能接收中断
	 */
    UART2_Init((u32)baudrate, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, \
    	UART2_PARITY_NO , UART2_SYNCMODE_CLOCK_DISABLE , UART2_MODE_TXRX_ENABLE);
    UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
    UART2_Cmd(ENABLE);
}

/*******************************************************************************
 * 名称: UART2_SendByte
 * 功能: UART2发送一个字节
 * 形参: data -> 要发送的字节
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART2_SendByte(u8 data)
{
	UART2_SendData8((unsigned char)data);
	
	/* 等待传输结束 */
	while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
}

/*******************************************************************************
 * 名称: UART2_SendString
 * 功能: UART2发送len个字符
 * 形参: data -> 指向要发送的字符串
 *       len -> 要发送的字节数
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART2_SendString(u8* Data,u16 len)
{
	u16 i=0;
	for(; i < len; i++)
		UART2_SendByte(Data[i]);	/* 循环调用发送一个字符函数 */
}

/*******************************************************************************
 * 名称: UART2_ReceiveByte
 * 功能: UART2接收一个字符
 * 形参: 无
 * 返回: 接收到的字符
 * 说明: 无 
 ******************************************************************************/
u8 UART2_ReceiveByte(void)
{
	u8 USART2_RX_BUF; 
	
	/* 等待接收完成 */
	while (UART2_GetFlagStatus(UART2_FLAG_RXNE) == RESET)
	  	;
	
	USART2_RX_BUF = UART2_ReceiveData8();
	
	return  USART2_RX_BUF;
}

/*******************************************************************************
 * 名称: fputc
 * 功能: 重定向c库函数printf到UART2
 * 形参: 无
 * 返回: 要打印的字符
 * 说明: 由printf调用 
 ******************************************************************************/
//int fputc(int ch, FILE *f)
//{  
//	/* 将Printf内容发往串口 */
//
//	UART2_SendByte(ch);
//	
//	return (ch);
//}

/********************************************************
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
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
 * 函数名：UART2_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-UARTx 串口通道，这里只用到了串口1，即UART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用	 UART2_printf( "\r\n this is a demo \r\n" );
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
	
	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					UART2_SendData8(0x0d);
					Data ++;
				break;
	
				case 'n':							          //换行符
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
				case 's':						//字符串
				s = va_arg(ap, const char *);
				for ( ; *s; s++) 
				{
					UART2_SendData8(*s);
					while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
				}
				Data++;
				break;
	
				case 'd':	//十进制
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
	/* 读取接收到的数据,当读完数据后自动取消RXNE的中断标志位 */
	Res =UART2_ReceiveData8();
	
	/* 将读取的字符发送到PC */
	//UART2_SendData8(Res);

    if((USART_RX_STA&0x8000)==0)//接收未完成
    {
        if(USART_RX_STA&0x4000)//接收到了0x0d
        {
            if(Res!=0x0a)
                USART_RX_STA=0;//接收错误，重新开始
            else
                USART_RX_STA|=0x8000;	//接收完成
        }
        else //还没接收到0X0D
        {	
            if(Res==0x0d)
                USART_RX_STA|=0x4000;
            else
            {
                USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                USART_RX_STA++;
                if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误，重新开始接收  
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

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/