#include "all.h"

#define  Max_The_Number_Of_FTProjects 64//邮箱最大长度
#define  Max_The_Number_Of_GSMProjects 64//邮箱最大长度
#define  Max_The_Number_Of_ASSITProjects 64//邮箱最大长度
#define  Max_The_Number_Of_PCASSITProjects 64//邮箱最大长度

extern TaskHandle_t GSM_Task_Handler;
extern TaskHandle_t ASSIT_Task_Handler;
extern TaskHandle_t MAIN_Task_Handler;
extern TaskHandle_t REALTIME_Task_Handler;
extern TaskHandle_t P_REALTIME_Task_Handler;
extern TaskHandle_t P_GSM_Task_Handler;
extern TaskHandle_t PC_Task_Handler;
extern TaskHandle_t PC_ASSIT_Task_Handler;

QueueHandle_t GSM_Message_Queue;//信息队列句柄
QueueHandle_t FT_Message_Queue;//信息队列句柄
QueueHandle_t ASSIT_Message_Queue;//信息队列句柄
QueueHandle_t P_GSM_Message_Queue;//信息队列句柄
QueueHandle_t PCASSIT_Message_Queue;//信息队列句柄

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
u8 check_mode_IO=0;//检测CHECK模式对应的IO状态
void start_task(void *pvParameters);

//任务优先级
#define PRODUCT_TASK_PRIO		6
//任务堆栈大小	
#define PRODUCT_STK_SIZE 		128
//任务句柄
TaskHandle_t PRODUCTTask_Handler;
//任务函数
void PRODUCT_task(void *pvParameters);

int main(void)
{	 
		 SCB->VTOR = FLASH_BASE | 0x2800; /* APP1 RUN ADDRESS. */
		//SCB->VTOR = FLASH_BASE | 0xC800; /* APP2 RUN ADDRESS. */	
	
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
		delay_init();//延时函数初始化	 
		delay_ms(500);//开机延时0.5s
		GSM_Power_Init();//控制GSM模块电源IO初始化
		CHECK_IO_Init();//检测模式IO初始化
		LED_IO_Init();//指示灯LED初始化
		uart1_init(115200);//串口1初始化
		InitAndCheckFlash();	
		IWDG_Init(4,2500);//看门狗初始化，与分频数为64,重载值为625,溢出时间为1s,在线调试要关闭		
		if(Check_IO==0)//检测IO是否为低电平
		{
				delay_ms(10);//消抖
				if(Check_IO==0)//再次检测IO是否为低电平
					check_mode_IO=1;//进入生产模式
		}
		else
				check_mode_IO=0;//进入正常工作模式
		if(check_mode_IO==1)
		{
				uart2_init(9600);//串口2初始化 LCD1602的波特率是9600
				InitAndCheckFlash();//首次开机Flash检查
				//创建生产任务 
				xTaskCreate((TaskFunction_t )PRODUCT_task,            //任务函数
										(const char*    )"PRODUCT_task",          //任务名称
										(uint16_t       )PRODUCT_STK_SIZE,        //任务堆栈大小
										(void*          )NULL,                    //传递给任务函数的参数
										(UBaseType_t    )PRODUCT_TASK_PRIO,       //任务优先级
										(TaskHandle_t*  )&PRODUCTTask_Handler);   //任务句柄
		}
		else
		{
				uart2_init(115200);//串口2初始化
				//创建正常工作任务
				xTaskCreate((TaskFunction_t )start_task,            //任务函数
										(const char*    )"start_task",          //任务名称
										(uint16_t       )START_STK_SIZE,        //任务堆栈大小
										(void*          )NULL,                  //传递给任务函数的参数
										(UBaseType_t    )START_TASK_PRIO,       //任务优先级
										(TaskHandle_t*  )&StartTask_Handler);   //任务句柄
		}									
		vTaskStartScheduler();//开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();//进入临界区
    //创建消息队列
		FT_Message_Queue=xQueueCreate(Max_The_Number_Of_FTProjects,1);//创建消息FT_Message_Queue,项目数Max_The_Number_Of_FTProjects,每个项目长度1
		GSM_Message_Queue=xQueueCreate(Max_The_Number_Of_GSMProjects,1);//创建消息GSM_Message_Queue,项目数Max_The_Number_Of_FTProjects,每个项目长度1
		ASSIT_Message_Queue=xQueueCreate(Max_The_Number_Of_ASSITProjects,1);//创建消息ASSIT_Message_Queue,项目Max_The_Number_Of_ASSITProjects,每个项目长度1
	  //创建REALTIME任务
    xTaskCreate((TaskFunction_t )REALTIME_task,     (const char*    )"REALTIME_task",   	
                (uint16_t       )REALTIME_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )REALTIME_TASK_PRIO,(TaskHandle_t*  )&REALTIME_Task_Handler);
		//创建MAIN任务
    xTaskCreate((TaskFunction_t )MAIN_task,     (const char*    )"MAIN_task",   
                (uint16_t       )MAIN_STK_SIZE, (void*          )NULL,
                (UBaseType_t    )MAIN_TASK_PRIO,(TaskHandle_t*  )&MAIN_Task_Handler); 
	  //创建GSM任务
    xTaskCreate((TaskFunction_t )GSM_task,     (const char*    )"GSM_task",   	
                (uint16_t       )GSM_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )GSM_TASK_PRIO,(TaskHandle_t*  )&GSM_Task_Handler);   
   //创建ASSIT任务
    xTaskCreate((TaskFunction_t )ASSIT_task,     (const char*    )"ASSIT_task",   
                (uint16_t       )ASSIT_STK_SIZE, (void*          )NULL,
                (UBaseType_t    )ASSIT_TASK_PRIO,(TaskHandle_t*  )&ASSIT_Task_Handler);    								
    vTaskDelete(StartTask_Handler);//删除开始任务
    taskEXIT_CRITICAL();//退出临界区
}

//开始任务任务函数
void PRODUCT_task(void *pvParameters)
{
		taskENTER_CRITICAL();//进入临界区
		P_GSM_Message_Queue=xQueueCreate(Max_The_Number_Of_GSMProjects,1);//创建消息FT_Message_Queue,项目数Max_The_Number_Of_FTProjects,每个项目长度1
		PCASSIT_Message_Queue=xQueueCreate(Max_The_Number_Of_PCASSITProjects,1);//创建消息FT_Message_Queue,项目64,每个项目长度1
		//创建P_REALTIME任务
		xTaskCreate((TaskFunction_t )P_REALTIME_task,     (const char*    )"P_REALTIME_task",   	
                (uint16_t       )P_REALTIME_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )P_REALTIME_TASK_PRIO,(TaskHandle_t*  )&P_REALTIME_Task_Handler);
		//创建PC任务
    xTaskCreate((TaskFunction_t )PC_task,     (const char*    )"PC_task",   
                (uint16_t       )PC_STK_SIZE, (void*          )NULL,
                (UBaseType_t    )PC_TASK_PRIO,(TaskHandle_t*  )&PC_Task_Handler);						
		//创建P_GSM任务
    xTaskCreate((TaskFunction_t )P_GSM_task,     (const char*    )"P_GSM_task",   	
                (uint16_t       )P_GSM_STK_SIZE, (void*          )NULL,				
                (UBaseType_t    )P_GSM_TASK_PRIO,(TaskHandle_t*  )&P_GSM_Task_Handler); 							
    vTaskDelete(PRODUCTTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();//退出临界区
}




