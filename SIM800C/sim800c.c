#include "sim800c.h"
#include "pc.h"

extern u8 GPRS_FALUT_STATUS;
extern u8 LCD_CHANGE_NEED;
u8 SimOperator=0;//SIM卡的运营商,0:没有检测到 1:移动 2:联通 
u8 SimInsertGsmStatus=0;//SIM卡是否插入,(gsm模块返回的状态)0:没有 1:有
u8 g_gsm_status=0;//GSM 状态
u8 SysGsmTemp=0;//系统中gsm模块的温度
u16 SysGsmVoltage=0;//系统中gsm模块的电压
u16 SysGsmRssi=0;//系统中gsm当前的信号强度,这里是百分比
u8 GSMUART_buff[MAX_GSMUART];//GSM串口缓存数组
u16 GSMUART_cnt=0;//GSM串口接收字节的个数
u8 GSMUART_flag=0;//GSM串口状态
//Coneo
u8 IMEIbuff[18]={0};//IMEI
u8 TIMEbuff[25]={0};
u8 TIMEbufftoSim[30]={0};
NETWROKTIME  Netwroktime;
//*************************************************
//描述:清除GSM串口数组缓存
void clear_USART1buff(void)
{
		u16 i=0;
		for(i=0;i<MAX_GSMUART;i++)GSMUART_buff[i]=0;
		GSMUART_cnt=0;
		GSMUART_flag=0;
}

//*************************************************
//描述:从p1中查找p2字符串位置
//参数:字符串p1,p2
//注意:p1中不能空格0x00 
//返回:返回位置pos
u16 EndstrsearchGSMUART_buff(const u8 *p2)
{
		u16 p2_len=0;
		s16 i=0,j=0,k=0;
		u16 pos=0;
		if(!*p2)return 0;
		p2_len=strlen((char const *)p2);
		for(i=MAX_GSMUART-1;i>=0;i--)
		{
			if(GSMUART_buff[i]==*(p2))
			{
					for(k=i,j=0;j<p2_len;k++,j++)
					{
							if(GSMUART_buff[k]==*(p2+j))
							{
									pos=i+1;
									if(j==p2_len-1)return pos;
							}
							else{pos = 0;break;}
					}
			} 
		}
		return pos;
}

u16 GSMatoi(char *s)  
{  
    u16 i;  
    u16 n = 0;  
    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)  
    {  
        n = 10 * n + (s[i] - '0');  
    }  
    return n;  
} 

//*************************************************
//描述:从GSMUART_buff中查找p2字符串位置
//参数:字符串p2
//返回:返回位置pos
u16 HEXsearchforGSMUART_buff(const u8 *p2,const u16 p2_len)
{
		u16 p1_len=0;
		u16 i=0,j=0,k=0;
		u16 pos=0;
		if(!*p2)return 0;
		p1_len=MAX_GSMUART;
		for(i=0;i<p1_len;i++)
		{
			if(GSMUART_buff[i]==*(p2))
			{
				for(k=i,j=0;j<p2_len;k++,j++)
				{
					if(GSMUART_buff[k]==*(p2+j))
					{
						pos=i+1;
						if(j==p2_len-1)return pos;
					}
					else{pos = 0;break;}
				}
			} 
		}
		return pos;
}

void analysis_upgrade(void)
{
		u16 File_Total=0;
	    u16 test1,test2,test3;
		u16 posid1=0,posid2=0,posid3=0;
		u16 updatabit=0,Vernum=0;
		u8 updatabitbuf[4]={0},Vernumbuf[8]={0},Filetotalbuf[8]={0};	
		posid1=HEXsearchforGSMUART_buff((u8 *)">>>",3);
		posid2=HEXsearchforGSMUART_buff((u8 *)"<<<",3);
		posid3=HEXsearchforGSMUART_buff((u8 *)"@@@",3);
		CoypArraryAtoArraryB_LR(GSMUART_buff,updatabitbuf,1,posid1+2);
		CoypArraryAtoArraryB_LR(GSMUART_buff,Vernumbuf,posid3-posid2-3,posid2+2);
		updatabit=GSMatoi((char*)updatabitbuf);	
		Vernum=GSMatoi((char*)Vernumbuf);	
		posid1=HEXsearchforGSMUART_buff((u8 *)"Total\":",7);
		posid2=EndstrsearchGSMUART_buff((u8 *)"}");	
		CoypArraryAtoArraryB_LR(GSMUART_buff,Filetotalbuf,posid2-posid1-7,posid1+6);
		File_Total=GSMatoi((char*)Filetotalbuf);	
		
		#if  Debugpritn 
		printf("updatabit:  %d\r\n",updatabit);
		printf("Vernum:  %d\r\n",Vernum);
		printf("File_Total:  %d\r\n",File_Total);
		#endif
		
		if(updatabit==0)
		{
				FLASH_Unlock();
				MyEE_WriteWord(Vernum_Add,Vernum);
				MyEE_WriteWord(CheckUpdataAdd,updatabit);
				FLASH_Lock();
		}
		else if(updatabit==9)//暗命令复位
		{
				Soft_SystemReset();
		}
		else if(updatabit==8)//暗命令刷Flash的安装状态
		{
		}
		else
		{
				FLASH_Unlock();
				MyEE_WriteWord(Vernum_Add,Vernum);
				MyEE_WriteWord(FileNumAdd,File_Total);
				MyEE_WriteWord(CheckUpdataAdd,updatabit);
				FLASH_Lock();
				#if  Debugpritn 
			    FLASH_Unlock();
				UART_string((u8 *)"FlashVER:",2);
				printf("%d\r\n",MyEE_ReadWord(Vernum_Add));
				UART_string((u8 *)"FlashFileNum:",2);
				printf("%d\r\n",MyEE_ReadWord(FileNumAdd));
				UART_string((u8 *)"FlashAPPaddr:",2);
				printf("%d\r\n",MyEE_ReadWord(CheckUpdataAdd));
				UART_string((u8 *)"FlashFirst:",2);
				printf("%d\r\n",MyEE_ReadWord(FristRunAdd));
			    FLASH_Lock();
				#endif
				Soft_SystemReset();
		}
}

void GSM_Find_upgrade(void)
{
		if(strsearchforGSMUART_buff("Ver")!=0&&strsearchforGSMUART_buff("Total")!=0)
		{
				#if  Debugpritn 
				UART_string_newline((u8 *)"\r\nStart upgrade\r\n",2);
				#endif
				analysis_upgrade();
				#if  Debugpritn 
				UART_string_newline((u8 *)"\r\nEnd upgrade\r\n",2);
				#endif
		}
}

u8 Gsm_init_Status=0x10;//重关闭GSM电源开始
u8 gprs_rex=0;
extern u16 register_flag;
void GSMConnect_INIT(void)
{
		#define INFO_GSM_POWER_OFF  0x10//关闭电源
		#define INFO_GSM_POWER_ON   0x11//打开电源
		#define INFO_CHECK_AT       0x12//检查AT指令是否正常
		#define READ_SIM_STATUS     0x13//检查SIM卡
		#define SET_GSM_CERG        0x14//等待CERG
		#define SET_GSM_PARA        0x15//设置基本配置
		#define GSM_INIT_GPRS       0x16//初始化GPRS
		#define GSM_INIT_FINISH     0x17//初始化成功
		u8 check_AT_error=0,pos=0,error_creg=0;//检测AT通信错误计数
		while(1)
		{
				delay_ms(100);
				switch(Gsm_init_Status)
				{
						case INFO_GSM_POWER_OFF://关闭电源
									#if  Debugpritn 
									UART_string_newline("关闭电源\r\n",2);
									#endif
									GSM_POWER_OFF();Gsm_init_Status=INFO_GSM_POWER_ON;break;
						case INFO_GSM_POWER_ON://打开电源
							        #if  Debugpritn 
									UART_string_newline("打开电源\r\n",2);
									#endif
									GSM_POWER_ON();Gsm_init_Status=INFO_CHECK_AT;error_creg=0;g_gsm_status=GSM_OFFLINE;break;
						case INFO_CHECK_AT://检查AT指令是否正常
							        #if  Debugpritn 
									UART_string_newline("检查AT指令是否正常\r\n",2);
									#endif
									if(check_ATcommand())
									{
											Gsm_init_Status=READ_SIM_STATUS;
											delay_ms(2000);
									}
									else 
											check_AT_error++;//累计错误次数
									if(check_AT_error>=2)//重复检查AT指令3次，若三次都失败，重启GSM电源
									{
											check_AT_error=0;
											Gsm_init_Status=INFO_GSM_POWER_OFF;//重新启动GSM模块
									}
									break;
						case READ_SIM_STATUS://检查SIM卡
									#if  Debugpritn 
									UART_string_newline("检查SIM卡\r\n",2);
									#endif
									if(checkSIMsertStatus()==1)
									{
										    register_flag= SIMOK;
											SimInsertGsmStatus=1;//有卡
											Gsm_init_Status=SET_GSM_CERG;
									}
									else//没卡，一直循环，直至拨电重启
									{
											register_flag= NOSIM;
											SimInsertGsmStatus=0;
											delay_ms(500);
									}
									break;
						case SET_GSM_CERG://等待CERG
							        #if  Debugpritn 
									UART_string_newline("等待CERG\r\n",2);
									#endif
									UART_string_newline("AT+CREG=1",1);
									clear_USART1buff();	
									UART_string_newline("AT+CREG?",1);//获取网络状态
									delay_ms(1000);
									pos=strsearch(GSMUART_buff,"CREG:");
									if(pos)//0:未注册和未搜索 1:已注册 2:未注册，正在搜索 4:未知 5:已注册，处于漫游状态              
									{
											if((GSMUART_buff[pos+7]=='1')||(GSMUART_buff[pos+7]=='5'))
											{g_gsm_status=GSM_ONLINE;error_creg=100;}
											else if(GSMUART_buff[pos+7]=='2')
											{g_gsm_status=GSM_FINDING;delay_ms(1000);}
											else 
											{
													error_creg++;
													g_gsm_status=GSM_OFFLINE;
													delay_ms(1000);
											}//没有注册到网络
									}
									if(error_creg==100)Gsm_init_Status=SET_GSM_PARA;
									else if(error_creg>=10)Gsm_init_Status=INFO_GSM_POWER_OFF;
									break;
						case SET_GSM_PARA://设置基本配置
									#if  Debugpritn 
									UART_string_newline("设置基本配置\r\n",2);
									#endif
									if(set_GSMconfiguration()==0)
											Gsm_init_Status=GSM_INIT_GPRS;
									else
											Gsm_init_Status=INFO_GSM_POWER_OFF;
									break;
						case GSM_INIT_GPRS://初始化GPRS
									#if  Debugpritn 
									UART_string_newline("初始化GPRS\r\n",2);
									#endif							
									gprs_rex=CIPRXGET_Command();
									if(gprs_rex==0)
									   Gsm_init_Status=INFO_GSM_POWER_OFF;
									gprs_rex=0;
									gprs_rex=gsm_gprs_init();
									if(gprs_rex!=0)
									{ 
										#if  Debugpritn 
										if(gprs_rex==1) {UART_string_newline("该SIM卡无法联网\r\n",2);register_flag= NOGPRS;}
										else if(gprs_rex==2) UART_string_newline("写入APN名失败\r\n",2);
										else if(gprs_rex==3) UART_string_newline("APN接入场景失败 \r\n",2);
										#endif	
										Gsm_init_Status=INFO_GSM_POWER_OFF;
									}
									else if(gprs_rex==0)
									{
										register_flag= GPRSOK;
									    Gsm_init_Status=GSM_INIT_FINISH;
									}
									break;
						case GSM_INIT_FINISH:
									#if  Debugpritn 
									UART_string_newline("初始化GPRS成功\r\n",2);
									#endif	
							    delay_ms(200);
						            return;//进入开机前的延时4s,为了以后可以正常连接网络
				}
		}
}

//*************************************************
//描述:检查SIM卡是否插上
//返回: 返回1有卡 0没卡
u8 checkSIMsertStatus(void)
{
		u8 try_cnt=0,try_flag=0;
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				clear_USART1buff();
				UART_string_newline("AT+CSMINS?",1);
				delay_ms(500);
				if(!strsearch(GSMUART_buff,"0,1"))try_flag=0;	
				else
				{
						try_flag=1;
						break;
				}
		}
		return try_flag;
}

//*************************************************
//描述:设置GSM模块配置
//返回: 返回0成功
u8 set_GSMconfiguration(void)
{
		u8 try_cnt=0;
		u8 returnflag=0;
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("ATE0V1",1);//关闭回显
				delay_ms(200);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=1;
				else
					break;
		}		
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("AT+CMGF=0",1);//设置PDU模式,一定要保证SIM卡已插上
				delay_ms(200);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=2;		
				else
					break;
		}
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("AT+CNMI=0,0",1);//设定新sms接收模式(不通知)
				delay_ms(200);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=3;	
				else
					break;
		}
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("AT+CLTS=1",1);//设定可以搜索网络时间
				delay_ms(2000);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=4;	
				else
					break;
		}
		clear_USART1buff();
		return returnflag;
}

//*************************************************
//描述:发送AT到GSM模块,检测AT是否正常
//返回: 返回0失败 1成功
u8 check_ATcommand(void)
{
		clear_USART1buff();
		UART_string_newline("AT",1);
		delay_ms(2000);
		if(strsearch(GSMUART_buff,"OK")!=0)return 1;
		return 0;
}

//描述:获取GSM温度,电压,信号,网络状态
void GSM_BasicData(void)
{
		static u8 cnt=0;
		u16 pos = 0;
		u16 temp = 0;	
		if(cnt%10==0)//每运行10次获取一次数据
		{
				DeleteAllSMS();//删除全部短信
				#if SMS_Function
				clear_USART1buff();
				UART_string_newline("AT+COPS?",1);//查询运营商
				delay_ms(500);
				if(strsearch(GSMUART_buff,"MOBILE"))SimOperator = ChinaMobile;//中国移动
				else if(strsearch(GSMUART_buff,"UNICOM"))SimOperator = ChinaUnicom;//中国联通
				else SimOperator=NoOperator; 
				#endif
				clear_USART1buff(); 
				UART_string_newline("AT+CSQ",1);//获取信号
				delay_ms(500);
				pos=strsearch(GSMUART_buff,"CSQ:");
				if(pos)
				{ 
					 temp=string_cint(GSMUART_buff+pos+4,2);
					 temp=(temp*32)/10;//转为百分比的形式
					 if(temp<=100)SysGsmRssi=temp;
				}
				clear_USART1buff();	
				UART_string_newline("AT+CREG?",1);//获取网络状态
				delay_ms(500);
				pos=strsearch(GSMUART_buff,"CREG:");
				if(pos)               
				{
						if((GSMUART_buff[pos+5]=='1')&&((GSMUART_buff[pos+7]=='1')||(GSMUART_buff[pos+7]=='5')))
								g_gsm_status=GSM_ONLINE;
						else if((GSMUART_buff[pos+5]!='1'))
								UART_string_newline("AT+CREG=1",1);
						else 
								g_gsm_status=GSM_OFFLINE;//没有注册到网络
				}
				clear_USART1buff(); 
		}
		cnt++;
		if(cnt>100)cnt=0;
}

//***********************
//描述:删除所有的卡中的存储的短信
//返回:1:成功 0:失败
u8 DeleteAllSMS(void)
{
		u8 try_cnt=0,returnflag=1;
		clear_USART1buff();
		for(try_cnt=0;try_cnt<10;try_cnt++)
		{
				UART_string_newline("AT+CMGDA=6",1);//删除所有信息
				delay_ms(500);
				if(!strsearch(GSMUART_buff,"OK"))returnflag=0;	
				else	
					break;
		}
		return returnflag;
}

//********************************************
//描述:初始化gprs功能
//函数:0:所有项目通过 1:该SIM卡没有依附gprs功能 2:写入APN名失败   3:APN接入场景失败 
u8 GPRS_Init_State=1;
u8 gsm_gprs_init(void)
{
		#define  ATTACH_GPRS  			  1//判断是否有依附着gprs功能
		#define  WRITE_APN_NAME			  2//写入APN名
		#define  START_CONNECT_GPRS		3//启动通过APN接入GPRS
		u8 res=0,GPRS_INIT_ERROR_CNT=0;
		while(1)
		{
				delay_ms(100);
				switch(GPRS_Init_State)
				{
					case ATTACH_GPRS://判断是否有依附着gprs功能
								res=GetSimAttachGprs();
								if(res)GPRS_Init_State=WRITE_APN_NAME;
								else
								{
										GPRS_INIT_ERROR_CNT++;
										if(GPRS_INIT_ERROR_CNT>3)
										{
												GPRS_INIT_ERROR_CNT=0;
												return 1;
										}
								}
								break;
					case WRITE_APN_NAME://写入APN名
								if(WriteApnName("CMNET"))
								{
										if(WriteApnName("CMNET"))return 2;//第一次写失败，重写
										else
												GPRS_Init_State=START_CONNECT_GPRS;
								}
								else
										GPRS_Init_State=START_CONNECT_GPRS;
								break;
					case START_CONNECT_GPRS://启动通过APN接入GPRS
								if(StartApnGprs())return 3;										
								else 
								{
											if(http_init()==0)return 0;//初始化http											
											else
												return 3;
								} 
				}
		}
}

//************************************************************************
//描述: 判断当前sim卡有没有依附gprs功能
//返回: 1:有依附  0错误
u8 GetSimAttachGprs(void)
{
		UART_string_newline("AT+CGATT=1",1);//写入之前必须shut gprs
		delay_ms(2000);
    clear_USART1buff();
		UART_string_newline("AT+CGATT?",1);//读取依附
    delay_ms(100);
		if(strsearch(GSMUART_buff,"+CGATT"))
		{
				if(strsearch(GSMUART_buff,"0"))	
				{
						clear_USART1buff();
						UART_string_newline("AT+CIPSHUT",1);//写入之前必须shut gprs
						delay_ms(500);
						return 0;
				}
				else if(strsearch(GSMUART_buff,"1"))
				{
						return 1;
				}
				else
						return 0;
		}	
		else
				return 0;//没有响应
}
//************************************
//描述:写入APN接入名称
//参数:name:写入的名称
//返回:0:成功写入 1:失败
u8 WriteApnName(u8 *name)
{
		u16 pos = 0;
    clear_USART1buff();
		UART_string_newline("AT+CIPSHUT",1);//写入之前必须shut gprs
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"OK");   	
		if(!pos)return 1;//不能关闭GPRS系统
    clear_USART1buff();
		UART_string("AT+CSTT=",1);			//写入
		UART_string("\x22",1);
		UART_string(name,1);
		UART_string_newline("\x22,\x22\x22,\x22\x22",1);
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"OK");  	
		if(pos)return 0;
		else return 1;
}

//**************************************************************
//描述:获取目前的GPRS状态
//返回: 255:获取失败 0: 初始化状态	 1:启动任务	 2:配置场景	 	
//返回: 3:接受场景配置  4:	获得本地IP地址	 5:连接中
//返回:6: 连接成功 7 :正在关闭 8:已经关闭 9:场景被释放
u8 GetCurrGprsStatus(void)
{
		u16 pos = 0;
    clear_USART1buff();
		UART_string_newline("AT+CIPSTATUS",1);
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"IP INITIAL");  
		if(pos)return 0;	
    pos=strsearch(GSMUART_buff,"IP START");  
		if(pos)return 1; 
    pos=strsearch(GSMUART_buff,"IP CONFIG");  
		if(pos)return 2;
    pos=strsearch(GSMUART_buff,"IP GPRSACT");  
		if(pos)return 3; 
    pos=strsearch(GSMUART_buff,"IP STATUS");  
		if(pos)return 4;	
    pos=strsearch(GSMUART_buff,"CONNECTING");  
		if(pos)return 5; 
    pos=strsearch(GSMUART_buff,"CONNECT OK");  
		if(pos)return 6; 
    pos=strsearch(GSMUART_buff,"CLOSING");  
		if(pos)return 7; 
    pos=strsearch(GSMUART_buff,"CLOSED");  
		if(pos)return 8; 
		pos=strsearch(GSMUART_buff,"PDP DEACT");  
		if(pos)return 9; 
		return 255;
}

//**********************************************
//描述:通过APN接入GPRS ,执行以后必须确保GPRS处于在网状态
//返回:	 0:接入成功   1:接入失败
u8 StartApnGprs(void)
{
		u16 pos=0;
		u8 res=0;
		u8 times=0;
		startapn:
		while(times < 3)//最多执行三次 APN接入过程,否则放弃
		{
				times++;
				clear_USART1buff();
				res=GetCurrGprsStatus();	
				switch(res)
				{
						case 255://这个是获取状态失败
						case 9:
						default:
										UART_string_newline("AT+CIPSHUT",1);//写入之前必须shut gprs
										delay_ms(1000);
										pos=strsearch(GSMUART_buff,"OK");
										if(!pos)
										{
												goto startapn;				
										}
						case 0:	//最原始的初始化状态，			
										clear_USART1buff();
										UART_string_newline("AT+CSTT",1);//启动APN
										delay_ms(1000);
										pos=strsearch(GSMUART_buff,"OK"); 
										if(!pos)
										{
												goto startapn;				
										}		
						case 1:
										clear_USART1buff();
										UART_string_newline("AT+CIICR",1);//激活场景
										delay_ms(1000);
										pos=strsearch(GSMUART_buff,"OK");
										if(!pos)
										{
												goto startapn;				
										}
						case 2://什么都不需要做
						case 3:	
						case 4:  
						case 8:			
										UART_string_newline("AT+CIFSR",1);//获取本地IP
										delay_ms(1000);			
										goto leave;
						case 5:
						case 6:
										UART_string_newline("AT+CIPCLOSE",1);//close tcp connect
										delay_ms(1000); 
										pos=strsearch(GSMUART_buff,"OK");
										if(!pos)
										{
												goto startapn;				
										}
										break;
				}
		}
		leave:
		if(IsGetDnsIp())return 1;
		else	return 0;
}

//*************************
//描述: 判断是否成功获取DNS地址
//返回:	0:成功  1:获取失败
u8 IsGetDnsIp(void)
{
		u16 pos = 0;
    clear_USART1buff();
		UART_string_newline("AT+CDNSCFG?",1);			//获取DNS地址
		delay_ms(1000);
		pos=strsearch(GSMUART_buff,"PrimaryDns:");
		if(pos)
		{
				pos=strsearch(GSMUART_buff,"PrimaryDns: 0.0.0.0");
				if(pos)return 1;	
				else	return 0;
		}
		else return 1;	
}

//**************************************
//描述：写入APN接入名称  Bearer http使用
//参数: name:写入的名称
//返回:	0:成功写入   1:失败
u8 WriteApnName22(u8 *name)
{
		u16 pos=0;
    clear_USART1buff();
		UART_string("AT+SAPBR=3,1,\"APN\",\"",1);//写入	AT+SAPBR=3,1,"APN","CMNET"
		UART_string(name,1);
		UART_string_newline("\"",1);
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"OK");  	
		if(pos)return 0;
		else	return 1;
}

//**********************************************
//描述:检查FALSH中的CRC是否和序列号计算CRC出来的值一致，如果不一致，死循环
void Check_CDKEY(void)
{
		u8 poscd1=0,poscd2=0,plength=0,pbuffer[32]={0};
		u16 pcdkey_crc=0,check_cdkkey_crc=0;
		clear_USART1buff();
		UART_string_newline("AT+CGSN",1);//查询序列号
		delay_ms(50);
		if(GSMUART_buff[0]==0x0d&&GSMUART_buff[1]==0x0a)poscd1=3;
		poscd2=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0aOK\x0d\x0a");
		if(poscd1!=0&&poscd2!=0)
		{	
				CoypArraryAtoArraryB_LR(GSMUART_buff,pbuffer,poscd2-poscd1,poscd1-1);//复制部分数组
				plength=poscd2-poscd1;
				pcdkey_crc=get_crc16_value(pbuffer,plength);//计算CRC
				check_cdkkey_crc=MyEE_ReadWord(CDKEY_CRC16Add);//获取FLASH的CRC
				if(pcdkey_crc!=check_cdkkey_crc)//判断是否一样
				{
						GPRS_FALUT_STATUS=1;//GPRS初始化失败
						while(1);//死循环
				}
		}
}


void TCPConnect(char* IP_Add,char* Port_Add)
{
		clear_USART1buff();
		UART_string_newline("AT+CIPCLOSE=1",1);//一定要确保TCP已经断开
		delay_ms(100);
		clear_USART1buff();
		UART_string_newline("AT+CGDCONT=1,\"IP\",\"CMNET\"",1);
		delay_ms(100);
		clear_USART1buff();
		UART_string_newline("AT+CGATT=1",1);
		delay_ms(100);
		clear_USART1buff();
		UART_string_newline("AT+CIPCSGP=1,\"CMNET\"",1);
		delay_ms(100);
		clear_USART1buff();
		UART_string_newline("AT+CIPHEAD=1",1);
		delay_ms(100);
		clear_USART1buff();
		UART_string("AT+CIPSTART=\"TCP\",\"",1);
		UART_string((unsigned char*)IP_Add,1);
		UART_string("\",\"",1);
		UART_string((unsigned char*)Port_Add,1);
		UART_string_newline("\"",1);
        //UART_string_newline("AT+CIPSTART=\"TCP\",\"47.106.255.185\",\"1899\"",1);
}


u8 Find_StringforGSMUART_buff(char* STRING_BUFFER,u16 cnt,u8 mode)
{
		u16 i=0;
		while(1)
		{
				delay_ms(100);
				if(strsearchforGSMUART_buff((unsigned char*)STRING_BUFFER)!=0)
				{
						delay_ms(100);
						return 1;
				}					
				i++;
				if(i>=cnt&&mode==1)
						Soft_SystemReset();
				else if(i>=cnt&&mode==0)
				{
						return 0;
				}
		}
}

//*************************************************
//描述:从GSMUART_buff中查找p2字符串位置
//参数:字符串p2
//返回:返回位置pos
u16 strsearchforGSMUART_buff(const u8 *p2)
{
		u16 p1_len=0,p2_len=0;
		u16 i=0,j=0,k=0;
		u16 pos=0;
		if(!*p2)return 0;
		p1_len=MAX_GSMUART;
		p2_len=strlen((char const *)p2);
		for(i=0;i<p1_len;i++)
		{
			if(GSMUART_buff[i]==*(p2))
			{
				for(k=i,j=0;j<p2_len;k++,j++)
				{
					if(GSMUART_buff[k]==*(p2+j))
					{
						pos=i+1;
						if(j==p2_len-1)return pos;
					}
					else{pos = 0;break;}
				}
			} 
		}
		return pos;
}

void UART1_BUF_SEND(char * _data,u16 lenth)
{
	u16 i;
	for(i=0;i<lenth;i++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART1,_data[i]); 
	} 
}
void UART2_BUF_SEND(char * _data,u16 lenth)
{
	u16 i;
	for(i=0;i<lenth;i++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,_data[i]); 
	} 
}

//*************************************************
//描述:设置GSM模块的TCPIP为手动模块,必须在GPRS初始化前完成设置
//返回: 返回0失败 1成功
u8 CIPRXGET_Command(void)
{
		clear_USART1buff();
		UART_string_newline("AT+CIPRXGET=1",1);
		delay_ms(100);
		clear_USART1buff();
		UART_string_newline("AT+CIPRXGET?",1);
		delay_ms(100);
		if(strsearch(GSMUART_buff,"+CIPRXGET: 1")!=0)
				return 1;
		else
				return 0;
}

u8 GetIMEIofSIM800(void)
{
	u16 poscd1=0,poscd2=0;
	u16 IMEIlen=0;

	clear_USART1buff();
	UART_string_newline("AT+GSN",1);//查询序列号
	delay_ms(50);
	if(GSMUART_buff[0]==0x0d&&GSMUART_buff[1]==0x0a) poscd1=3;
	poscd2=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0aOK\x0d\x0a");
	if(poscd1!=0&&poscd2!=0)
	{	
		CoypArraryAtoArraryB_LR(GSMUART_buff,IMEIbuff,poscd2-poscd1,poscd1-1);//将序列号复制到cdkeybuffer
		IMEIlen=poscd2-poscd1; 
		register_flag=IMEIFLAG;
		#if Debugpritn
		UART_string("设备唯一IMEI号：",2);
	    UART_string_newline(IMEIbuff,2);
	    #endif
		return 1;
	}
	else return 0;
}

u8 GetNetworkTimeFromSim(void)
{

	clear_USART1buff();
	UART_string_newline("AT+CCLK?",1);
	delay_ms(100);	
	#if Debugpritn
	UART_string("SIM NetworkTime：",2);
	UART_string_newline(GSMUART_buff,2);
	#endif

}
	
u8 _sendIMEItoIP(u8 *IMEI)
{
	u8 timebuff[100]={0};
	u8 cnt=0;
	char imeitempbuff[40]={0};
	char ATsendbuff[20]={0};
	char ATtimebuff[40]={0};
	u8 TimebuffLen=0;
	u8 SuccessFlag=0;
	u8 ConnectStatua=0;
	u16 timepos=0,timecnt=0;
	u8 Timetempbuff[30]={0};
	for(cnt=0;cnt++;cnt<40) imeitempbuff[cnt]=0;
	for(cnt=0;cnt++;cnt<100) timebuff[cnt]=0;
	for(cnt=0;cnt++;cnt<20) ATsendbuff[cnt]=0;
	strcat(timebuff,"{imei:\"");
	sprintf(imeitempbuff,"%s",IMEI);
	strcat(timebuff,imeitempbuff);
	strcat(timebuff,"\",cmd:\"gettime\"}");
    TimebuffLen=strlen(timebuff);    
	clear_USART1buff();
	UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
	delay_ms(500); 
	if(!strsearch(GSMUART_buff,"0.0.0.0"))
	  {	
			sprintf(ATsendbuff,"AT+CIPSEND=%d",TimebuffLen);
			UART_string_newline(ATsendbuff,1);
			delay_ms(500);
			clear_USART1buff();
			UART1_BUF_SEND(timebuff,TimebuffLen);
			#if  Debugpritn 
			UART2_BUF_SEND(timebuff,TimebuffLen);
			UART_string_newline(" ",2);
			#endif
			clear_Packbuff();
			clear_Pdatabuff();
			SuccessFlag=Find_StringforGSMUART_buff("SEND OK",50,0);
			if(SuccessFlag)
			{
				clear_USART1buff();
				UART_string_newline("AT+CIPRXGET=2,1000",1);
				ConnectStatua=Find_StringforGSMUART_buff("OK",10,0);
				if(ConnectStatua) 
				{    											
					timepos=strsearch(GSMUART_buff,"time:");
					if(timepos)
					{
						#if  Debugpritn 
						UART_string_newline((u8 *)"网络时间数据接收服务器返回成功\r\n",2);
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
				return 0;
		}
		else
			return 0;	
	
}

u8 _sendIMEIandgetTime(u8 *IMEI)
{
	
	u8 timebuff[100]={0};
	u8 cnt=0;
	char imeitempbuff[40]={0};
	char ATsendbuff[20]={0};
	char ATtimebuff[40]={0};
	u8 TimebuffLen=0;
	u8 SuccessFlag=0;
	u8 ConnectStatua=0;
	u16 timepos=0,timecnt=0;
	u8 Timetempbuff[30]={0};
	for(cnt=0;cnt++;cnt<40) imeitempbuff[cnt]=0;
	for(cnt=0;cnt++;cnt<100) timebuff[cnt]=0;
	for(cnt=0;cnt++;cnt<20) ATsendbuff[cnt]=0;
	strcat(timebuff,"{imei:\"");
	sprintf(imeitempbuff,"%s",IMEI);
	strcat(timebuff,imeitempbuff);
	strcat(timebuff,"\",cmd:\"gettime\"}");
    TimebuffLen=strlen(timebuff);    
	clear_USART1buff();
	UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
	delay_ms(500); 
	if(!strsearch(GSMUART_buff,"0.0.0.0"))
	  {	
			sprintf(ATsendbuff,"AT+CIPSEND=%d",TimebuffLen);
			UART_string_newline(ATsendbuff,1);
			delay_ms(500);
			clear_USART1buff();
			UART1_BUF_SEND(timebuff,TimebuffLen);
			#if  Debugpritn 
			UART2_BUF_SEND(timebuff,TimebuffLen);
			UART_string_newline(" ",2);
			#endif
			clear_Packbuff();
			clear_Pdatabuff();
			SuccessFlag=Find_StringforGSMUART_buff("SEND OK",50,0);
			if(SuccessFlag)
			{
				clear_USART1buff();
				UART_string_newline("AT+CIPRXGET=2,1000",1);
				ConnectStatua=Find_StringforGSMUART_buff("OK",10,0);
				if(ConnectStatua) 
				{    											
					timepos=strsearch(GSMUART_buff,"time:");
					if(timepos)
					{
						#if  Debugpritn 
						UART_string_newline((u8 *)"网络时间数据接收服务器返回成功\r\n",2);
						#endif
						//处理返回时间，写进去SIM RTC
						for(timecnt=0;timecnt<30;timecnt++) Timetempbuff[timecnt]=0;
						for(timecnt=0;timecnt<30;timecnt++) TIMEbufftoSim[timecnt]=0;
						CoypArraryAtoArraryB_LR(GSMUART_buff,Timetempbuff,timepos+19,timepos+4);						
						TimechargetoWrite(TIMEbufftoSim,Timetempbuff);
						clear_USART1buff();
						sprintf(ATtimebuff,"AT+CCLK=%s",TIMEbufftoSim);
						UART_string_newline(ATtimebuff,1);
						if(Find_StringforGSMUART_buff("OK",20,0))
						{
							#if  Debugpritn 
							UART_string_newline((u8 *)"网络时间写入SIM成功\r\n",2);
							#endif
						}
						else 
						{
							#if  Debugpritn 
							UART_string_newline((u8 *)"网络时间写入SIM失败\r\n",2);
							#endif
							return 0;
						}
						Simtimechargetostr(TIMEbuff);
						#if  Debugpritn 
						UART_string("Receivetime:",2);
						UART_string_newline(Timetempbuff,2);
						UART_string("Simtimetowrtie:",2);
						UART_string_newline(TIMEbufftoSim,2);
						UART_string("ReadtimefromSIM:",2);
						UART_string_newline(TIMEbuff,2);
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
				return 0;
		}
		else
			return 0;		
	
}


u8 _PsendIMEIandgetTime(u8 *IMEI)
{
	
	u8 timebuff[100]={0};
	char IMEIbuff[20]={0};
	char ATsendbuff[20]={0};
	char ATtimebuff[40]={0};
	u8 TimebuffLen=0;
	u8 SuccessFlag=0;
	u8 ConnectStatua=0;
	u16 timepos=0,timecnt=0;
	u8 Timetempbuff[30]={0};
	strcat(timebuff,"{imei:\"");
	sprintf(IMEIbuff,"%s",IMEI);
	strcat(timebuff,IMEIbuff);
	strcat(timebuff,"\",cmd:\"gettime\"}");
    TimebuffLen=strlen(timebuff);    
	clear_USART1buff();
	UART_string_newline("AT+SAPBR=2,1",1);//查询GPRS上下文是否正常
	delay_ms(500); 
	if(!strsearch(GSMUART_buff,"0.0.0.0"))
	  {	
			sprintf(ATsendbuff,"AT+CIPSEND=%d",TimebuffLen);
			UART_string_newline(ATsendbuff,1);
			delay_ms(500);
			clear_USART1buff();
			UART1_BUF_SEND(timebuff,TimebuffLen);
			#if  Debugpritn 
			UART2_BUF_SEND(timebuff,TimebuffLen);
			#endif
			clear_Packbuff();
			clear_Pdatabuff();
			SuccessFlag=Find_StringforGSMUART_buff("SEND OK",50,0);
			if(SuccessFlag)
			{
				#if  Debugpritn 
				UART_string_newline((u8 *)"网络时间数据发送成功\r\n",2);
				#endif
				clear_USART1buff();
				UART_string_newline("AT+CIPRXGET=2,1000",1);
				ConnectStatua=Find_StringforGSMUART_buff("OK",10,0);
        if(ConnectStatua) 
				{    											
					timepos=strsearch(GSMUART_buff,"time:");
					if(timepos)
					{
						#if  Debugpritn 
						UART_string_newline((u8 *)"网络时间数据接收服务器返回成功\r\n",2);
						#endif
						//处理返回时间，写进去SIM RTC
						for(timecnt=0;timecnt<30;timecnt++) Timetempbuff[timecnt]=0;
						for(timecnt=0;timecnt<30;timecnt++) TIMEbufftoSim[timecnt]=0;
						CoypArraryAtoArraryB_LR(GSMUART_buff,Timetempbuff,timepos+19,timepos+4);						
						TimechargetoWrite(TIMEbufftoSim,Timetempbuff);
						clear_USART1buff();
						sprintf(ATtimebuff,"AT+CCLK=%s",TIMEbufftoSim);
						UART_string_newline(ATtimebuff,1);
						if(Find_StringforGSMUART_buff("OK",20,0))
						{
							#if  Debugpritn 
							UART_string_newline((u8 *)"网络时间写入SIM成功\r\n",2);
							#endif
						}
						else 
						{
							#if  Debugpritn 
							UART_string_newline((u8 *)"网络时间写入SIM失败\r\n",2);
							#endif
							return 0;
						}
						Simtimechargetostr(TIMEbuff);
						#if  Debugpritn 
						UART_string("Receivetime:",2);
						UART_string_newline(Timetempbuff,2);
						UART_string("Simtimetowrtie:",2);
						UART_string_newline(TIMEbufftoSim,2);
						UART_string("ReadtimefromSIM:",2);
						UART_string_newline(TIMEbuff,2);
						#endif	
						return 1;
					}
					else 
					{
						return 0;
					}
					
				}
				else return 0;
			}
			else
				return 0;
		}
		else
			return 0;		
	
}

u8 TimechargetoWrite(char *timenew,char *timeold)
{
	strcat(timenew,"\"");
	*(timenew+1) = *(timeold+2);
	*(timenew+2) = *(timeold+3);
	strcat(timenew,"/");
	*(timenew+4) = *(timeold+5);
	*(timenew+5) = *(timeold+6);
	strcat(timenew,"/");
	*(timenew+7) = *(timeold+8);
	*(timenew+8) = *(timeold+9);	
	strcat(timenew,",");
	*(timenew+10) = *(timeold+11);	
	*(timenew+11) = *(timeold+12);	
	strcat(timenew,":");
	*(timenew+13) = *(timeold+14);	
	*(timenew+14) = *(timeold+15);
	strcat(timenew,":");
	*(timenew+16) = *(timeold+17);	
	*(timenew+17) = *(timeold+18);
	strcat(timenew,"+08");
	strcat(timenew,"\"");
}

u8 Simtimechargetostr(u8 *strtime)
{
	u8 pos=0,j=0;
	u8 Timetempbuff[25]={0};
	for(j=0;j<25;j++) TIMEbuff[j]=0;
	UART_string_newline("AT+CCLK?",1);
	delay_ms(100);
	pos=strsearch(GSMUART_buff,"+CCLK:");
	if(pos)
	{
		CoypArraryAtoArraryB_LR(GSMUART_buff,Timetempbuff,pos+21,pos+5);
		strcat(strtime,"20");
		*(strtime+2) = *(Timetempbuff+2);//yy
		*(strtime+3) = *(Timetempbuff+3);
		*(strtime+4) = *(Timetempbuff+5);//mm
		*(strtime+5) = *(Timetempbuff+6);
		*(strtime+6) = *(Timetempbuff+8);//dd
		*(strtime+7) = *(Timetempbuff+9);
		*(strtime+8) = *(Timetempbuff+11);//hh
		*(strtime+9) = *(Timetempbuff+12);	
		*(strtime+10) = *(Timetempbuff+14);//mm
		*(strtime+11) = *(Timetempbuff+15);	
		*(strtime+12) = *(Timetempbuff+17);//ss
		*(strtime+13) = *(Timetempbuff+18);		
		Netwroktime.Year    = 2000 + 10*(*(strtime+2) - '0') + *(strtime+3) - '0';
		Netwroktime.Month   = 10*(*(strtime+4) - '0') + *(strtime+5) - '0';
		Netwroktime.Day     = 10*(*(strtime+6) - '0') + *(strtime+7) - '0';
		Netwroktime.Hour    = 10*(*(strtime+8) - '0') + *(strtime+9) - '0';
		Netwroktime.Minute  = 10*(*(strtime+10) - '0') + *(strtime+11) - '0';
		Netwroktime.Second  = 10*(*(strtime+12) - '0') + *(strtime+13) - '0';
		clear_USART1buff();
		#if  Debugpritn 
		 UART_string("*****Timenow:",2);
		 UART_string(U16IntToStr(Netwroktime.Year ),2);
		 UART_string("/",2);
		 UART_string(U16IntToStr(Netwroktime.Month ),2);
		 UART_string("/",2);
		 UART_string(U16IntToStr(Netwroktime.Day ),2);
		 UART_string(",",2);
		 UART_string(U16IntToStr(Netwroktime.Hour ),2);
		 UART_string(":",2);
		 UART_string(U16IntToStr(Netwroktime.Minute ),2);
		 UART_string(":",2);
		 UART_string(U16IntToStr(Netwroktime.Second ),2);
		 UART_string_newline(".",2);
		 //UART_string_newline(TIMEbuff,2);
		 //UART_string_newline((u8 *)"读取SIM时间成功\r\n",2);
		clear_USART1buff();
		#endif
		return 1;
	}
    else 
	{
		clear_USART1buff();
		#if  Debugpritn 
		UART_string_newline((u8 *)"读取SIM时间失败\r\n",2);
		#endif
		return 0;
	}		
	
}


u8 Return_Signal(void)
{
		u16 Signal_Buf=0,pos=0;
	  u8  GsmSignal=0;
		clear_USART1buff();
		UART_string_newline("AT+CSQ",1);
		delay_ms(500);
		pos=strsearch(GSMUART_buff,"CSQ:");
		if(pos)
		{ 
				Signal_Buf=string_cint(GSMUART_buff+pos+4,2);
				Signal_Buf=(Signal_Buf*32)/10;//转为百分比的形式
				if(Signal_Buf<=100)GsmSignal=Signal_Buf;
		}
		return GsmSignal;
}




