#ifndef __PC_H
#define __PC_H 	

#include "all.h" 
#include "PC.h"

//任务优先级
#define PC_TASK_PRIO		7
//任务堆栈大小	
#define PC_STK_SIZE 		FreeRTOS_Main_STK_SIZE 


#define NOSIM  		0x0010
#define SIMOK       0x0011
#define NOGPRS		0x0012
#define GPRSOK      0x0013
#define INITOK      0x0014
#define IMEIFLAG    0x0015
#define IPOK 		0x0016
#define FAILIP		0x0017
#define POSTOK      0x0009
#define FAILPOST	0x0019
#define MNFLAG    	0x0020
#define TIMEFLAG  	0x0021
//任务函数
void PC_task(void *pvParameters);

#endif
