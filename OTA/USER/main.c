#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "rtc.h"
#include "iap.h"
#include "stmflash.h"
#include "stdio.h"
#include <string.h> 
/************************************************

************************************************/
struct  bootloader_info_t {
	
	u8  u8download_flag;
	u16 u16applen;
	u16 *iap_buf;
};
struct bootloader_info_t  tbootloader_info; 

struct firmware_info_t{
	
	u16 u16update_flag;
	u16 u16applen;
};
struct firmware_info_t   tfirmware_info;

struct firmwaretest_info_t{
	
	u16 u16updatetest_flag;
};
struct firmwaretest_info_t   tfirmwaretest_info;



void ota_reboot(void);
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);
void write_read_write_flash(u32 u32startaddr, u32 u32applen);
void firmware_setup(u32 addr, u8 *f_buf, u32 f_buf_len);
int main(void)
{	
	u16 oldcount=0;			//��ʼ��RTC 
	u32 u32test = 0;
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */ 
  
	HAL_Init();                    	 	//��ʼ��HAL��    
	Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	LED_Init();							//��ʼ��LED	
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
	usmart_dev.init(84); 		   		//��ʼ��USMART	
	RTC_Init(); 


	POINT_COLOR=RED;		//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"CJQ TEST  ^_^");	
//	LCD_ShowString(30,70,200,16,16,"RTC TEST1000");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2019/10/10");	
	while(RTC_Init())		//RTC��ʼ��	��һ��Ҫ��ʼ���ɹ�
	{ 
		LCD_ShowString(30,130,200,16,16,"RTC ERROR!   ");	
		delay_ms(800);
		LCD_ShowString(30,130,200,16,16,"RTC Trying...");	
	}		    						
	//��ʾʱ��
	POINT_COLOR=BLUE;//��������Ϊ��ɫ					 
	LCD_ShowString(30,130,200,16,16,"    -  -     ");	   
	LCD_ShowString(30,166,200,16,16,"  :  :  ");	 		    
	while(1)
	{	
		if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				tbootloader_info.u16applen=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",tbootloader_info.u16applen);
				tbootloader_info.u8download_flag = 1;
			}else oldcount=USART_RX_CNT;			
		}
		if(tbootloader_info.u8download_flag == 1)
		{

				printf("app2 u32test =  %d", u32test);
			
				tbootloader_info.u8download_flag = 0;
				tfirmware_info.u16update_flag = 1;	
				tfirmware_info.u16applen = tbootloader_info.u16applen;
				printf("bootloader_len = %d", tbootloader_info.u16applen);
			
				FLASH_PageErase(FIRMWARE_INFO);	//�����������
				FLASH_WaitForLastOperation(FLASH_WAITETIME);            	//�ȴ��ϴβ������
				CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
			
				STMFLASH_Write(FIRMWARE_INFO,(u16 *)&tfirmware_info, sizeof(tfirmware_info)/sizeof(u16));	
				
				printf("��ʼд�����APP2 \r\n");
				iap_write_appbin(APP2 , USART_RX_BUF, tbootloader_info.u16applen);
				printf("д��ɹ���\r\n");
				
				memset(&USART_RX_BUF, 0 ,sizeof(USART_RX_BUF));
			
		//		write_read_write_flash(APP2, tbootloader_info.u16applen);
			
				STMFLASH_Read(FIRMWARE_INFO,(u16 *)&tfirmwaretest_info,sizeof(tfirmwaretest_info)/sizeof(u16));
				printf("tfirmware_infoflag = %d\r\n", tfirmwaretest_info.u16updatetest_flag);
				printf("������ɹ�����������ϵͳ\r\n");
				
				ota_reboot();
		
		}
			
		delay_ms(10);								  
	}; 
}

void ota_reboot(void)
{
    INTX_DISABLE();
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
}


u16 iapbuf[1024];
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//ƫ��2���ֽ�
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.  
}

