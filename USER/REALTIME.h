#ifndef __REALTIME_H
#define __REALTIME_H 	

#include "all.h" 
#include "REALTIME.h"

//任务优先级
#define REALTIME_TASK_PRIO		5
//任务堆栈大小	
#define REALTIME_STK_SIZE 		FreeRTOS_REALTIME_STK_SIZE  
//任务函数
void REALTIME_task(void *pvParameters);

//任务优先级
#define P_REALTIME_TASK_PRIO		10
//任务堆栈大小	
#define P_REALTIME_STK_SIZE 		FreeRTOS_REALTIME_STK_SIZE  
//任务函数
void P_REALTIME_task(void *pvParameters);

#endif

