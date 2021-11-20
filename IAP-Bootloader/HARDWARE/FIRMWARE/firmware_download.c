#include <firmware_download.h>
#include "sys.h"
#include <string.h>
#include "iap.h"
#include "usart.h"	
#include "stmflash.h"


/************************************************
author: CJQ2805		
		2021/04
************************************************/

struct firmware_info_t{
	
	u16 u16update_flag;
	u16 u16applen;
};
struct firmware_info_t   gt_firmware_info;


void firm_download(void)
{
		STMFLASH_Read(FIRMWARE_INFO,(u16*)&gt_firmware_info,sizeof(gt_firmware_info)/sizeof(u16));				
		printf("firmware_info = %d \r\n",gt_firmware_info.u16update_flag);
		
		if(gt_firmware_info.u16update_flag == 1)
		{
			printf("firmware_info_applen = %d \r\n",gt_firmware_info.u16applen);
			gt_firmware_info.u16update_flag = 0;
			STMFLASH_Write(FIRMWARE_INFO,(u16 *)&gt_firmware_info, sizeof(gt_firmware_info)/sizeof(u16));
			printf("读取到标志位，进行升级\r\n");

			write_read_write_flash(APP2, (u32)gt_firmware_info.u16applen);
			printf("写入完成，等待运行\r\n");	
			iap_load_app(APP1);

		}
		else{
			printf("进入else");
			iap_load_app(APP1);
		}	

}



void write_read_write_flash(u32 u32startaddr, u32 u32applen)
{
		u32 u32write_num = 0;
		u16 u16temp = 0;
		u32 u32add = 0;
		u32 u32last_add = 0;
		u32 u32address = APP2;
		INTX_DISABLE();
		HAL_FLASH_Unlock();	

		u32last_add	 =	u32applen % 2;
		u32write_num  = u32applen - u32last_add;	

		for(u32 i = 0; i < u32write_num; i += 2)
		{
				printf("循环次数： i= %d", i);
				STMFLASH_Read(u32address, &u16temp, 1);

				rx_buff[u32add + 1] = ((u16temp>>8)&0xFF);
				rx_buff[u32add] = ((u16temp)&0xFF);

				u32add += 2;
				u32address += 2;

		}	
			STMFLASH_Read(u32address, &u16temp, 1);
			for(u8 i = 0; i<u32last_add; i++)
			{
				rx_buff[u32add + i ] = ((u16temp >> (8 * i)) & 0xFF);
			
			}
			u32add += u32last_add;
			printf("长度为：%d", u32add);
			
			firmware_setup(APP1, rx_buff,  u32add);
			
			
}


void firmware_setup(u32 u32addr, u8 *u8f_buf, u32 u32f_buf_len)
{
	if(u32f_buf_len == 0)
	{
		return;
	}
	
 	if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
	{	
			iap_write_appbin( u32addr, u8f_buf, u32f_buf_len);	
			printf("下载程序成功\r\n");
	}
	else{
	
			printf("flash固件错误！\r\n");
	}
}


void firm_init(void)
{
		memset(&gt_firmware_info, 0 ,sizeof(gt_firmware_info));
}
