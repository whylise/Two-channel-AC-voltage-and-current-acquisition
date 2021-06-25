#include "http_api.h"
#include "hj212.h"
#include "all.h" 

#define HTTP200CNT 150//等待服务器响应时间的次数，总时间=HTTP200CNT*HTTP200DELAY
#define HTTP200DELAY 200//每次等待服务器的延时时间，单位ms

extern CURR_RTCM FT_rtcm;//下板数据结构体
extern CURR_RTCM NEW_DATA_FT;//发送的数据，发送过程中不能更新数据值
extern STATUS_RTCM FT_status;//下板状态结构体
extern STATUS_RTCM NEW_status_FT;//发送的状态，发送过程中不能更新数据值
extern u16 t_sec;
extern u8 IMEIbuff[18];//IMEI
extern u8 TIMEbuff[25];
extern u8 GPRS_FALUT_STATUS;
extern u8 GSMUART_buff[MAX_GSMUART];
extern QueueHandle_t GSM_Message_Queue;	//信息队列句柄
extern QueueHandle_t P_GSM_Message_Queue;//信息队列句柄
extern QueueHandle_t ASSIT_Message_Queue;//信息队列句柄
extern QueueHandle_t FT_Message_Queue;//信息队列句柄
extern CURR_RTCM FT_rtcm;
extern u8 NumBer[11];
extern u16 SysGsmRssi;
extern char MD5_BUF[33];
extern u8 LCD_CHANGE_NEED;

//用于存放从flash读取的数据
extern u8  FlashS;
extern u8 FlashIMEI[25];
extern u8 FlashIP[20];
extern u8 FlashPORT[15];
extern u16 FlashST;
extern u8 FlashPW[15];
extern u8 FlashMN[30];
extern u8 FlashDC[15];
extern u8 FlashPLOID[15];
extern u8 FlashWIRING;
extern u8 FlashCT[15];
extern u8 FlashPT[15];
extern u8 FlashisSAVE[15];
extern u8 FlashLNG[15];
extern u8 FlashLAT[15];
extern u8 FlashUID[15];
extern u8 FlashUGUID[50];
extern u8 FlashCMD[15];

u32 HTTP_PCB_ID=0;//从服务获取的注册ID
const u8 login_password1[]="antenicantenic888";//登录账户密码1
const u8 login2[]="leakage";//登录账户2
const u8 password2[]="33rgthsy";//登录密码2
const u8 login3[]="current";//登录账户3
const u8 password3[]="ffjszvjs";//登录密码3
const u8 login4[]="temperature";//登录账户4
const u8 password4[]="yw5tacwc";//登录密码4
const u8 login5[]="voltage";//登录账户5
const u8 password5[]="wg54rvrg";//登录密码5
const u8 login6[]="alarm";//登录账户6
const u8 password6[]="gdx4435a";//登录密码6
const u8 login7[]="malfunction";//登录账户7
const u8 password7[]="a3r3aw32";//登录密码7
const u8 login8[]="server";//登录账户8
const u8 password8[]="qra32r22";//登录密码8
const u8 login9[]="admin";//登录账户9
const u8 password9[]="zgre4234";//登录密码9

u8 FlagBIT_BUF[16]={0};//POST_HTTP标志位缓存
u8 CDKEY_MOTHOD_bit=99;//POSTCDKEY后的状态
u8 cdkeybuffer[24]={0};//CDKEY缓存
u8 cdkey_length=0;//CDKEY长度
u8 id_buffer[16]={0};//ID缓存
u8 get_http_flagbit[16]={0};//GET_HTTP标志位缓存
u8 IsSMS=1;//默认需要短信服务
u8 PushMode=0;//默认上传时间模式模式，固定上传
u8 PushRate=0;//变化幅值
u8 EncryptMode=0;
//***********************************************************************
//描述:清除上传的标志位缓存
void clear_FlagBIT(void)
{
		u8 flag_i=0;
		for(flag_i=0;flag_i<16;flag_i++)FlagBIT_BUF[flag_i]='0';
}

//***********************************************************************
//描述:HTTP每次POST或GET的初始化
void HTTP_INIT_MOTHOD(void)
{
		u16 URL_LEN=0,posti=0;
		u8 	URL_BUF[50]={0};
		URL_LEN=MyEE_ReadWord(HTTP_URL_LenAdd);
		for(posti=0;posti<URL_LEN;posti++)URL_BUF[posti]=MyEE_ReadWord(HTTP_URL_Add+posti);//读取FLASH的正常工作域名
		clear_USART1buff();
		UART_string_newline("AT+HTTPINIT",1);//初始化HTTP
		delay_ms(100); 
		clear_USART1buff();
		UART_string_newline("AT+HTTPPARA=\"CID\",1",1);//设置HTTP会话参数
		delay_ms(100); 	
		clear_USART1buff();
		UART_string_newline("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"",1);//配置http中的post的"CONTENT"
		delay_ms(100);
		clear_USART1buff();
		UART_string("AT+HTTPPARA=\"URL\",\"",1);//设置URL
		UART_string(URL_BUF,1);
}

void CDHostUrlMode(void)
{
		u8 HostURL_Len=0,posti=0;
		u8 HostURL[50]={0},ModeBuf[8]={0};//生产注册时获取的URL缓存
		u16 posid1=0,posid2=0;
		posid1=strsearch(GSMUART_buff,"SimID\":");//获取关键字位置
		posid2=strsearch(GSMUART_buff,",\"SimCode\"");//获取关键字位置
		CoypArraryAtoArraryB_LR(GSMUART_buff,id_buffer,posid2-posid1-7,posid1+7-1);//复制要获取的部分数组
		HTTP_PCB_ID=stingcnt_to_uint32(id_buffer);//数组转十进制
		posid1=0;
		posid2=0;
		posid1=strsearch(GSMUART_buff,"HostUrl\":\"");//获取关键字位置
		posid2=strsearch(GSMUART_buff,"\"}");//获取关键字位置
		CoypArraryAtoArraryB_LR(GSMUART_buff,HostURL,posid2-posid1-10,posid1+10-1);//复制要获取的部分数组
		posid1=0;
		posid2=0;
		posid1=strsearch(GSMUART_buff,"EncryptMode\":");//获取关键字位置
		posid2=strsearch(GSMUART_buff,",\"HostUrl\"");//获取关键字位置
		CoypArraryAtoArraryB_LR(GSMUART_buff,ModeBuf,posid2-posid1-13,posid1+13-1);//复制要获取的部分数组
		EncryptMode=stingcnt_to_uint32(ModeBuf);//数组转十进制
		FLASH_Unlock();
		//写服务器
		HostURL_Len=strlen((char *)HostURL);
		MyEE_WriteWord(HTTP_URL_LenAdd,HostURL_Len);//有效服务器长度
		for(posti=0;posti<HostURL_Len;posti++)
				MyEE_WriteWord(HTTP_URL_Add+posti,HostURL[posti]);
		MyEE_WriteWord(IDAddH,HTTP_PCB_ID>>16);
		MyEE_WriteWord(IDAddL,HTTP_PCB_ID);
		MyEE_WriteWord(EncryptMode_Add,EncryptMode);
		FLASH_Lock();
}

//***********************
//描述:POSTCDKEY
//返回:0:成功 1:失败
u8 POST_HTTP_CDKEY_MOTHOD(void)
{
		u8 sign_buf[64]={0},URL_BUF[50]={0};
		u16 length_buffer=0,poscd1=0,poscd2=0;
		u8 Queue_BUFF=0;
		u16 URL_LEN=0,posti=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				clear_USART1buff();
				UART_string_newline("AT+CGSN",1);//查询序列号
				delay_ms(50);
				if(GSMUART_buff[0]==0x0d&&GSMUART_buff[1]==0x0a)poscd1=3;
				poscd2=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0aOK\x0d\x0a");
				if(poscd1!=0&&poscd2!=0)
				{	
						CoypArraryAtoArraryB_LR(GSMUART_buff,cdkeybuffer,poscd2-poscd1,poscd1-1);//将序列号复制到cdkeybuffer
						cdkey_length=poscd2-poscd1;
				}
				strcat((char*)sign_buf,(char*)login_password1);//数组连接
				strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));//数组连接
				MD5(sign_buf);//计算MD5
				URL_LEN=MyEE_ReadWord(HTTP_SCURL_LenAdd);//读取FLASH的生产工作域名的长度
				for(posti=0;posti<URL_LEN;posti++)URL_BUF[posti]=MyEE_ReadWord(HTTP_SCURL_Add+posti);//读取FLASH的生产工作域名
				clear_USART1buff();
				UART_string_newline("AT+HTTPINIT",1);//初始化HTTP
				delay_ms(100); 
				clear_USART1buff();
				UART_string_newline("AT+HTTPPARA=\"CID\",1",1);//设置HTTP会话参数
				delay_ms(100); 	
				clear_USART1buff();
				UART_string_newline("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"",1);//配置http中的post的"CONTENT"
				delay_ms(100);
				clear_USART1buff();
				UART_string("AT+HTTPPARA=\"URL\",\"",1);//设置URL
				UART_string(URL_BUF,1);
				UART_string_newline("/json/RegSim\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();
				length_buffer=strlen((char*)MD5_BUF)+strlen((char*)U16IntToStr(SysGsmRssi));//计算上传数据的长度
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer+poscd2-poscd1+20),1);
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{	
						UART_string("SimCode=",1);//8
						UART_string(cdkeybuffer,1);
						UART_string("&sign=",1);//6
						UART_string((unsigned char*)MD5_BUF,1);
						UART_string("&once=",1);//6
						UART_string(U16IntToStr(SysGsmRssi),1);
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)//等待服务器响应200成功
						{
								delay_ms(HTTP200DELAY);
								if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))//判断响应是否是200
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,200",1);//读服务器内容前200个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"\"ResultCode\":1"))//首次注册
								{
										CDHostUrlMode();
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										CDKEY_MOTHOD_bit=1;
										return 0;//成功获取
								}
								else if(strsearch(GSMUART_buff,"\"ResultCode\":2"))//已经注册
								{
										CDHostUrlMode();
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										CDKEY_MOTHOD_bit=2;
										return 0;//成功获取
								}
								else if(strsearch(GSMUART_buff,"\"ResultCode\":3"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										CDKEY_MOTHOD_bit=3;
										return 0;//成功获取
								}
								else if(strsearch(GSMUART_buff,"\"ResultCode\":4"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										CDKEY_MOTHOD_bit=4;
										return 1;//成功获取
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								CDKEY_MOTHOD_bit=0;
								return 1;
						}
				}
				UART_string_newline("AT+HTTPTERM",1);
				delay_ms(100);
		}
		else
		{Queue_BUFF=ASSIT_SystemReset;err=xQueueSend(ASSIT_Message_Queue,&Queue_BUFF,portMAX_DELAY);}//上电初始化
		CDKEY_MOTHOD_bit=0;
		return 2;//错误
}
//***********************
//描述:POST余额
//返回:0:成功 1:失败
u8 POST_HTTP_SEFT_MOTHOD(void)
{
		u8 sign_buf[64]={0},id_buf[10]={0};
		u32 ID_buffer=0;
	  u16 length_buffer=0,posti=0,now_sec=0;
		u8 yubuf[64]={0},BalanceLen=0,i=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				BalanceLen = MyEE_ReadWord(BalanceLenAdd);//从Flash获取余额信息长度
				for(i=0; i<BalanceLen;i++)
					yubuf[i]=MyEE_ReadWord(BalanceTextAdd+i);//从Flash获取余额信息
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/Json/UpdateRemain\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();
				if(EncryptMode==1)length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen(MD5_BUF);//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9);//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF);//2-8
				UART_string("AT+HTTPDATA=",1);
				if(EncryptMode==9)UART_string(U16IntToStr(length_buffer+BalanceLen+29),1);//上传数据长度
				else UART_string(U16IntToStr(length_buffer+BalanceLen+24),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{	
						UART_string("ID=",1);//3
						UART_string(id_buf,1);
						UART_string("&Message=",1);//9
						UART_string(yubuf,1);
						if(EncryptMode==9)
						{
								UART_string("&Login=",1);//7
								UART_string((unsigned char*)login9,1);
								UART_string("&Password=",1);//10
								UART_string((unsigned char*)password9,1);
						}
						else
						{
								UART_string("&sign=",1);//6
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("&once=",1);//6
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);//1
								else UART_string(U16IntToStr(now_sec),1);//2-8
						}
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
							delay_ms(HTTP200DELAY);
							if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				else
				{
						UART_string_newline("AT+HTTPTERM",1);
						delay_ms(100);
						return 1;
				}
		}
		else
		 return 2;//错误
}

//***********************
//描述:POST基本的数据
//返回:0:成功 1:失败
u16 PackLen=0;	
u8 _hj212_pack[500];	
u8 _pdata[500];
u8 POST_HTTP_BasicData_MOTHOD(void)
{

	    u8 SuccessFlag=0;
	    u8 ConnectStatua=0;
	    u8 Queue_BUFF=0;
	    BaseType_t err;
	    char ATsendbuff[20]={0};
		clear_Packbuff();
		clear_Pdatabuff();
	    _hj212_packdatatoUP(_hj212_pack,_pdata,FlashST,2011,FlashMN);
	    PackLen=strlen(_hj212_pack);    
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
			sprintf(ATsendbuff,"AT+CIPSEND=%d",PackLen);
			UART_string_newline(ATsendbuff,1);
			delay_ms(500);
			clear_USART1buff();
			UART1_BUF_SEND(_hj212_pack,PackLen);
			#if  Debugpritn 
			UART2_BUF_SEND(_hj212_pack,PackLen);
			#endif
			SuccessFlag=Find_StringforGSMUART_buff("SEND OK",50,0);
			if(SuccessFlag)
			{
				#if  Debugpritn 
				UART_string_newline("测试数据发送成功\r\n",2);
				#endif
				clear_USART1buff();
				UART_string_newline("AT+CIPRXGET=2,1000",1);
				ConnectStatua=Find_StringforGSMUART_buff("OK",20,0);
                if(ConnectStatua) 
				{
					if(strsearch(GSMUART_buff,"##"))
					{
						#if  Debugpritn 
						UART_string_newline("测试数据接收服务器返回成功\r\n",2);
						#endif
						AnalysisRrceiveDataNOAT();
						return 1;
					}
					else 
					{
						AnalysisRrceiveDataNOAT();
						return 0;
					}
					
				}
				else return 0;
			}
			else
			{
				#if  Debugpritn 
				UART_string_newline("测试数据发送失败\r\n",2);
				#endif				
				return 0;
			}
		}
		else
		{
			#if  Debugpritn 
				UART_string_newline("TCPIP断开连接\r\n",2);
			#endif		
			return 2;			
		}
}



void clear_Packbuff(void)
{
		u16 i=0;
		for(i=0;i<500;i++)_hj212_pack[i]=0;
}
void clear_Pdatabuff(void)
{
		u16 i=0;
		for(i=0;i<500;i++)_pdata[i]=0;
}
//***********************
//描述:POST报警
//返回:0:成功 1:失败
u8 POST_HTTP_AlarmData_MOTHOD(void)
{
		u32 ID_buffer=0;
		u8 sign_buf[64]={0},id_buf[10]={0};
		u16 length_buffer=0,posti=0,now_sec=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/Json/Push\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();	
				if(EncryptMode==1)length_buffer=strlen(MD5_BUF)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen((char*)id_buf);//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9);//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF);//2-8
				length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_Status))+strlen((char*)S16IntToStr(SysGsmRssi))+strlen((char*)id_buf)
				+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_S))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_CSA))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_CSB))
				+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_CSC))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_LS))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_TS1))
				+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_TS2))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_TS3))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_TS4))
				+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_VSA))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_VSB))+strlen((char*)S16IntToStr(NEW_status_FT.RTCM_VSC));
				if(EncryptMode==9)length_buffer=length_buffer+19+14+7+10+10+6+21+8+7+7+7+6+7+7+7+7+7+7+7+3;
				else length_buffer=length_buffer+18+10+7+10+10+6+21+8+7+7+7+6+7+7+7+7+7+7+7+3;	
				if(abs(NEW_DATA_FT.R_RTCM_Acurrent)!=32768)length_buffer=length_buffer+strlen((char*)U16IntToStr(NEW_DATA_FT.R_RTCM_Acurrent))+6;
				if(abs(NEW_DATA_FT.R_RTCM_Bcurrent)!=32768)length_buffer=length_buffer+strlen((char*)U16IntToStr(NEW_DATA_FT.R_RTCM_Bcurrent))+6;
				if(abs(NEW_DATA_FT.R_RTCM_Ccurrent)!=32768)length_buffer=length_buffer+strlen((char*)U16IntToStr(NEW_DATA_FT.R_RTCM_Ccurrent))+6;
				if(abs(NEW_DATA_FT.R_RTCM_Lcurrent)!=32768)length_buffer=length_buffer+strlen((char*)U16IntToStr(NEW_DATA_FT.R_RTCM_Lcurrent))+5;
				if(abs(NEW_DATA_FT.R_RTCM_Atemp)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Atemp))+6;
				if(abs(NEW_DATA_FT.R_RTCM_Btemp)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Btemp))+6;
				if(abs(NEW_DATA_FT.R_RTCM_Ctemp)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Ctemp))+6;
				if(abs(NEW_DATA_FT.R_RTCM_Etemp)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Etemp))+6;	
				if(abs(NEW_DATA_FT.R_RTCM_Avoltage)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Avoltage))+6;	
				if(abs(NEW_DATA_FT.R_RTCM_Bvoltage)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Bvoltage))+6;	
				if(abs(NEW_DATA_FT.R_RTCM_Cvoltage)!=32768)length_buffer=length_buffer+strlen((char*)S16IntToStr(NEW_DATA_FT.R_RTCM_Cvoltage))+6;	
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{
						if(EncryptMode==9)
						{	
								UART_string("JsonData={\"Login\":\"",1);//19
								UART_string((unsigned char*)login9,1);
								UART_string("\",\"Password\":\"",1);//14
								UART_string((unsigned char*)password9,1);		
						}
						else
						{
								UART_string("JsonData={\"sign\":\"",1);//18
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("\",\"once\":\"",1);//10
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);	
								else UART_string(U16IntToStr(now_sec),1);	
						}
						UART_string("\",\"ID\":",1);//7
						UART_string(id_buf,1);
						UART_string(",\"Status\":",1);//10
						UART_string(S16IntToStr(NEW_status_FT.RTCM_Status),1);
						UART_string(",\"Signal\":",1);//10
						UART_string(U16IntToStr(SysGsmRssi),1);
						UART_string(",\"N\":1",1);//6
						UART_string(",\"ProbeData\":[{\"PID\":",1);//21
						UART_string(id_buf,1);
						UART_string("001,\"S\":",1);//8
						UART_string(S16IntToStr(NEW_status_FT.RTCM_S),1);
						UART_string(",\"CSA\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_CSA),1);
						UART_string(",\"CSB\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_CSB),1);
						UART_string(",\"CSC\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_CSC),1);
						UART_string(",\"LS\":",1);//6
						UART_string(S16IntToStr(NEW_status_FT.RTCM_LS),1);
						UART_string(",\"TS1\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_TS1),1);
						UART_string(",\"TS2\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_TS2),1);
						UART_string(",\"TS3\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_TS3),1);
						UART_string(",\"TS4\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_TS4),1);
						UART_string(",\"VSA\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_VSA),1);
						UART_string(",\"VSB\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_VSB),1);
						UART_string(",\"VSC\":",1);//7
						UART_string(S16IntToStr(NEW_status_FT.RTCM_VSC),1);
						if(abs(NEW_DATA_FT.R_RTCM_Acurrent)!=32768)
						{
								UART_string(",\"CA\":",1);//6
								UART_string(U16IntToStr(NEW_DATA_FT.R_RTCM_Acurrent),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Bcurrent)!=32768)
						{
								UART_string(",\"CB\":",1);//6
								UART_string(U16IntToStr(NEW_DATA_FT.R_RTCM_Bcurrent),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Ccurrent)!=32768)
						{
								UART_string(",\"CC\":",1);//6
								UART_string(U16IntToStr(NEW_DATA_FT.R_RTCM_Ccurrent),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Lcurrent)!=32768)
						{
								UART_string(",\"L\":",1);//5
								UART_string(U16IntToStr(NEW_DATA_FT.R_RTCM_Lcurrent),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Atemp)!=32768)
						{
								UART_string(",\"T1\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Atemp),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Btemp)!=32768)
						{
								UART_string(",\"T2\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Btemp),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Ctemp)!=32768)
						{
								UART_string(",\"T3\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Ctemp),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Etemp)!=32768)
						{
								UART_string(",\"T4\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Etemp),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Avoltage)!=32768)
						{
								UART_string(",\"VA\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Avoltage),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Bvoltage)!=32768)
						{
								UART_string(",\"VB\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Bvoltage),1);
						}
						if(abs(NEW_DATA_FT.R_RTCM_Cvoltage)!=32768)
						{
								UART_string(",\"VC\":",1);//6
								UART_string(S16IntToStr(NEW_DATA_FT.R_RTCM_Cvoltage),1);
						}
						UART_string("}]}",1);//3
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
							delay_ms(HTTP200DELAY);
							if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				UART_string_newline("AT+HTTPTERM",1);
				delay_ms(100);
				return 1;
		}
		else
			return 2;
}


//***********************
//描述:GET服务器上的请求标志位，并作处理
//返回:0:成功 1:失败
u8 GET_HTTP_Flags(void)
{
		u32 ID_buffer=0;
		u8 id_buf[10]={0};
		u16 posflag=0,posti=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				HTTP_INIT_MOTHOD();
				UART_string("/Json/getConfig?ID=",1);//配置push"URL"
				UART_string(id_buf,1);
				UART_string_newline("\"",1);
				delay_ms(100);
				clear_USART1buff();
				UART_string_newline("AT+HTTPACTION=0",1);//开始GET
				for(posti=0;posti<HTTP200CNT;posti++)
				{
						delay_ms(HTTP200DELAY);
						if(strsearch(GSMUART_buff,"200"))break;
				}
				if(strsearch(GSMUART_buff,",200,"))
				{
						clear_USART1buff();
						UART_string_newline("AT+HTTPREAD=0,37",1);//读服务器内容前37个字节
						delay_ms(500);
						posflag=strsearch(GSMUART_buff,"FlagBit\":\"");//获取关键字位置
						CoypArraryAtoArraryB_LR(GSMUART_buff,get_http_flagbit,16,posflag+10-1);//复制要获取的部分数组
						Handle_mark_bit();
						UART_string_newline("AT+HTTPTERM",1);
						delay_ms(100);
						return 0;
				}
				else
				{
						UART_string_newline("AT+HTTPTERM",1);
						delay_ms(100);
						return 1;
				}
		}
		else
		  return 2;
}

//描述:处理HTTP返回来的标志位，数组是get_http_flagbit
void Handle_mark_bit(void)
{
		u8 Queue_BUFF=0;
//		static u8 fw_flag=0;
		BaseType_t err;
//		if(fw_flag==1)
//		{
//				if(get_http_flagbit[0]=='0')//已经清楚标志位，可以复位了
//				{
//						Queue_BUFF=W_RESET_Task;
//						err=xQueueSendToFront(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);
//						fw_flag=0;
//				}
//		}
//		if(get_http_flagbit[0]=='1')//服务器请求复位
//		{
//				clear_FlagBIT();
//				FlagBIT_BUF[0]='1';
//				Queue_BUFF=POST_HTTP_FlagBIT;
//				err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//首先清复位标志位
//				fw_flag=1;
//		}
//		if(get_http_flagbit[1]=='1')//服务器请求自检
//		{
//				clear_FlagBIT();
//				FlagBIT_BUF[1]='1';
//				Queue_BUFF=POST_HTTP_FlagBIT;
//				err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);
//				Queue_BUFF=W_Self_Check_Task;
//				err=xQueueSendToFront(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);
//		}
//		if(get_http_flagbit[2]=='1')//服务器请求数据更新
//		{
//				Queue_BUFF=POST_HTTP_BasicData;
//				err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//上传基本数据
//		}
		if(get_http_flagbit[3]=='1'||get_http_flagbit[5]=='1')//服务器中的门限修改或配置信息有修改
		{
				Queue_BUFF=GET_HTTP_Config;
				err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//GET配置信息
		}
//		if(get_http_flagbit[4]=='1')//服务器请求配置信息
//		{
//		}
}

//***********************
//描述:POST清除标志位
//返回:0:成功 1:失败
u8 POST_HTTP_Clear_MOTHOD(u8 *FlagBIT)
{
		u8 sign_buf[64]={0},id_buf[10]={0};
		u32 ID_buffer=0;
	  u16 length_buffer=0,posti=0,now_sec=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/Json/UpdateConfig\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();
				if(EncryptMode==1)length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen(MD5_BUF)+16+18+10+7+12+2;//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9)+16+19+14+7+12+2;//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF)+16+18+10+7+12+2;//2-8
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{	
						if(EncryptMode==9)
						{	
								UART_string("JsonData={\"Login\":\"",1);//19
								UART_string((unsigned char*)login9,1);
								UART_string("\",\"Password\":\"",1);//14
								UART_string((unsigned char*)password9,1);		
						}
						else
						{
								UART_string("JsonData={\"sign\":\"",1);//18
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("\",\"once\":\"",1);//10
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);	
								else UART_string(U16IntToStr(now_sec),1);	
						}
						UART_string("\",\"ID\":",1);//7
						UART_string(id_buf,1);
						UART_string(",\"FlagBit\":\"",1);//12
						UART_string(FlagBIT,1);
						UART_string("\"}",1);//2
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
							delay_ms(HTTP200DELAY);
							if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				UART_string_newline("AT+HTTPTERM",1);
				delay_ms(100);
				return 1;
		}
		else
				return 2;//错误
}

void POST_HTTP_Frist_MOTHOD(void)
{
		u8 Queue_BUFF=0,j=0;
		BaseType_t err;
		for(j=0;j<10;j++)
		{
				GPRS_FALUT_STATUS=POST_HTTP_Alarming_Value_MOTHOD();//上传门限值
				if(GPRS_FALUT_STATUS==0)break;
				if(GPRS_FALUT_STATUS==2)gsm_gprs_init();
		}
		if(j==10)
		{
				Queue_BUFF=ASSIT_SystemReset;
				err=xQueueSend(ASSIT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//重启
		}
		for(j=0;j<10;j++)
		{
				GPRS_FALUT_STATUS=POST_HTTP_INSTALL_MOTHOD();//上传安装信息
				if(GPRS_FALUT_STATUS==0)break;
				if(GPRS_FALUT_STATUS==2)gsm_gprs_init();
		}
		if(j==10)
		{
				Queue_BUFF=ASSIT_SystemReset;
				err=xQueueSend(ASSIT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//重启
		}
		for(j=0;j<10;j++)
		{
				GPRS_FALUT_STATUS=POST_HTTP_Ver_MOTHOD();//上传版本信息
				if(GPRS_FALUT_STATUS==0)break;
				if(GPRS_FALUT_STATUS==2)gsm_gprs_init();
		}
		if(j==10)
		{
				Queue_BUFF=ASSIT_SystemReset;
				err=xQueueSend(ASSIT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//重启
		}
}

//***********************
//描述:POST安装信息
//返回:0:成功 1:失败
u8 POST_HTTP_INSTALL_MOTHOD(void)
{
		u32 ID_buffer=0;
		u16 length_buffer=0,posti=0,now_sec=0;
		u8 sign_buf[64]={0},id_buf[10]={0};
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/json/push\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();
				if(EncryptMode==1)length_buffer=strlen(MD5_BUF)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen((char*)id_buf);//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9);//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF);//2-8
				length_buffer=length_buffer+strlen((char*)U16IntToStr(SysGsmRssi))+strlen((char*)id_buf)
				+strlen((char*)S16IntToStr(FT_status.RTCM_CSA))+strlen((char*)S16IntToStr(FT_status.RTCM_CSB))
				+strlen((char*)S16IntToStr(FT_status.RTCM_CSC))+strlen((char*)S16IntToStr(FT_status.RTCM_LS))+strlen((char*)S16IntToStr(FT_status.RTCM_TS1))
				+strlen((char*)S16IntToStr(FT_status.RTCM_TS2))+strlen((char*)S16IntToStr(FT_status.RTCM_TS3))+strlen((char*)S16IntToStr(FT_status.RTCM_TS4))
				+strlen((char*)S16IntToStr(FT_status.RTCM_VSA))+strlen((char*)S16IntToStr(FT_status.RTCM_VSB))+strlen((char*)S16IntToStr(FT_status.RTCM_VSC));
				if(EncryptMode==9)length_buffer=length_buffer+19+14+7+10+21+10+7+7+6+7+7+7+7+7+7+7+3;
				else length_buffer=length_buffer+18+10+7+10+21+10+7+7+6+7+7+7+7+7+7+7+3;
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{	
						if(EncryptMode==9)
						{	
								UART_string("JsonData={\"Login\":\"",1);//19
								UART_string((unsigned char*)login9,1);
								UART_string("\",\"Password\":\"",1);//14
								UART_string((unsigned char*)password9,1);		
						}
						else
						{
								UART_string("JsonData={\"sign\":\"",1);//18
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("\",\"once\":\"",1);//10
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);	
								else UART_string(U16IntToStr(now_sec),1);	
						}
						UART_string("\",\"ID\":",1);//7
						UART_string(id_buf,1);
						UART_string(",\"Signal\":",1);//10
						UART_string(U16IntToStr(SysGsmRssi),1);
						UART_string(",\"ProbeData\":[{\"PID\":",1);//21
						UART_string(id_buf,1);
						UART_string("001,\"CSA\":",1);//10
						UART_string(S16IntToStr(FT_status.RTCM_CSA),1);
						UART_string(",\"CSB\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_CSB),1);
						UART_string(",\"CSC\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_CSC),1);
						UART_string(",\"LS\":",1);//6
						UART_string(S16IntToStr(FT_status.RTCM_LS),1);
						UART_string(",\"TS1\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_TS1),1);
						UART_string(",\"TS2\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_TS2),1);
						UART_string(",\"TS3\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_TS3),1);
						UART_string(",\"TS4\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_TS4),1);
						UART_string(",\"VSA\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_VSA),1);
						UART_string(",\"VSB\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_VSB),1);
						UART_string(",\"VSC\":",1);//7
						UART_string(S16IntToStr(FT_status.RTCM_VSC),1);
						UART_string("}]}",1);//3
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
							delay_ms(HTTP200DELAY);
							if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				else
				{
						UART_string_newline("AT+HTTPTERM",1);
						delay_ms(100);
						return 1;
				}
		}
		else
			return 2;
}

//***********************
//描述:POST版本信息
//返回:0:成功 1:失败
u16 hw_sim800c=0,sw_sim800c=0;
u8 POST_HTTP_Ver_MOTHOD(void)
{
		u32 ID_buffer=0;
		u16 length_buffer=0,posti=0,now_sec=0;
		u8 sign_buf[64]={0},id_buf[10]={0};
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{
				hw_sim800c=MyEE_ReadWord(SIM800C_PCBA_HW_Ver);//从Flash获取SIM800C_PSBA的硬件版本号
				sw_sim800c=MyEE_ReadWord(SIM800C_PCBA_SW_Ver);//从Flash获取SIM800C_PSBA的软件版本号
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/Json/UpdateConfig\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();
				if(EncryptMode==1)length_buffer=strlen(MD5_BUF)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen((char*)id_buf);//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9);//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF);//2-8
				length_buffer=length_buffer+strlen((char*)U16IntToStr(hw_sim800c))+strlen((char*)U16IntToStr(sw_sim800c))+strlen((char*)id_buf)
				+strlen((char*)U16IntToStr(FT_rtcm.R_RTCM_HardwareVersion_Number))+strlen((char*)U16IntToStr(FT_rtcm.R_RTCM_SoftwareVersion_Number));
				if(EncryptMode==9)length_buffer=length_buffer+19+14+7+8+1+24+11+1+4;
				else length_buffer=length_buffer+18+10+7+8+1+24+11+1+4;
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{	
						if(EncryptMode==9)
						{	
								UART_string("JsonData={\"Login\":\"",1);//19
								UART_string((unsigned char*)login9,1);
								UART_string("\",\"Password\":\"",1);//14
								UART_string((unsigned char*)password9,1);		
						}
						else
						{
								UART_string("JsonData={\"sign\":\"",1);//18
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("\",\"once\":\"",1);//10
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);	
								else UART_string(U16IntToStr(now_sec),1);	
						}
						UART_string("\",\"ID\":",1);//7
						UART_string(id_buf,1);
						UART_string(",\"Ver\":\"",1);//8
						UART_string(U16IntToStr(hw_sim800c),1);
						UART_string("|",1);//1
						UART_string(U16IntToStr(sw_sim800c),1);
						UART_string("\",\"ProbeConfig\":[{\"PID\":",1);//24
						UART_string(id_buf,1);
						UART_string("001,\"Ver\":\"",1);//11
						UART_string(U16IntToStr(FT_rtcm.R_RTCM_HardwareVersion_Number),1);
						UART_string("|",1);//1
						UART_string(U16IntToStr(FT_rtcm.R_RTCM_SoftwareVersion_Number),1);
						UART_string("\"}]}",1);//4
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
							delay_ms(HTTP200DELAY);
							if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				else
				{
						UART_string_newline("AT+HTTPTERM",1);
						delay_ms(100);
						return 1;
				}
		}
		else
			return 2;
}

//***********************
//描述:POST门限值
//返回:0:成功 1:失败
u8 POST_HTTP_Alarming_Value_MOTHOD(void)
{
		u32 ID_buffer=0;
		u16 length_buffer=0,posti=0,now_sec=0;
		u8 sign_buf[64]={0},id_buf[10]={0};
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{
				hw_sim800c=MyEE_ReadWord(SIM800C_PCBA_HW_Ver);//从Flash获取SIM800C_PSBA的硬件版本号
				sw_sim800c=MyEE_ReadWord(SIM800C_PCBA_SW_Ver);//从Flash获取SIM800C_PSBA的软件版本号
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/Json/UpdateConfig\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();
				if(EncryptMode==1)length_buffer=strlen(MD5_BUF)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen((char*)id_buf);//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9);//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF);//2-8
				length_buffer=length_buffer+strlen((char*)id_buf)+strlen((char*)U16IntToStr(FT_rtcm.R_RTCM_LcAvalue))+strlen((char*)S16IntToStr(FT_rtcm.R_RTCM_AtAvalue))
				+strlen((char*)S16IntToStr(FT_rtcm.R_RTCM_BtAvalue))+strlen((char*)S16IntToStr(FT_rtcm.R_RTCM_CtAvalue))+strlen((char*)S16IntToStr(FT_rtcm.R_RTCM_EtAvalue))
				+strlen((char*)U16IntToStr(FT_rtcm.R_RTCM_AcAvalue))+strlen((char*)U16IntToStr(FT_rtcm.R_RTCM_BcAvalue))+strlen((char*)U16IntToStr(FT_rtcm.R_RTCM_CcAvalue));
				if(EncryptMode==9)length_buffer=length_buffer+19+14+7+23+3+6+7+7+7+7+7+7+7+3;
				else length_buffer=length_buffer+18+10+7+23+3+6+7+7+7+7+7+7+7+3;
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{	
						if(EncryptMode==9)
						{	
								UART_string("JsonData={\"Login\":\"",1);//19
								UART_string((unsigned char*)login9,1);
								UART_string("\",\"Password\":\"",1);//14
								UART_string((unsigned char*)password9,1);		
						}
						else
						{
								UART_string("JsonData={\"sign\":\"",1);//18
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("\",\"once\":\"",1);//10
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);	
								else UART_string(U16IntToStr(now_sec),1);	
						}
						UART_string("\",\"ID\":",1);//7
						UART_string(id_buf,1);
						UART_string(",\"ProbeConfig\":[{\"PID\":",1);//23
						UART_string(id_buf,1);
						UART_string("001",1);//3
						UART_string(",\"LA\":",1);//6
						UART_string(U16IntToStr(FT_rtcm.R_RTCM_LcAvalue),1);
						UART_string(",\"TA1\":",1);//7
						UART_string(S16IntToStr(FT_rtcm.R_RTCM_AtAvalue),1);
						UART_string(",\"TA2\":",1);//7
						UART_string(S16IntToStr(FT_rtcm.R_RTCM_BtAvalue),1);
						UART_string(",\"TA3\":",1);//7
						UART_string(S16IntToStr(FT_rtcm.R_RTCM_CtAvalue),1);
						UART_string(",\"TA4\":",1);//7
						UART_string(S16IntToStr(FT_rtcm.R_RTCM_EtAvalue),1);
						UART_string(",\"CAA\":",1);//7
						if(FT_rtcm.R_RTCM_AcAvalue==0)FT_rtcm.R_RTCM_AcAvalue=2;
						UART_string(U16IntToStr(FT_rtcm.R_RTCM_AcAvalue),1);
						UART_string(",\"CAB\":",1);//7
						if(FT_rtcm.R_RTCM_BcAvalue==0)FT_rtcm.R_RTCM_BcAvalue=2;
						UART_string(U16IntToStr(FT_rtcm.R_RTCM_BcAvalue),1);
						UART_string(",\"CAC\":",1);//7
						if(FT_rtcm.R_RTCM_CcAvalue==0)FT_rtcm.R_RTCM_CcAvalue=2;
						UART_string(U16IntToStr(FT_rtcm.R_RTCM_CcAvalue),1);
						UART_string("}]}",1);//3
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
							delay_ms(HTTP200DELAY);
							if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				else
				{	
						UART_string_newline("AT+HTTPTERM",1);
					  delay_ms(100);
						return 1;
				}
		}
		else
		  return 2;
} 

u8 GET_HTTP_Config_MOTHOD(void)
{
		u32 ID_buffer=0;
		u8 id_buf[10]={0};
		u16 posti=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				HTTP_INIT_MOTHOD();
				UART_string("/Json/getConfig?ID=",1);//配置push"URL"
				UART_string(id_buf,1);
				UART_string("&Entire=1",1);
				UART_string_newline("\"",1);
				delay_ms(100);
				clear_USART1buff();
				UART_string_newline("AT+HTTPACTION=0",1);//开始GET
				for(posti=0;posti<HTTP200CNT;posti++)
				{
						delay_ms(HTTP200DELAY);
						if(strsearch(GSMUART_buff,"200"))break;
				}
				if(strsearch(GSMUART_buff,",200,"))
				{
						clear_USART1buff();
						UART_string_newline("AT+HTTPREAD=0,1536",1);//读服务器内容
						delay_ms(1000);
						anlysis_config();
						return 0;
				}
				UART_string_newline("AT+HTTPTERM",1);
				delay_ms(100);
			  return 1;
		}
		else
		  return 2; 
}
#if SMS_Function
u8 MobileList_BUF[128]={0},WhiteList_BUF[128]={0};//手机号码全局数组，方便调试
#endif
u8 URL_BUF[50]={0};//服务器GET的URL全局数组，方便调试
void anlysis_config(void)
{
		u16 pos=0,pushrate_len=0,anlysis_i=0;
		u8 pushrate_buf[16]={0},Queue_BUFF=0;
		BaseType_t err;
		#if SMS_Function
		u8 Data_BUF[200]={0};
		#endif
		#if SMS_Function
		pos=strsearch(GSMUART_buff,"IsSMS\":");//获取短信报警配置
		if(pos)
		{
				if(GSMUART_buff[pos+7-1]=='t')
					IsSMS=1;//需要
				else if(GSMUART_buff[pos+7-1]=='f')
					IsSMS=0;//不需要
		}
		pos=0;
		#endif
		pos=strsearch(GSMUART_buff,"PushMode\":");//获取上传模式配置
		if(pos)
		{
				if(GSMUART_buff[pos+10-1]=='0')
					PushMode=0;//默认模式
				else if(GSMUART_buff[pos+10-1]=='1')					
				{
						PushMode=1;//根据PushRate比较变化量，不超限10分钟上传一次，超限立即上传
						pos=0;
						pos=strsearch(GSMUART_buff,"PushRate\":");
						for(pushrate_len=0;GSMUART_buff[pos+10-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+10-1+pushrate_len];
						PushRate=stingcnt_to_uint32(pushrate_buf);//数组转十进制
				}
		}
		pos=0;
		#if SMS_Function
		pos=strsearch(GSMUART_buff,"Address\":\"");//获取安装地址
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+10-1+pushrate_len]!='\"';pushrate_len++)
							Data_BUF[pushrate_len]=GSMUART_buff[pos+10-1+pushrate_len];
				FLASH_Unlock();
				MyEE_WriteWord(ADDRESSLenAdd,strlen((char*)Data_BUF));
				for(pushrate_len=0;pushrate_len<strlen((char*)Data_BUF);pushrate_len++)
						MyEE_WriteWord(ADDRESSAdd+pushrate_len,Data_BUF[pushrate_len]);
				FLASH_Lock(); 
				for(anlysis_i=0;anlysis_i<200;anlysis_i++)//清除缓存
					Data_BUF[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"SimName\":\"");//获取探测点详细说明
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+10-1+pushrate_len]!='\"';pushrate_len++)
							Data_BUF[pushrate_len]=GSMUART_buff[pos+10-1+pushrate_len];
				FLASH_Unlock();
				MyEE_WriteWord(SimNameLenAdd,strlen((char*)Data_BUF));
				for(pushrate_len=0;pushrate_len<strlen((char*)Data_BUF);pushrate_len++)
						MyEE_WriteWord(SimNameAdd+pushrate_len,Data_BUF[pushrate_len]);
				FLASH_Lock(); 
				for(anlysis_i=0;anlysis_i<200;anlysis_i++)//清除缓存
					Data_BUF[anlysis_i]=0;
		}
		pos=0;
		#endif
		pos=strsearch(GSMUART_buff,"LA\":");//获取漏电流阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+4-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+4-1+pushrate_len];
				FT_rtcm.R_RTCM_LcAvalue=stingcnt_to_uint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"TA1\":");//获取A相温度阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_AtAvalue=stingcnt_to_sint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"TA2\":");//获取B相温度阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_BtAvalue=stingcnt_to_sint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"TA3\":");//获取C相温度阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_CtAvalue=stingcnt_to_sint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"TA4\":");//获取环境温度阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_EtAvalue=stingcnt_to_sint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"CAA\":");//获取A相电流阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_AcAvalue=stingcnt_to_uint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"CAB\":");//获取B相电流阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_BcAvalue=stingcnt_to_uint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"CAC\":");//获取B相电流阀值
		if(pos)
		{
				for(pushrate_len=0;GSMUART_buff[pos+5-1+pushrate_len]!=',';pushrate_len++)
							pushrate_buf[pushrate_len]=GSMUART_buff[pos+5-1+pushrate_len];
				FT_rtcm.R_RTCM_CcAvalue=stingcnt_to_uint32(pushrate_buf);//数组转十进制
				for(anlysis_i=0;anlysis_i<16;anlysis_i++)//清除缓存
					pushrate_buf[anlysis_i]=0;
		}
		pos=0;
		#if SMS_Function
		pos=strsearch(GSMUART_buff,"MobileList\":null");//无号码
		if(pos==0)
		{
				pos=strsearch(GSMUART_buff,"MobileList\":\"");//获取短信报警手机号码，针对于有号码的
				if(pos)
				{
						for(anlysis_i=0;anlysis_i<128;anlysis_i++)//清除缓存
							MobileList_BUF[anlysis_i]=0;
						for(pushrate_len=0;GSMUART_buff[pos+13-1+pushrate_len]!='\"';pushrate_len++)
								MobileList_BUF[pushrate_len]=GSMUART_buff[pos+13-1+pushrate_len];
						PhoneNumlist(MobileList_BUF,0);//将号码存到Flash
				}
		}
		else
		{
				FLASH_Unlock();
				MyEE_WriteWord(MobileCntAdd,0);
				FLASH_Lock();
		}
		pos=0;
		pos=strsearch(GSMUART_buff,"WhiteList\":null");//无号码
		if(pos==0)
		{
				pos=strsearch(GSMUART_buff,"WhiteList\":\"");//获取操作权限手机号码，针对于有号码的
				if(pos)
				{
						for(anlysis_i=0;anlysis_i<128;anlysis_i++)//清除缓存
							WhiteList_BUF[anlysis_i]=0;
						for(pushrate_len=0;GSMUART_buff[pos+12-1+pushrate_len]!='\"';pushrate_len++)
								WhiteList_BUF[pushrate_len]=GSMUART_buff[pos+12-1+pushrate_len];
						PhoneNumlist(WhiteList_BUF,1);//将号码存到Flash
				}
		}
		else
		{
				FLASH_Unlock();
				MyEE_WriteWord(WhiteListCntAdd,0);
				FLASH_Lock();
		}
		pos=0;
		#endif
//		pos=strsearch(GSMUART_buff,"Url\":null");//无URL
//		if(pos==0)
//		{
//				pos=strsearch(GSMUART_buff,"Url\":\"");//查找Url位置
//				if(pos)
//				{
//						for(anlysis_i=0;anlysis_i<50;anlysis_i++)URL_BUF[anlysis_i]=0;//清除缓存URL_BUF
//						for(pushrate_len=0;GSMUART_buff[pos+6-1+pushrate_len]!='\"';pushrate_len++)
//								URL_BUF[pushrate_len]=GSMUART_buff[pos+6-1+pushrate_len];
//						SAVE_URL_FLASH(URL_BUF);
//				}
//		}
		clear_FlagBIT();
		FlagBIT_BUF[5]='1';
		Queue_BUFF=POST_HTTP_FlagBIT;err=xQueueSendToFront(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//清除标志位
		Queue_BUFF=W_Alarming_Value_Task;
		err=xQueueSendToFront(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//向下板发送写门限值指令
}

//***********************
//描述:POST手机号码数据
//返回:0:成功 1:失败
u8 POST_HTTP_Num_MOTHOD(void)
{
		u32 ID_buffer=0;
		u8 sign_buf[64]={0},id_buf[10]={0};
		u16 length_buffer=0,posti=0,now_sec=0;
		BaseType_t err;
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
		delay_ms(500); 
		if(!strsearch(GSMUART_buff,"0.0.0.0"))
		{	
				ID_buffer=MyEE_ReadWord(IDAddH)<<16|MyEE_ReadWord(IDAddL);//从Flash获取ID
				sprintf((char*)id_buf,"%d",ID_buffer);
				EncryptMode=MyEE_ReadWord(EncryptMode_Add);//从Flash中获取MODE
				now_sec=t_sec;
				switch(EncryptMode)
				{
						case 1:
										strcat((char*)sign_buf,(char*)login_password1);
										strcat((char*)sign_buf,(char*)U16IntToStr(SysGsmRssi));
										break;
						case 2:
										strcat((char*)sign_buf,(char*)login2);
										strcat((char*)sign_buf,(char*)password2);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 3:
										strcat((char*)sign_buf,(char*)login3);
										strcat((char*)sign_buf,(char*)password3);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 4:
										strcat((char*)sign_buf,(char*)login4);
										strcat((char*)sign_buf,(char*)password4);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 5:
										strcat((char*)sign_buf,(char*)login5);
										strcat((char*)sign_buf,(char*)password5);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 6:
										strcat((char*)sign_buf,(char*)login6);
										strcat((char*)sign_buf,(char*)password6);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 7:
										strcat((char*)sign_buf,(char*)login7);
										strcat((char*)sign_buf,(char*)password7);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
						case 8:
										strcat((char*)sign_buf,(char*)login8);
										strcat((char*)sign_buf,(char*)password8);
										strcat((char*)sign_buf,(char*)U16IntToStr(now_sec));
										break;
				}
				MD5(sign_buf);
				HTTP_INIT_MOTHOD();
				UART_string_newline("/Json/UpdateConfig\"",1);//配置push"URL"
				delay_ms(100);
				clear_USART1buff();		
				if(EncryptMode==1)length_buffer=strlen(MD5_BUF)+strlen((char*)U16IntToStr(SysGsmRssi))+strlen((char*)id_buf);//1
				else if(EncryptMode==9)length_buffer=strlen((char*)id_buf)+strlen((char*)login9)+strlen((char*)password9);//9
				else length_buffer=strlen((char*)id_buf)+strlen((char*)U16IntToStr(now_sec))+strlen(MD5_BUF);//2-8
				length_buffer=length_buffer+strlen((char*)NumBer);
				if(EncryptMode==9)length_buffer=length_buffer+19+14+7+11+2;
				else length_buffer=length_buffer+18+10+7+11+2;
				UART_string("AT+HTTPDATA=",1);
				UART_string(U16IntToStr(length_buffer),1);//上传数据长度
				UART_string_newline(",10000",1);//配置push的data长度
				delay_ms(500);
				if(strsearch(GSMUART_buff,"DOWNLOAD"))
				{
						if(EncryptMode==9)
						{	
								UART_string("JsonData={\"Login\":\"",1);//19
								UART_string((unsigned char*)login9,1);
								UART_string("\",\"Password\":\"",1);//14
								UART_string((unsigned char*)password9,1);		
						}
						else
						{
								UART_string("JsonData={\"sign\":\"",1);//18
								UART_string((unsigned char*)MD5_BUF,1);
								UART_string("\",\"once\":\"",1);//10
								if(EncryptMode==1)UART_string(U16IntToStr(SysGsmRssi),1);	
								else UART_string(U16IntToStr(now_sec),1);	
						}
						UART_string("\",\"ID\":",1);//7
						UART_string(id_buf,1);
						UART_string(",\"Mobile\":\"",1);//11
						UART_string(NumBer,1);
						UART_string("\"}",1);//2
						clear_USART1buff();
						delay_ms(1000);
						UART_string_newline("AT+HTTPACTION=1",1);//开始传送
						for(posti=0;posti<HTTP200CNT;posti++)
						{
								delay_ms(HTTP200DELAY);
								if(strsearch(GSMUART_buff,"200"))break;
						}
						if(strsearch(GSMUART_buff,",200,"))
						{
								clear_USART1buff();
								UART_string_newline("AT+HTTPREAD=0,100",1);//读服务器内容前100个字节
								delay_ms(500);
								if(strsearch(GSMUART_buff,"1\x0d\x0aOK"))
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 0;//成功获取
								}
								else
								{
										UART_string_newline("AT+HTTPTERM",1);
										delay_ms(100);
										return 1;
								}
						}
						else
						{
								UART_string_newline("AT+HTTPTERM",1);
								delay_ms(100);
								return 1;
						}
				}
				UART_string_newline("AT+HTTPTERM",1);
				delay_ms(100);
				return 1;
		}
		else
			return 2;
}


//描述：保存正常工作的URL到flash
void SAVE_URL_FLASH(u8 *DATABUF)
{
		u16 num_i=0,num_cnt=0;
		num_cnt=strlen((char *)DATABUF);
		FLASH_Unlock();
		for(num_i=0;DATABUF[num_i]!=0;num_i++)MyEE_WriteWord(HTTP_URL_Add+num_i,DATABUF[num_i]);//写正常工作的URL到FLASH
		MyEE_WriteWord(HTTP_URL_LenAdd,num_cnt);//写正常工作的URL的长度到FLASH
		FLASH_Lock();
}

//描述：保存生产工作的URL到flash
void SAVE_SCURL_FLASH(u8 *DATABUF)
{
		u16 num_i=0,num_cnt=0;
		num_cnt=strlen((char *)DATABUF);
		FLASH_Unlock();
		for(num_i=0;DATABUF[num_i]!=0;num_i++)MyEE_WriteWord(HTTP_SCURL_Add+num_i,DATABUF[num_i]);//写生产工作的URL的长度到FLASH
		MyEE_WriteWord(HTTP_SCURL_LenAdd,num_cnt);
		FLASH_Lock();
}

