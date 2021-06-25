#include "ASSIT.h" 

//������
TaskHandle_t ASSIT_Task_Handler;
extern QueueHandle_t ASSIT_Message_Queue;//��Ϣ���о��
extern u8 GPRS_FALUT_STATUS;
extern u8 MTSF;
extern u8 IMEIbuff[18];//IMEI
extern u8 TIMEbuff[25];
u8 err_for_gprs_cnt=0;//HTTP����ʱ�����

u16 testasssit=0;

//ASSIT_task������
void ASSIT_task(void *pvParameters)
{
		static u8 LED_FLAG=0,LED_CNT=0;
		u8 i=0,buffer[1];
		BaseType_t err;
    while(1)
    {
			TaskHandle_t TaskHandle;	
				TaskStatus_t TaskStatus;
				TaskHandle=xTaskGetHandle("ASSIT_task");//������������ȡ��������
				vTaskGetInfo((TaskHandle_t	)TaskHandle,//������
				 (TaskStatus_t*	)&TaskStatus,//������Ϣ�ṹ��
				 (BaseType_t	)pdTRUE,//����ͳ�������ջ��ʷ��Сʣ���С
			     (eTaskState	)eInvalid);//�����Լ���ȡ��������׳̬
				testasssit=TaskStatus.usStackHighWaterMark;
				if(ASSIT_Message_Queue!=NULL)
               {
						for(i=0;i<1;i++)buffer[i]=0;//���������
						err=xQueueReceive(ASSIT_Message_Queue,buffer,0);//������Ϣ
						if(err==pdTRUE)//���յ���Ϣ
						{
								switch(buffer[0])
								{
									case ASSIT_SystemReset://STM32ϵͳ��λ
											Soft_SystemReset();
											break;
									case UpdataTime:										    
										    Simtimechargetostr(TIMEbuff);																	
										    break;
								}
						}
				}
				if(GPRS_FALUT_STATUS!=0)//GPRSͨ�Ŵ��ڴ��󣬿�ʼ�ۼƴ���ʱ�����
				{
						err_for_gprs_cnt++;
						if(err_for_gprs_cnt>100)err_for_gprs_cnt=1;//�����
				}
				else if(err_for_gprs_cnt>0)//GPRS���쳣ʱ��˸��һ����һ��
				{
						if(LED_FLAG==1)LED_FLAG=0;//IO��ƽ��ת
						else if(LED_FLAG==0)LED_FLAG=1;//IO��ƽ��ת
						LED_POWER_IO=LED_FLAG;
				}
				else if(MTSF==1)//��̽����ͨ�����쳣��˸������任һ�ε�ƽ
				{
						LED_CNT++;
						if(LED_CNT>=10)LED_CNT=0;
						if(LED_CNT%5==0)
						{
								if(LED_FLAG==1)LED_FLAG=0;//IO��ƽ��ת
								else if(LED_FLAG==0)LED_FLAG=1;//IO��ƽ��ת
								LED_POWER_IO=LED_FLAG;
						}
				}
				else//��������������
						LED_POWER_IO=0;
				delay_ms(1000);
    }
}
