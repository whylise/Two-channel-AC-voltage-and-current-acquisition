#include "PC_ASSIT.h"

u16 register_flag=0;

//任务句柄
TaskHandle_t PC_ASSIT_Task_Handler;

extern QueueHandle_t PCASSIT_Message_Queue;
extern u8 cdkeybuffer[32];
extern u8 cdkey_length;
extern u32 HTTP_PCB_ID;
extern u8 LCD1602_IDBUF[24];
extern u8 LCD_CHANGE_NEED;
extern u8 EncryptMode;
extern u8 LCD1602_EncryptModeBuf[8];
//PC_ASSIT_task任务函数
void PC_ASSIT_task(void *pvParameters)
{
		static u8 LED_FLAG=0;
		u8 buffer[1],i=0;
		u16 cdkey_crc16_value=0;
		BaseType_t err;
     while(1)
    {
		if(PCASSIT_Message_Queue!=NULL)
        {
						for(i=0;i<1;i++)buffer[i]=0;//清除缓冲区
						err=xQueueReceive(PCASSIT_Message_Queue,buffer,0);//请求消息PCASSIT_Message_Queue
						if(err==pdTRUE)//接收到消息
						{
								switch(buffer[0])
								{
									case SAVE_CDKEY://计算CDKEY的CRC校验值并保存flash
											break;
								}
						}
				}
				delay_ms(1000);
    }
}
