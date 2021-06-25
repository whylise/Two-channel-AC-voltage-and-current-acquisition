#include "PC.h"

extern u16 register_flag;
extern u8	CDKEY_MOTHOD_bit;
extern QueueHandle_t P_GSM_Message_Queue;	//��Ϣ���о��
extern u8 TIMEbuff[25];
//������
TaskHandle_t PC_Task_Handler;

u8 LCD_CHANGE_NEED=0;//LCD��ʾ���±��� 0����Ҫ 1��Ҫ
extern u8 SimInsertGsmStatus;
extern u8 Gsm_init_Status;
//PC_task������
void PC_task(void *pvParameters)
{
    while(1)
    {
				if(LCD_CHANGE_NEED==1)//�ж��Ƿ���Ҫˢ����ʾ
				{
					switch(register_flag)
					{
						case NOSIM:
							LCD_SHOW_NO_SIM();
						    delay_ms(1000);
							break;
						case SIMOK:
							LCD_SHOW_SIMOK();
							delay_ms(1000);
							break;
						case NOGPRS:
							LCD_SHOW_NOGPRS();
							delay_ms(1000);
							break;
						case GPRSOK:
							LCD_SHOW_GPRSOK();
							delay_ms(1000);
							break;
						case IMEIFLAG:
							LCD_SHOW_IMEI();
							delay_ms(1000);
							break;
						case IPOK:
							LCD_SHOW_IPOK();
							delay_ms(5000);
						  break;
						case POSTOK:
							LCD_SHOW_POSTOK();
							delay_ms(1000);
							break;
						case FAILIP:
							LCD_SHOW_FAILIP();
							delay_ms(1000);
							break;
						case TIMEFLAG:
							LCD_SHOW_TIME();
							delay_ms(1000);
							break;
						default:
							LCD_SHOW_LOADING();						
							break;
					}
					LCD_CHANGE_NEED=0;
				}
				delay_ms(100);
    }
}

