#include "PC_ASSIT.h"

u16 register_flag=0;

//������
TaskHandle_t PC_ASSIT_Task_Handler;

extern QueueHandle_t PCASSIT_Message_Queue;
extern u8 cdkeybuffer[32];
extern u8 cdkey_length;
extern u32 HTTP_PCB_ID;
extern u8 LCD1602_IDBUF[24];
extern u8 LCD_CHANGE_NEED;
extern u8 EncryptMode;
extern u8 LCD1602_EncryptModeBuf[8];
//PC_ASSIT_task������
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
						for(i=0;i<1;i++)buffer[i]=0;//���������
						err=xQueueReceive(PCASSIT_Message_Queue,buffer,0);//������ϢPCASSIT_Message_Queue
						if(err==pdTRUE)//���յ���Ϣ
						{
								switch(buffer[0])
								{
									case SAVE_CDKEY://����CDKEY��CRCУ��ֵ������flash
											break;
								}
						}
				}
				delay_ms(1000);
    }
}
