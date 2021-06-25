#ifndef __MAIN_TASK_H
#define __MAIN_TASK_H 	

#include "all.h" 
#include "main_task.h"

//任务优先级
#define MAIN_TASK_PRIO		4
//任务堆栈大小	
#define MAIN_STK_SIZE 		FreeRTOS_Main_STK_SIZE  
//任务函数
void MAIN_task(void *pvParameters);

#define R_BasicData_Task 0x01
#define R_Alarming_Value_Task 0x02
#define R_Hardware_Version_Number_Task 0x03
#define R_Software_Version_Number_Task 0x04
#define R_Action_Time_Task 0x05

#define W_Alarming_Value_Task 0x11
#define W_Action_Time_Task  0x12
#define W_RESET_Task  0x13
#define W_Self_Check_Task 0x14
#define W_Fire_Linkage_Task 0x15
#define W_Relay_Mode_Task 0x16
#define W_GPRS_Communication_Fault_Task 0x17

#endif

