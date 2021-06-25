#include "GSM.h"

extern u8 GPRS_FALUT_STATUS;
extern u8 GSMUART_buff[MAX_GSMUART];//GSM串口缓存数组
extern u16 GSMUART_cnt;//GSM串口接收字节的个数
extern u8 IMEIbuff[18];//IMEI
extern u8 TIMEbuff[25];
//用于存放服务器下发的数据
u8  ReceiveS;
u8 ReceiveIP[20];
u8 ReceivePORT[15];
u16 ReceiveST;
u8 ReceivePW[15];
u8 ReceiveMN[30];
u8 ReceiveDC[15];
u8 ReceivePLOID[15];
u8  ReceiveWIRING;
u8 ReceiveCT[15];
u8 ReceivePT[15];
u8 ReceiveisSAVE[15];
u8 ReceiveLNG[15];
u8 ReceiveLAT[15];
u8 ReceiveUID[15];
u8 ReceiveUGUID[50];
u8 ReceiveCMD[15];
//用于存放从flash读取的数据
u8  FlashS;
u8 FlashIMEI[25];
u8 FlashIP[20];
u8 FlashPORT[15];
u16 FlashST;
u8 FlashPW[15];
u8 FlashMN[30];
u8 FlashDC[15];
u8 FlashPLOID[15];
u8 FlashWIRING;
u8 FlashCT[15];
u8 FlashPT[15];
u8 FlashisSAVE[15];
u8 FlashLNG[15];
u8 FlashLAT[15];
u8 FlashUID[15];
u8 FlashUGUID[50];
u8 FlashCMD[15];

u8 GSM_Active=0;//GSM模块是否正在通信中 0:否 1:正在通信
u8 succeed_GSM_INIT=0;//初始化成功标志位
u8 TCPIP_INIT_status =0;
u8 ReceiveDATA=0;
//任务句柄
TaskHandle_t GSM_Task_Handler;

extern QueueHandle_t GSM_Message_Queue;	//信息队列句柄
extern QueueHandle_t ASSIT_Message_Queue;
extern u8 FlagBIT_BUF[16];
extern u8 IsSMS;
u8 sms_rex=0;
u8 updataing=0;
u16 testgsm=0;
char *IPaddress="47.106.255.185"; //测试IP、端口
char *Port="1899";

//GSM_task任务函数 
void GSM_task(void *pvParameters)
{
	   u8 buffer[1],i=0,Queue_BUFF=0,flash_sms_bit=0,j=0;
	   u8 cnt=0;
	   u16 timesec=0,TCPconnect=0;
	   u8 GSMsec=0,SendIMEI=0;
	   u8 Timestatus=0;
	   u8 SingalValue=0;
	   BaseType_t err;
	   Queue_BUFF=INIT_GSM;err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//上电初始化
       while(1)
       {
				TaskHandle_t TaskHandle;	
				TaskStatus_t TaskStatus;
				TaskHandle=xTaskGetHandle("GSM_task");//根据任务名获取任务句柄。
				vTaskGetInfo((TaskHandle_t	)TaskHandle,//任务句柄
				 (TaskStatus_t*	)&TaskStatus,//任务信息结构体
				 (BaseType_t	)pdTRUE,//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);//函数自己获取任务运行壮态
				testgsm=TaskStatus.usStackHighWaterMark;
				if(GSM_Message_Queue!=NULL)
				{
						for(i=0;i<1;i++)buffer[i]=0;//清除缓冲区
						err=xQueueReceive(GSM_Message_Queue,buffer,0);//请求消息UART_Message_Queue
						if(err==pdTRUE)//接收到消息
						{
								switch(buffer[0])
								{
									case INIT_GSM://初始化GSM
											GSM_Active=1;
											GSMConnect_INIT();
											GetIMEIofSIM800();
											ReadFlashconfig();
                                            #if  Debugpritn 
											UART_string_newline((u8 *)"************FlashConfig********",2);
											UART_string((u8 *)"FlashIMEI:",2);UART_string_newline(FlashIMEI,2);
											UART_string((u8 *)"FlashIP:",2);UART_string_newline(FlashIP,2);
											UART_string((u8 *)"FlashPORT:",2);UART_string_newline(FlashPORT,2);
											UART_string((u8 *)"FlashMN:",2);UART_string_newline(FlashMN,2);
											#endif										
											succeed_GSM_INIT=1;
											GSM_Active=0;
									case GettimetoRTC:
										    #if  Debugpritn 
											UART_string_newline((u8 *)"获取网络时间且上传IMEI\r\n",2);
											#endif
											REget:	
											TCPConnect(IPaddress,Port);
									        if(Find_StringforGSMUART_buff("CONNECT OK",15,0)|Find_StringforGSMUART_buff("ALREAY",15,0))
											{
												Timestatus=_sendIMEIandgetTime(IMEIbuff); 
												cnt++;								
												if(Timestatus==0&&cnt<=3)
												{
													#if  Debugpritn 
													UART_string_newline((u8 *)"获取网络时间失败或者写入SIM失败，进行重连\r\n",2);
													#endif	
													goto REget;
												}
												else if(Timestatus==0&&cnt>3)
												{
													cnt=0;
													#if  Debugpritn 
													UART_string_newline((u8 *)"超过重连次数，获取网络时间或者写入SIM失败\r\n",2);
													#endif	
													Queue_BUFF=INIT_GSM;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);												
												}
												else if(Timestatus)
												{
													cnt=0;	
													Queue_BUFF=CONNECT_IP;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
													#if  Debugpritn 
													UART_string_newline((u8 *)"获取网络时间成功\r\n",2);
													#endif										
												}
											}
											else
											{
												TCPconnect++;
												Queue_BUFF=GettimetoRTC;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
												if(TCPconnect==3)
												{
												    Soft_SystemReset();
												}
											}
                                            break;
									case CONNECT_IP:
                                            REstart:										
											TCPConnect((char *)FlashIP,(char *)FlashPORT);
											TCPIP_INIT_status=Find_StringforGSMUART_buff("CONNECT OK",15,0)|Find_StringforGSMUART_buff("ALREAY",15,0);
									        cnt++;								
										    if(TCPIP_INIT_status==0&&cnt<=5)
											{
												#if  Debugpritn 
												UART_string_newline((u8 *)"连接TCPIP失败，进行重连\r\n",2);
												#endif	
												goto REstart;
											}
											else if(TCPIP_INIT_status==0&&cnt>5)
											{
												cnt=0;
												#if  Debugpritn 
												UART_string_newline((u8 *)"超过重连次数，连接TCPIP失败\r\n",2);
												#endif	
												Soft_SystemReset();
											}
											else if(TCPIP_INIT_status)
											{
												if(_sendIMEItoIP(IMEIbuff))
												{
													Queue_BUFF=POST_HTTP_BasicData;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);				
												}
												else
												{
													SendIMEI++;
													if(SendIMEI<3)
													{
														Queue_BUFF=CONNECT_IP;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
													}													
													else Soft_SystemReset();
												}
												#if  Debugpritn 
												UART_string_newline((u8 *)"连接TCPIP成功\r\n",2);
												#endif	
												cnt=0;																					
											}
										    break;
									case POST_HTTP_BasicData://POST基本数据--主动上传数据
											#if  Debugpritn 
											UART_string_newline((u8 *)"POST基本数据\r\n",2);
											#endif	
											Resend:
											GSM_Active=1;
											UPDATA_FT();
											updataing=0;										
											GPRS_FALUT_STATUS=POST_HTTP_BasicData_MOTHOD();
									        cnt++;
									        if(GPRS_FALUT_STATUS==0&&cnt<3)
											{												
												#if  Debugpritn 
												UART_string_newline((u8 *)"POST基本数据失败，进行重发\r\n",2);
												#endif	
												goto Resend;
											}
											else if(GPRS_FALUT_STATUS==0&&cnt>=3)
											{
												#if  Debugpritn 
												UART_string_newline((u8 *)"超过重发次数，重启SIM\r\n",2);
												#endif	
												cnt=0;
												Queue_BUFF=INIT_GSM;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
											}
											else if(GPRS_FALUT_STATUS==2&&cnt>=3)
											{
												#if  Debugpritn 
												UART_string_newline((u8 *)"超过重发次数，重连TCPIP\r\n",2);
												#endif	
												cnt=0;
												Queue_BUFF=CONNECT_IP;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
											}
											else if(GPRS_FALUT_STATUS)
											{
												cnt=0;
											}
											GSM_Active=0;
											break;
									case CorrectTime:
										    _sendIMEIandgetTime(IMEIbuff);																		
										    break;
									case GetSingal:
										    SingalValue=Return_Signal();
											#if  Debugpritn 
											UART_string("信号强度：",2);
											UART_string_newline(U16IntToStr(SingalValue),2);
											#endif	
											break;
								   default:break;
							    }
						}
				  }
				  UART_string_newline((u8 *)"AT+CIPRXGET=2,1000",1);
				  if(Find_StringforGSMUART_buff("OK",10,0))
				  {
								AnalysisRrceiveDataNOAT();
								GSM_Find_upgrade();
				  }
				  clear_USART1buff();
				  GSMsec++;
				  if(GSMsec==6)
				  {
					  GSMsec=0;
					  Simtimechargetostr(TIMEbuff);	
				  }
				  delay_ms(100);
    }
}


//任务句柄
TaskHandle_t P_GSM_Task_Handler;
extern u8 CDKEY_MOTHOD_bit;
extern QueueHandle_t P_GSM_Message_Queue;//信息队列句柄
extern QueueHandle_t PCASSIT_Message_Queue;//信息队列句柄
extern u8 sc_http_need;
extern u16 register_flag;
extern u32 HTTP_PCB_ID;
extern u8 LCD_CHANGE_NEED;
extern u8 EncryptMode;
u8 GETDATA_Succeed=0;
u8 LCD1602_IDBUF[16]={0};//ID缓存数组
u8 LCD1602_EncryptModeBuf[8];
//P_GSM_task函数 
void P_GSM_task(void *pvParameters)
{
	   u8 buffer[1],i=0,Queue_BUFF=0,flash_sms_bit=0,j=0;
	   u8 cnt=0,GSMsec=0;
	   u16 timesec=0,TCPconnect=0;
	   u8 Timestatus=0;
	   u8 SingalValue=0;
	   BaseType_t err;
	   Queue_BUFF=INIT_GSM;err=xQueueSend(P_GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//上电初始化
       while(1)
       {
				TaskHandle_t TaskHandle;	
				TaskStatus_t TaskStatus;
				TaskHandle=xTaskGetHandle("P_GSM_task");//根据任务名获取任务句柄。
				vTaskGetInfo((TaskHandle_t	)TaskHandle,//任务句柄
				 (TaskStatus_t*	)&TaskStatus,//任务信息结构体
				 (BaseType_t	)pdTRUE,//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);//函数自己获取任务运行壮态
				testgsm=TaskStatus.usStackHighWaterMark;
				if(P_GSM_Message_Queue!=NULL)
				{
						for(i=0;i<1;i++)buffer[i]=0;//清除缓冲区
						err=xQueueReceive(P_GSM_Message_Queue,buffer,0);//请求消息UART_Message_Queue
						if(err==pdTRUE)//接收到消息
						{
								switch(buffer[0])
								{
									case INIT_GSM://初始化GSM
											GSM_Active=1;
											GSMConnect_INIT();
											GetIMEIofSIM800();
											ReadFlashconfig();
                                            #if  DebugEnrollPrint 
											UART_string_newline((u8 *)"************FlashConfig********",2);
											UART_string((u8 *)"FlashIMEI:",2);UART_string_newline(FlashIMEI,2);
											UART_string((u8 *)"FlashIP:",2);UART_string_newline(FlashIP,2);
											UART_string((u8 *)"FlashPORT:",2);UART_string_newline(FlashPORT,2);
											UART_string((u8 *)"FlashMN:",2);UART_string_newline(FlashMN,2);
											#endif										
											succeed_GSM_INIT=1;
											GSM_Active=0;
									case GettimetoRTC:
										    #if  DebugEnrollPrint 
											UART_string_newline((u8 *)"获取网络时间且上传IMEI\r\n",2);
											#endif
											REget:	
											TCPConnect((char *)FlashIP,(char *)FlashPORT);
									    if(Find_StringforGSMUART_buff("CONNECT OK",15,0)|Find_StringforGSMUART_buff("ALREAY",15,0))
											{
												register_flag=IPOK;
												Timestatus=_PsendIMEIandgetTime(IMEIbuff); 
												cnt++;								
												if(Timestatus==0&&cnt<=3)
												{
													#if  DebugEnrollPrint 
													UART_string_newline((u8 *)"获取网络时间失败或者写入SIM失败，进行重连\r\n",2);
													#endif	
													goto REget;
												}
												else if(Timestatus==0&&cnt>3)
												{
													cnt=0;
													GETDATA_Succeed	=2;	
													#if  DebugEnrollPrint 
													UART_string_newline((u8 *)"超过重连次数，获取网络时间或者写入SIM失败\r\n",2);
													#endif	
													Queue_BUFF=INIT_GSM;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);												
												}
												else if(Timestatus)
												{
													cnt=0;		
													GETDATA_Succeed	=1;												
													#if  DebugEnrollPrint 
													UART_string_newline((u8 *)"获取网络时间成功，IMEI测试成功\r\n",2);
													#endif										
												}
											}
											else
											{
												TCPconnect++;
												#if  DebugEnrollPrint 
												UART_string_newline((u8 *)"连接TCPIP失败，再次连接TCPIP\r\n",2);
												#endif													
												Queue_BUFF=GettimetoRTC;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
												if(TCPconnect==3)
												{
													register_flag=FAILIP;
													delay_ms(1000);
												    Soft_SystemReset();
												}
											}											
                      break;								
									case GetSingal:
										    SingalValue=Return_Signal();
											#if  DebugEnrollPrint 
											UART_string((u8 *)"信号强度：",2);
											UART_string_newline(U16IntToStr(SingalValue),2);
											#endif	
											break;
								   default:break;
							    }
						}
				  }
				  clear_USART1buff();
		 }
		 delay_ms(50);
}

u8 AnalysisRrceiveDataNOAT(void)
{
  u16 pos1=0;
  u16 pos2=0;
  u16 cnt=0;
  u8 receviebuff[300]={0};
  for(cnt=0;cnt<300;cnt++) receviebuff[cnt]=0;
  for(cnt=0;cnt<20;cnt++) ReceiveIP[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceivePORT[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceivePW[cnt]=0;
  for(cnt=0;cnt<30;cnt++) ReceiveMN[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveDC[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceivePLOID[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveCT[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceivePT[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveisSAVE[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveLNG[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveLAT[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveUID[cnt]=0;
  for(cnt=0;cnt<50;cnt++) ReceiveUGUID[cnt]=0;
  for(cnt=0;cnt<15;cnt++) ReceiveCMD[cnt]=0;
  if(strsearch(GSMUART_buff,(u8 *)"IMEI")&&strsearch(GSMUART_buff,(u8 *)"cmd"))
  {
	pos1=strsearch(GSMUART_buff,(u8 *)"\"IMEI\""); 
	pos2=strsearch(GSMUART_buff,(u8 *)"\"cmd\"");
	CoypArraryAtoArraryB_LR(GSMUART_buff,receviebuff,pos2+10-pos1,pos1-1);
	pos1=strsearch(receviebuff,(u8 *)"\"s\":");
    ReceiveS = receviebuff[pos1+3]-'0';
	pos1=strsearch(receviebuff,(u8 *)"\"ip\"");
	pos2=strsearch(receviebuff,(u8 *)"\"port\"");
	for(cnt=0;cnt<20;cnt++) ReceiveIP[cnt]=0;
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveIP,pos2-pos1-8,pos1+5);  
	pos1=strsearch(receviebuff,(u8 *)"\"port\"");
	pos2=strsearch(receviebuff,(u8 *)"\"st\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceivePORT,pos2-pos1-8,pos1+6);	
	pos1=strsearch(receviebuff,(u8 *)"\"st\"");
	ReceiveST = 10*(receviebuff[pos1+5]-'0')+receviebuff[pos1+6]-'0';
	pos1=strsearch(receviebuff,(u8 *)"\"pw\"");
	pos2=strsearch(receviebuff,(u8 *)"\"mn\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceivePW,pos2-pos1-8,pos1+5); 
	pos1=strsearch(receviebuff,(u8 *)"\"mn\"");  
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveMN,24,pos1+5);
 	pos1=strsearch(receviebuff,(u8 *)"\"dc\"");  
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveDC,6,pos1+5);
	pos1=strsearch(receviebuff,(u8 *)"\"ploid\"");
	pos2=strsearch(receviebuff,(u8 *)"\"wiring\"");	  
	CoypArraryAtoArraryB_LR(receviebuff,ReceivePLOID,pos2-pos1-11,pos1+8); 
	pos1=strsearch(receviebuff,(u8 *)"\"wiring\"");  
	ReceiveWIRING = receviebuff[pos1+8]-'0';	
	pos1=strsearch(receviebuff,(u8 *)"\"ct\"");
	pos2=strsearch(receviebuff,(u8 *)"\"pt\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveCT,pos2-pos1-6,pos1+4); 
	pos1=strsearch(receviebuff,(u8 *)"\"pt\"");
	pos2=strsearch(receviebuff,(u8 *)"\"isSave\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceivePT,pos2-pos1-6,pos1+4);
	pos1=strsearch(receviebuff,(u8 *)"\"isSave\"");
	pos2=strsearch(receviebuff,(u8 *)"\"lng\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveisSAVE,pos2-pos1-10,pos1+8); 
	pos1=strsearch(receviebuff,(u8 *)"\"lng\"");
	pos2=strsearch(receviebuff,(u8 *)"\"lat\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveLNG,pos2-pos1-7,pos1+5);
	pos1=strsearch(receviebuff,(u8 *)"\"lat\"");
	pos2=strsearch(receviebuff,(u8 *)"\"uid\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveLAT,pos2-pos1-7,pos1+5);
	pos1=strsearch(receviebuff,(u8 *)"\"uid\"");
	pos2=strsearch(receviebuff,(u8 *)"\"uguid\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveUID,pos2-pos1-7,pos1+5);
	pos1=strsearch(receviebuff,(u8 *)"\"uguid\"");
	pos2=strsearch(receviebuff,(u8 *)"\"cmd\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveUGUID,pos2-pos1-11,pos1+8);
	pos1=strsearch(receviebuff,(u8 *)"\"cmd\"");
	CoypArraryAtoArraryB_LR(receviebuff,ReceiveCMD,5,pos1+5); 
	ReceiveDATA=1;	
	#if  Debugpritn 
	UART_string_newline((u8 *)"*****************RECEIVE DATA*****************\r\n",2);  
	UART_string_newline(receviebuff,2);
	UART_string((u8 *)"S:",2);
	UART_string_newline(U16IntToStr(ReceiveS),2);
	UART_string((u8 *)"IP:",2);
	UART_string_newline(ReceiveIP,2);
	UART_string((u8 *)"PORT:",2);
	UART_string_newline(ReceivePORT,2);
	UART_string((u8 *)"ST:",2);
	UART_string_newline(U16IntToStr(ReceiveST),2);
	UART_string((u8 *)"PW:",2);
	UART_string_newline(ReceivePW,2);
	UART_string((u8 *)"MN:",2);
	UART_string_newline(ReceiveMN,2);
	UART_string((u8 *)"DC:",2);
	UART_string_newline(ReceiveDC,2);
	UART_string((u8 *)"PLOID:",2);
	UART_string_newline(ReceivePLOID,2);
	UART_string((u8 *)"WIRING:",2);
	UART_string_newline(U16IntToStr(ReceiveWIRING),2);
	UART_string((u8 *)"CT:",2);
	UART_string_newline(ReceiveCT,2);
	UART_string((u8 *)"PT:",2);
	UART_string_newline(ReceivePT,2);
	UART_string((u8 *)"isSAVE:",2);
	UART_string_newline(ReceiveisSAVE,2);
	UART_string((u8 *)"LNG:",2);
	UART_string_newline(ReceiveLNG,2);
	UART_string((u8 *)"LAT:",2);
	UART_string_newline(ReceiveLAT,2);
	UART_string((u8 *)"UID:",2);
	UART_string_newline(ReceiveUID,2);
	UART_string((u8 *)"UGUID:",2);
	UART_string_newline(ReceiveUGUID,2);
	UART_string((u8 *)"CMD:",2);
	UART_string_newline(ReceiveCMD,2);
	#endif
	if(ReceiveDATA)  //数据解析成功
	{
	  ReceiveDATA=0;
	  if(WritetoFlash())
	  {
		  #if  Debugpritn 
		  UART_string_newline((u8 *)"<<<<<<解析数据成功写入，进行重启>>>>>>\r\n",2);
		  #endif	
		  Soft_SystemReset();
	  }
	}
	return 1;
  }	
  else 	return 2;
 
}

u8 WritetoFlash(void)
{
	u16 ReadCnt=0;
	u16 Writelen=0;
	u8 Readtempbuff=0;
	u8 Tempbuff[50]={0};
	u16 Tempvalue=0,Bufflen=0;
	FLASH_Unlock();
	//IMEI
	Writelen=strlen((char *)IMEIbuff);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(IMEI_ADDRESS+ReadCnt,IMEIbuff[ReadCnt]);
	MyEE_WriteWord(IMEI_ADDRESS-1,Writelen);
	//S
	MyEE_WriteWord(S_ADDRESS,ReceiveS);
	//IP
	Writelen=strlen((char *)ReceiveIP);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(IP_ADDRESS+ReadCnt,ReceiveIP[ReadCnt]);
	MyEE_WriteWord(IP_ADDRESS-1,Writelen);
	//PORT
	Writelen=strlen((char *)ReceivePORT);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(PORT_ADDRESS+ReadCnt,ReceivePORT[ReadCnt]);
	MyEE_WriteWord(PORT_ADDRESS-1,Writelen);
	//ST
	MyEE_WriteWord(ST_ADDRESS,ReceiveST);
	//PW
	Writelen=strlen((char *)ReceivePW);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(PW_ADDRESS+ReadCnt,ReceivePW[ReadCnt]);
	MyEE_WriteWord(PW_ADDRESS-1,Writelen);
	//MN
	Writelen=strlen((char *)ReceiveMN);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(MN_ADDRESS+ReadCnt,ReceiveMN[ReadCnt]);
	MyEE_WriteWord(MN_ADDRESS-1,Writelen);
	//DC
	Writelen=strlen((char *)ReceiveDC);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(DC_ADDRESS+ReadCnt,ReceiveDC[ReadCnt]);
	MyEE_WriteWord(DC_ADDRESS-1,Writelen);
	//PLOID
	Writelen=strlen((char *)ReceivePLOID);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(PLOID_ADDRESS+ReadCnt,ReceivePLOID[ReadCnt]);
	MyEE_WriteWord(PLOID_ADDRESS-1,Writelen);
	//WIRING
	MyEE_WriteWord(WIRING_ADDRESS,ReceiveWIRING);
	//CT
	Writelen=strlen((char *)ReceiveCT);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(CT_ADDRESS+ReadCnt,ReceiveCT[ReadCnt]);
	MyEE_WriteWord(CT_ADDRESS-1,Writelen);
	//PT
	Writelen=strlen((char *)ReceivePT);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(PT_ADDRESS+ReadCnt,ReceivePT[ReadCnt]);
	MyEE_WriteWord(PT_ADDRESS-1,Writelen);
	//isSAVE
	Writelen=strlen((char *)ReceiveisSAVE);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(isSAVE_ADDRESS+ReadCnt,ReceiveisSAVE[ReadCnt]);
	MyEE_WriteWord(isSAVE_ADDRESS-1,Writelen);
	//LNG
	Writelen=strlen((char *)ReceiveLNG);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(LNG_ADDRESS+ReadCnt,ReceiveLNG[ReadCnt]);
	MyEE_WriteWord(LNG_ADDRESS-1,Writelen);
	//LAT
	Writelen=strlen((char *)ReceiveLAT);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(LAT_ADDRESS+ReadCnt,ReceiveLAT[ReadCnt]);
	MyEE_WriteWord(LAT_ADDRESS-1,Writelen);
	//UID
	Writelen=strlen((char *)ReceiveUID);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(UID_ADDRESS+ReadCnt,ReceiveUID[ReadCnt]);
	MyEE_WriteWord(UID_ADDRESS-1,Writelen);
	//UGUID
	Writelen=strlen((char *)ReceiveUGUID);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(UGUID_ADDRESS+ReadCnt,ReceiveUGUID[ReadCnt]);
	MyEE_WriteWord(UGUID_ADDRESS-1,Writelen);
	//CMD
	Writelen=strlen((char *)ReceiveCMD);
	for(ReadCnt=0;ReadCnt<Writelen;ReadCnt++)
	MyEE_WriteWord(CMD_ADDRESS+ReadCnt,ReceiveCMD[ReadCnt]);
	MyEE_WriteWord(CMD_ADDRESS-1,Writelen);	
	MyEE_WriteWord(AlreadyWrite,111);
	FLASH_Lock();
	FLASH_Unlock();
	   #if  Debugpritn 
	   //S
       Tempvalue=MyEE_ReadWord(S_ADDRESS);
	   UART_string_newline(U16IntToStr(Tempvalue),2);
	   //IMEI
	   Bufflen=MyEE_ReadWord(IMEI_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(IMEI_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //IP
	   Bufflen=MyEE_ReadWord(IP_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(IP_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //PORT
	   Bufflen=MyEE_ReadWord(PORT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(PORT_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //ST
	   Tempvalue=MyEE_ReadWord(ST_ADDRESS);
	   UART_string_newline(U16IntToStr(Tempvalue),2);
	   //PW
	   Bufflen=MyEE_ReadWord(PW_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(PW_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //MN
	   Bufflen=MyEE_ReadWord(MN_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(MN_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //DC
	   Bufflen=MyEE_ReadWord(DC_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(DC_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //PLOID
	   Bufflen=MyEE_ReadWord(PLOID_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(PLOID_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //WIRING
	   Tempvalue=MyEE_ReadWord(WIRING_ADDRESS);
	   UART_string_newline(U16IntToStr(Tempvalue),2);
	   //CT
	   Bufflen=MyEE_ReadWord(CT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(CT_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //PT
	   Bufflen=MyEE_ReadWord(PT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(PT_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //isSAVE
	   Bufflen=MyEE_ReadWord(isSAVE_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(isSAVE_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //LNG
	   Bufflen=MyEE_ReadWord(LNG_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(LNG_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //LAT
	   Bufflen=MyEE_ReadWord(LAT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(LAT_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //UID
	   Bufflen=MyEE_ReadWord(UID_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(UID_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //UGUID
	   Bufflen=MyEE_ReadWord(UGUID_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(UGUID_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   //CMD
	   Bufflen=MyEE_ReadWord(CMD_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   Tempbuff[ReadCnt]=MyEE_ReadWord(CMD_ADDRESS+ReadCnt);
	   UART_string_newline(Tempbuff,2);
	   for(ReadCnt=0;ReadCnt<50;ReadCnt++) Tempbuff[ReadCnt]=0;
	   #endif	
	Readtempbuff=MyEE_ReadWord(AlreadyWrite);
	#if  Debugpritn 
	UART_string_newline(U16IntToStr(Readtempbuff),2);
	#endif
	FLASH_Lock();
	if(Readtempbuff==111)
	{
	#if  Debugpritn 
	UART_string_newline((u8 *)"Finished Write Flash!",2);
	#endif	
	return 1;
	}
	else return 0;
}

u8 ReadFlashconfig(void)
{
	u16 AlreadyHavedData=0;
	u16 ReadCnt=0;
	u16 Bufflen=0;
	FLASH_Unlock();
	AlreadyHavedData=MyEE_ReadWord(AlreadyWrite);
	FLASH_Lock();	
	if(AlreadyHavedData==111)  //之前已经写过数据进去
	{
	   FLASH_Unlock();
	   //MyEE_WriteWord(AlreadyWrite,0);  //将标志位重新置零
	   //S
       FlashS=MyEE_ReadWord(S_ADDRESS);
	   //IMEI
	   Bufflen=MyEE_ReadWord(IMEI_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashIMEI[ReadCnt]=MyEE_ReadWord(IMEI_ADDRESS+ReadCnt);
	   //IP
	   Bufflen=MyEE_ReadWord(IP_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashIP[ReadCnt]=MyEE_ReadWord(IP_ADDRESS+ReadCnt);
	   //PORT
	   Bufflen=MyEE_ReadWord(PORT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashPORT[ReadCnt]=MyEE_ReadWord(PORT_ADDRESS+ReadCnt);
	   //ST
	   FlashST=MyEE_ReadWord(ST_ADDRESS);
	   //PW
	   Bufflen=MyEE_ReadWord(PW_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashPW[ReadCnt]=MyEE_ReadWord(PW_ADDRESS+ReadCnt);
	   //MN
	   Bufflen=MyEE_ReadWord(MN_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashMN[ReadCnt]=MyEE_ReadWord(MN_ADDRESS+ReadCnt);
	   //DC
	   Bufflen=MyEE_ReadWord(DC_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashDC[ReadCnt]=MyEE_ReadWord(DC_ADDRESS+ReadCnt);
	   //PLOID
	   Bufflen=MyEE_ReadWord(PLOID_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashPLOID[ReadCnt]=MyEE_ReadWord(PLOID_ADDRESS+ReadCnt);
	   //WIRING
	   FlashWIRING=MyEE_ReadWord(WIRING_ADDRESS);
	   //CT
	   Bufflen=MyEE_ReadWord(CT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashCT[ReadCnt]=MyEE_ReadWord(CT_ADDRESS+ReadCnt);
	   //PT
	   Bufflen=MyEE_ReadWord(PT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashPT[ReadCnt]=MyEE_ReadWord(PT_ADDRESS+ReadCnt);
	   //isSAVE
	   Bufflen=MyEE_ReadWord(isSAVE_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashisSAVE[ReadCnt]=MyEE_ReadWord(isSAVE_ADDRESS+ReadCnt);
	   //LNG
	   Bufflen=MyEE_ReadWord(LNG_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashLNG[ReadCnt]=MyEE_ReadWord(LNG_ADDRESS+ReadCnt);
	   //LAT
	   Bufflen=MyEE_ReadWord(LAT_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashLAT[ReadCnt]=MyEE_ReadWord(LAT_ADDRESS+ReadCnt);
	   //UID
	   Bufflen=MyEE_ReadWord(UID_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashUID[ReadCnt]=MyEE_ReadWord(UID_ADDRESS+ReadCnt);
	   //UGUID
	   Bufflen=MyEE_ReadWord(UGUID_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashUGUID[ReadCnt]=MyEE_ReadWord(UGUID_ADDRESS+ReadCnt);
	   //CMD
	   Bufflen=MyEE_ReadWord(CMD_ADDRESS-1);
	   for(ReadCnt=0;ReadCnt<Bufflen;ReadCnt++)
	   FlashCMD[ReadCnt]=MyEE_ReadWord(CMD_ADDRESS+ReadCnt);
	   FLASH_Lock();
	
		FLASH_Unlock();
		Bufflen=MyEE_ReadWord(AlreadyWrite);
		FLASH_Lock();
		
		if(Bufflen==111)
		{
			#if  Debugpritn 
			UART_string_newline((u8 *)"********Finished READ Flash!********",2);		
			#endif	
		    return 1;
		}	
	}
	else
	{
	  #if  Debugpritn 
		UART_string_newline((u8 *)"********Failed READ Flash!********",2);		
	  #endif	
	  strcat((char *)FlashIP,"47.106.255.185");
	  strcat((char *)FlashPORT,"1899");
	  strcat((char *)FlashMN,(char *)IMEIbuff);
	  strcat((char *)FlashIMEI,(char *)IMEIbuff);
	  strcat((char *)FlashPW,"123456");
	  FlashST=80;
	  strcat((char *)FlashDC,"P10001");
      return 0;
	}
	
}

