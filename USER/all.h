#ifndef __ALL_H
#define __ALL_H 	

#include "all.h" 

#define FreeRTOS_GSM_STK_SIZE 300
#define FreeRTOS_Main_STK_SIZE 150
#define FreeRTOS_ASSIT_STK_SIZE 150
#define FreeRTOS_REALTIME_STK_SIZE 150

#define SUCCEED_BIT 0x00
#define FAULT_BIT 0x01

#define SMS_Function 0//���Ź��� 1���� 0�ر�
#define Dynamic_Upload_Function 0//��̬�ϴ�����

#define Debugpritn 1    //1:�������ڴ�ӡ���رտ��Ź�   0���رմ��ڴ�ӡ���������Ź�
#define DebugEnrollPrint 0

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "wdg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ASSIT.h"
#include "GSM.h"
#include "main_task.h"
#include "REALTIME.h" 
#include "PC.h"
#include "PC_ASSIT.h"

#include "crc16.h"
#include "recevice.h"
#include "sim800c.h"
#include "http.h"
#include "eeporm.h"
#include "init_flash.h"
#include "sms.h"
#include "http_api.h"
#include "MD5.h" 
#include "LCD.h"
#include "hj212.h"

#include "StackMacros.h"

#endif
