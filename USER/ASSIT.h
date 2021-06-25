#ifndef __ASSIT_H
#define __ASSIT_H 	

#include "all.h" 
#include "ASSIT.h"

//任务优先级
#define ASSIT_TASK_PRIO		2
//任务堆栈大小	
#define ASSIT_STK_SIZE 		FreeRTOS_ASSIT_STK_SIZE 
//任务函数
void ASSIT_task(void *pvParameters);

#define ASSIT_SystemReset 0x01

#endif
