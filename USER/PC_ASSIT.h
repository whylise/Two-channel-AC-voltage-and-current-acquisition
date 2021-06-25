#ifndef __PC_ASSIT_H
#define __PC_ASSIT_H 	

#include "all.h" 
#include "PC_ASSIT.h"

//任务优先级
#define PC_ASSIT_TASK_PRIO		6
//任务堆栈大小	
#define PC_ASSIT_STK_SIZE 		FreeRTOS_ASSIT_STK_SIZE 
//任务函数
void PC_ASSIT_task(void *pvParameters);

#define SAVE_CDKEY 0x11

#endif
