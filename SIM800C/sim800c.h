#ifndef __SIM800C_H__
#define __SIM800C_H__ 	

#include "all.h" 
#include "sim800c.h"

#define MAX_GSMUART 2048//最大串口接受数组长度

//GSM的状态
#define GSM_OFFLINE 0//gsm模块断开了网络
#define GSM_FINDING 1//搜索中
#define GSM_ONLINE  2//gsm模块已经注册网络，处于正常待机状态

//SIM卡运营商
#define NoOperator        0   //没有检测到
#define ChinaMobile       1   //中国移动
#define ChinaUnicom       2   //中国联通

typedef struct _NETWROKTIME
{
	u16 Year;
	u16 Month;
	u16 Day;
	u16 Hour;
	u16 Minute;
    u16 Second;	
}NETWROKTIME;


void clear_USART1buff(void);
void GSMConnect_INIT(void);
u8 check_ATcommand(void);
void GSM_BasicData(void);
u8 set_GSMconfiguration(void);
u8 checkSIMsertStatus(void);
u8 DeleteAllSMS(void);
u8 gsm_gprs_init(void);
u8 GetSimAttachGprs(void);
u8 WriteApnName(u8 *name);
u8 GetCurrGprsStatus(void);
u8 IsGetDnsIp(void);
u8 WriteApnName22(u8 *name);
u8 StartApnGprs(void);
void Check_CDKEY(void);
void TCPConnect(char* IP_Add,char* Port_Add);
u8 Find_StringforGSMUART_buff(char* STRING_BUFFER,u16 cnt,u8 mode);
u16 strsearchforGSMUART_buff(const u8 *p2);
void UART1_BUF_SEND(char * _data,u16 lenth);
void UART2_BUF_SEND(char * _data,u16 lenth);
u8 CIPRXGET_Command(void);
u8 GetIMEIofSIM800(void);
u8 GetNetworkTimeFromSim(void);
u8 _sendIMEIandgetTime(u8 *IMEI);
u8 _PsendIMEIandgetTime(u8 *IMEI);
u8 TimechargetoWrite(char *timenew,char *timeold);
u8 Simtimechargetostr(u8 *strtime);
u8 Return_Signal(void);
u8 _sendIMEItoIP(u8 *IMEI);

void GSM_Find_upgrade(void);
void analysis_upgrade(void);
u16 HEXsearchforGSMUART_buff(const u8 *p2,const u16 p2_len);
u16 EndstrsearchGSMUART_buff(const u8 *p2);
u16 GSMatoi(char *s);

#endif
