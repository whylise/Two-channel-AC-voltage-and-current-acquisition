#ifndef __REALTIME_H
#define __REALTIME_H 	

#include "all.h" 
#include "REALTIME.h"

//�������ȼ�
#define REALTIME_TASK_PRIO		5
//�����ջ��С	
#define REALTIME_STK_SIZE 		FreeRTOS_REALTIME_STK_SIZE  
//������
void REALTIME_task(void *pvParameters);

//�������ȼ�
#define P_REALTIME_TASK_PRIO		10
//�����ջ��С	
#define P_REALTIME_STK_SIZE 		FreeRTOS_REALTIME_STK_SIZE  
//������
void P_REALTIME_task(void *pvParameters);

#endif

