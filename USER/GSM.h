#ifndef __GSM_H
#define __GSM_H 	

#include "all.h" 
#include "GSM.h"

//任务优先级
#define GSM_TASK_PRIO		3
//任务堆栈大小	
#define GSM_STK_SIZE 		FreeRTOS_GSM_STK_SIZE  
//任务函数
void GSM_task(void *pvParameters);

//任务优先级
#define P_GSM_TASK_PRIO		8
//任务堆栈大小	
#define P_GSM_STK_SIZE 		FreeRTOS_GSM_STK_SIZE  
//任务函数
void P_GSM_task(void *pvParameters);

#define INIT_GSM 0x01//初始化GSM
#define CONNECT_IP    0x02 //连接TCPIP
#define GettimetoRTC  0x03 //获取时间写入SIM800 RTC
#define CorrectTime   0x04
#define UpdataTime    0x05
#define GetSingal     0x06
#define SEND_SEFT_SMS 0x81//发送短信查询余额
#define SEND_NUM_SMS 0x82//发送短信查询号码
#define SEND_ZJ_SMS_TO_ALL 0x83//向所有号码发送自检完成短信
#define SEND_FW_SMS_TO_ALL 0x84//向所有号码发送复位完成短信
#define SEND_CXBJ_SMS_TO_ALL 0x85//向所有号码发送超限报警短信
#define SEND_GZ_SMS_TO_ALL 0x86//向所有号码发送故障短信
#define POST_HTTP_SEFT 0x31//向服务器POST余额信息
#define POST_HTTP_BasicData 0x32//向服务器POST基本数据
#define POST_HTTP_AlarmData 0x33//向服务器POST报警数据
#define POST_HTTP_Frist 0x34//上传第一次的信息
#define POST_HTTP_FlagBIT 0x35//上传标志位
#define POST_HTTP_Num 0x36//上传号码
#define GET_HTTP_FLAGS  0x41//向服务器GET标志位
#define GET_HTTP_Config 0x42//向服务器GET配置信息

#define POST_HTTP_CDKEY 0xA1//上传序列号

//#define AlreadyWrite    (0x0000)
//#define IMEI_ADDRESS    (0x0003)
//#define S_ADDRESS       (0x0020)
//#define IP_ADDRESS      (0x0025)
//#define PORT_ADDRESS    (0x0050)
//#define ST_ADDRESS      (0x0060)
//#define PW_ADDRESS      (0x0070)
//#define MN_ADDRESS      (0x0080)
//#define DC_ADDRESS      (0x0110)
//#define PLOID_ADDRESS   (0x0120)
//#define WIRING_ADDRESS  (0x0130)
//#define CT_ADDRESS      (0x0135)
//#define PT_ADDRESS      (0x0145)
//#define isSAVE_ADDRESS  (0x0155)
//#define LNG_ADDRESS     (0x0175)
//#define LAT_ADDRESS     (0x0190)
//#define UID_ADDRESS     (0x0205)
//#define UGUID_ADDRESS   (0x0210)
//#define CMD_ADDRESS     (0x0260)

#define LossCnt 3

u8 AnalysisRrceiveDataNOAT(void);
u8 WritetoFlash(void);
u8 ReadFlashconfig(void);

#endif
