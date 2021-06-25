#include "REALTIME.h" 
#include "LCD.h"

//任务句柄
TaskHandle_t REALTIME_Task_Handler;

extern QueueHandle_t GSM_Message_Queue;	//信息队列句柄
extern QueueHandle_t ASSIT_Message_Queue;
extern u8 succeed_GSM_INIT;
extern u8 PushMode;
extern u8 sms_cmd_time_start;
extern u8 updataing;
extern u8 TCPIP_INIT_status;
extern u8 TIMEbuff[25];
extern NETWROKTIME  Netwroktime;
extern u8 LCD_CHANGE_NEED;//LCD显示更新变量 0不需要 1需要
extern u8 GETDATA_Succeed;

u16 t_sec=0,t_min=0,t_hour=0,t_day=0;
u16 last_t_sec=0,last_t_min=0,last_t_hour=0,last_t_day=0;
u16 lastNetsec=0,lastNetmin=0,lastNethour=0,lastNetday=0;
u16 sms_sec=0;

u16 testwdg=0;

u8 DownFlash_EM_BIT=0;

//REALTIME任务函数
void REALTIME_task(void *pvParameters)
{
	static u8 cnt_sec=0;
	u8 Queue_BUFF=0;
	volatile BaseType_t err;
    while(1)
    {
			
			  TaskHandle_t TaskHandle;	
				TaskStatus_t TaskStatus;
				TaskHandle=xTaskGetHandle("REALTIME_task");//根据任务名获取任务句柄。
				vTaskGetInfo((TaskHandle_t	)TaskHandle,//任务句柄
				(TaskStatus_t*	)&TaskStatus,//任务信息结构体
				(BaseType_t	)pdTRUE,//允许统计任务堆栈历史最小剩余大小
			    (eTaskState	)eInvalid);//函数自己获取任务运行壮态
				testwdg=TaskStatus.usStackHighWaterMark;
			
				cnt_sec++;
				
				if(cnt_sec%2==0)
		        {
					IWDG_Feed();//喂狗
					t_sec++;//1秒
				}
				if(t_sec>=59){t_sec=0;t_min++;}//1分钟
				if(t_min>=59){t_min=0;t_hour++;}//1小时
				if(t_hour>=24){t_hour=0;t_day++;}//一天
				if(t_day>=7)t_day=0;//一周
				if(TCPIP_INIT_status==1)//GSM模块初始化成功后
				{				    
						if(Netwroktime.Minute%2==0&&Netwroktime.Minute!=lastNetmin)
						{
								Queue_BUFF=POST_HTTP_BasicData;
								err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//上传基本数据
						}
						if(Netwroktime.Hour%12==0&&Netwroktime.Hour!=lastNethour)
						{
								DownFlash_EM_BIT=1;
						}
						if(t_sec!=last_t_sec&&t_sec%40==0)
						{	
							    Queue_BUFF=GetSingal;
								  err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//查阅信号强度
						}
						
				}
				
				
				
				lastNetsec =Netwroktime.Second;
				lastNetmin =Netwroktime.Minute;
				lastNethour=Netwroktime.Hour;
				lastNetday =Netwroktime.Day;
				last_t_sec=t_sec;
				last_t_min=t_min;
				last_t_hour=t_hour;
				last_t_day=t_day;
				if(cnt_sec>=100)cnt_sec=0;
                delay_ms(500);
    }
}

//任务句柄
TaskHandle_t P_REALTIME_Task_Handler;
//REALTIME任务函数
void P_REALTIME_task(void *pvParameters)
{
	static u8 cnt_sec=0;
    while(1)
    {
			    IWDG_Feed();//喂狗
				cnt_sec++;
				if(cnt_sec%2==0)t_sec++;//1秒
				if(t_sec>=60){t_sec=0;t_min++;}//1分钟
				if(t_min>=60){t_min=0;t_hour++;}//1小时
				if(t_hour>=24){t_hour=0;t_day++;}//一天
				if(t_day>=7)t_day=0;//一周
				
	      if(t_sec!=last_t_sec&&t_sec%2==0)
				{
						LCD_CHANGE_NEED=1;
						if(GETDATA_Succeed==1)
						{
							LCD_CHANGE_NEED=0;
							LCD_SHOW_POSTOK();
						}
						else if(GETDATA_Succeed	==2)
						{
							LCD_SHOW_RESET();
						}
				}
					
				last_t_sec=t_sec;
				last_t_min=t_min;
				last_t_hour=t_hour;
				last_t_day=t_day;
				if(cnt_sec>=100)cnt_sec=0;
				delay_ms(500);
    }
}


