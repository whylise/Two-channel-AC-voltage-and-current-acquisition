#include "ASSIT.h" 

//任务句柄
TaskHandle_t ASSIT_Task_Handler;
extern QueueHandle_t ASSIT_Message_Queue;//信息队列句柄
extern u8 GPRS_FALUT_STATUS;
extern u8 MTSF;
extern u8 IMEIbuff[18];//IMEI
extern u8 TIMEbuff[25];
u8 err_for_gprs_cnt=0;//HTTP错误时间次数

u16 testasssit=0;

//ASSIT_task任务函数
void ASSIT_task(void *pvParameters)
{
		static u8 LED_FLAG=0,LED_CNT=0;
		u8 i=0,buffer[1];
		BaseType_t err;
    while(1)
    {
			TaskHandle_t TaskHandle;	
				TaskStatus_t TaskStatus;
				TaskHandle=xTaskGetHandle("ASSIT_task");//根据任务名获取任务句柄。
				vTaskGetInfo((TaskHandle_t	)TaskHandle,//任务句柄
				 (TaskStatus_t*	)&TaskStatus,//任务信息结构体
				 (BaseType_t	)pdTRUE,//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);//函数自己获取任务运行壮态
				testasssit=TaskStatus.usStackHighWaterMark;
				if(ASSIT_Message_Queue!=NULL)
               {
						for(i=0;i<1;i++)buffer[i]=0;//清除缓冲区
						err=xQueueReceive(ASSIT_Message_Queue,buffer,0);//请求消息
						if(err==pdTRUE)//接收到消息
						{
								switch(buffer[0])
								{
									case ASSIT_SystemReset://STM32系统复位
											Soft_SystemReset();
											break;
									case UpdataTime:										    
										    Simtimechargetostr(TIMEbuff);																	
										    break;
								}
						}
				}
				if(GPRS_FALUT_STATUS!=0)//GPRS通信存在错误，开始累计错误时间次数
				{
						err_for_gprs_cnt++;
						if(err_for_gprs_cnt>100)err_for_gprs_cnt=1;//防溢出
				}
				else if(err_for_gprs_cnt>0)//GPRS有异常时闪烁，一秒闪一次
				{
						if(LED_FLAG==1)LED_FLAG=0;//IO电平反转
						else if(LED_FLAG==0)LED_FLAG=1;//IO电平反转
						LED_POWER_IO=LED_FLAG;
				}
				else if(MTSF==1)//与探测器通信有异常闪烁，五秒变换一次电平
				{
						LED_CNT++;
						if(LED_CNT>=10)LED_CNT=0;
						if(LED_CNT%5==0)
						{
								if(LED_FLAG==1)LED_FLAG=0;//IO电平反转
								else if(LED_FLAG==0)LED_FLAG=1;//IO电平反转
								LED_POWER_IO=LED_FLAG;
						}
				}
				else//正常工作不亮灯
						LED_POWER_IO=0;
				delay_ms(1000);
    }
}
