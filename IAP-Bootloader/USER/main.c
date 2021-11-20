#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
#include <string.h>
#include <firmware_download.h>

/************************************************
 firm_download 下载程序
 
************************************************/

int main(void)
{	
	
	HAL_Init();                    	    
  Stm32_Clock_Init(RCC_PLL_MUL9);   
	delay_init(72);               
	uart_init(115200);				
	firm_init();
	while(1)
	{	
		firm_download(); 
	}   	   
}



