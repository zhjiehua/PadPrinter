#include "management.h"
#include "misc.h"
#include "project.h"

#include "common.h"
#include "io.h"
#include "uart.h"
#include "tm1638.h"
#include "24cxx.h"
#include "timer.h"

#include "simplc_io.h"
#include "simplc_timer.h"


void FactoryMode(void)
{
	static uint8_t  IODetectionSwitchFlag = 0;		  //IO检测切换标志         1:输入检测		  0:输出检测
	static uint8_t	InputDetectionSwitchFlag = 0;	  //输入检测切换标志	  1:传感器组1检测	  0:传感器组2检测

	static uint8_t  ledControl = 0;
	static uint8_t  IOTestPreparationFlag = 0;		  // IO检测准备标志 用于熄灭所有的灯	
    static uint8_t  led = 0;
	static uint16_t DisplayTime = 0;

	static uint8_t  DetectionMode = 0;

	
	if(GMR(M_KEY_AUX)||GMR(M_KEY_FOOT)) 
	{
			 
		DetectionMode++;									 //	输入输出检测模式切换*************************
		if(DetectionMode>3)
	    {
            DetectionMode=1;
		} 

		Delay10us();
		Delay10us();					  			  
	}
	
    if(DetectionMode == 0)
    {
        TM1638_SendData(0,"r----" );
    }	
	//输出检测模式（led，数码管闪烁）****************************************************************************
	else if(DetectionMode == 1)		
	{	 
		IOTestPreparationFlag = 0;

		DisplayTime++;
		if(DisplayTime == 1000)
		{
		    led = !led;	
			DisplayTime = 0;	 
		} 

	    SYL(0, !led);
		SYL(1, !led);
		SYL(2, !led);
		SYL(3, !led);
		SYL(4, !led);
		SYL(5, !led);
		
		if(led == 0)
		{				
			TM1638_SendData(0, "8.8.8.8.8.");			
			TM1638_LEDIndependentControl(5, (uint16_t)0xff );
		}
		else
		{		
			TM1638_SendData(0,"     ");
			TM1638_LEDIndependentControl(5, (uint16_t)0x00 );							
		}	
	}
	//传感器组1检测模式********************************************************************************************
	else if(DetectionMode == 2)	 
	{
		//熄灭所有灯***************
		if(IOTestPreparationFlag == 0)	 
		{
		
			SYL(0, 0);
			SYL(1, 0);
			SYL(2, 0);
			SYL(3, 0);
			SYL(4, 0);
			
			TM1638_SendData(0,"     " );
			
			TM1638_LEDIndependentControl(5, (uint16_t)0x00 );
		
		}
		IOTestPreparationFlag = 1;

		//按键检测****************
		if(GML(M_KEY_SLOW))      {ledControl |= 0x02;}	  else{ledControl &= (~0x02);}	   
		if(GML(M_KEY_UPDOWN))    {ledControl |= 0x08;}	  else{ledControl &= (~0x08);}	 
		if(GML(M_KEY_FRONTBACK)) {ledControl |= 0x10;}	  else{ledControl &= (~0x10);}	 
		if(GML(M_KEY_STARTSTOP)) {ledControl |= 0x20;}	  else{ledControl &= (~0x20);}	 
		if(GML(M_KEY_FOOT ))     {SYL(5, 1);}	          else{SYL(5, 0);}	//ledControl|=0x04;		 ledControl&=(~0x04);
		if(GML(M_KEY_FAST))      {ledControl |= 0x01;}	  else{ledControl &= (~0x01);}	
		
		TM1638_LEDIndependentControl(5, (uint16_t)(ledControl) )	;	//对应led灯点亮	 
		
		//传感器检测***************
		
		TM1638_SendData(0,"--1--" );
		if(X0)	{SYL(Y_UPDOWN, 1);}	      else{SYL(Y_UPDOWN, 0);}
		if(X1)	{SYL(Y_FRONTBACK, 1);}	  else{SYL(Y_FRONTBACK, 0);}
		if(X2)	{SYL(Y_SHIFT, 1);}	      else{SYL(Y_SHIFT, 0);}
		if(X3)	{SYL(Y_POS, 1);}	      else{SYL(Y_POS, 0);}
		if(X8)	{SYL(Y_SCRAPER, 1);}	  else{SYL(Y_SCRAPER, 0);}
	
	}
	//传感器组2检测模式**********************************************************************************************	
	else if(DetectionMode == 3)
    {
		TM1638_SendData(0,"--2--" );
		if(X4)	{SYL(Y_UPDOWN, 1);}	      else{SYL(Y_UPDOWN, 0);}
		if(X5)	{SYL(Y_FRONTBACK, 1);}	  else{SYL(Y_FRONTBACK, 0);}
		if(X6)	{SYL(Y_SHIFT, 1);}	      else{SYL(Y_SHIFT, 0);}
		if(X7)	{SYL(Y_POS, 1);}	      else{SYL(Y_POS, 0);}

		if(GML(M_KEY_FOOT)) {SYL(5, 1);}	          else{ SYL(5, 0);}
    }	    
}
