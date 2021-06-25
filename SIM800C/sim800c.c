#include "sim800c.h"
#include "pc.h"

extern u8 GPRS_FALUT_STATUS;
extern u8 LCD_CHANGE_NEED;
u8 SimOperator=0;//SIM������Ӫ��,0:û�м�⵽ 1:�ƶ� 2:��ͨ 
u8 SimInsertGsmStatus=0;//SIM���Ƿ����,(gsmģ�鷵�ص�״̬)0:û�� 1:��
u8 g_gsm_status=0;//GSM ״̬
u8 SysGsmTemp=0;//ϵͳ��gsmģ����¶�
u16 SysGsmVoltage=0;//ϵͳ��gsmģ��ĵ�ѹ
u16 SysGsmRssi=0;//ϵͳ��gsm��ǰ���ź�ǿ��,�����ǰٷֱ�
u8 GSMUART_buff[MAX_GSMUART];//GSM���ڻ�������
u16 GSMUART_cnt=0;//GSM���ڽ����ֽڵĸ���
u8 GSMUART_flag=0;//GSM����״̬
//Coneo
u8 IMEIbuff[18]={0};//IMEI
u8 TIMEbuff[25]={0};
u8 TIMEbufftoSim[30]={0};
NETWROKTIME  Netwroktime;
//*************************************************
//����:���GSM�������黺��
void clear_USART1buff(void)
{
		u16 i=0;
		for(i=0;i<MAX_GSMUART;i++)GSMUART_buff[i]=0;
		GSMUART_cnt=0;
		GSMUART_flag=0;
}

//*************************************************
//����:��p1�в���p2�ַ���λ��
//����:�ַ���p1,p2
//ע��:p1�в��ܿո�0x00 
//����:����λ��pos
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
//����:��GSMUART_buff�в���p2�ַ���λ��
//����:�ַ���p2
//����:����λ��pos
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
		else if(updatabit==9)//�����λ
		{
				Soft_SystemReset();
		}
		else if(updatabit==8)//������ˢFlash�İ�װ״̬
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

u8 Gsm_init_Status=0x10;//�عر�GSM��Դ��ʼ
u8 gprs_rex=0;
extern u16 register_flag;
void GSMConnect_INIT(void)
{
		#define INFO_GSM_POWER_OFF  0x10//�رյ�Դ
		#define INFO_GSM_POWER_ON   0x11//�򿪵�Դ
		#define INFO_CHECK_AT       0x12//���ATָ���Ƿ�����
		#define READ_SIM_STATUS     0x13//���SIM��
		#define SET_GSM_CERG        0x14//�ȴ�CERG
		#define SET_GSM_PARA        0x15//���û�������
		#define GSM_INIT_GPRS       0x16//��ʼ��GPRS
		#define GSM_INIT_FINISH     0x17//��ʼ���ɹ�
		u8 check_AT_error=0,pos=0,error_creg=0;//���ATͨ�Ŵ������
		while(1)
		{
				delay_ms(100);
				switch(Gsm_init_Status)
				{
						case INFO_GSM_POWER_OFF://�رյ�Դ
									#if  Debugpritn 
									UART_string_newline("�رյ�Դ\r\n",2);
									#endif
									GSM_POWER_OFF();Gsm_init_Status=INFO_GSM_POWER_ON;break;
						case INFO_GSM_POWER_ON://�򿪵�Դ
							        #if  Debugpritn 
									UART_string_newline("�򿪵�Դ\r\n",2);
									#endif
									GSM_POWER_ON();Gsm_init_Status=INFO_CHECK_AT;error_creg=0;g_gsm_status=GSM_OFFLINE;break;
						case INFO_CHECK_AT://���ATָ���Ƿ�����
							        #if  Debugpritn 
									UART_string_newline("���ATָ���Ƿ�����\r\n",2);
									#endif
									if(check_ATcommand())
									{
											Gsm_init_Status=READ_SIM_STATUS;
											delay_ms(2000);
									}
									else 
											check_AT_error++;//�ۼƴ������
									if(check_AT_error>=2)//�ظ����ATָ��3�Σ������ζ�ʧ�ܣ�����GSM��Դ
									{
											check_AT_error=0;
											Gsm_init_Status=INFO_GSM_POWER_OFF;//��������GSMģ��
									}
									break;
						case READ_SIM_STATUS://���SIM��
									#if  Debugpritn 
									UART_string_newline("���SIM��\r\n",2);
									#endif
									if(checkSIMsertStatus()==1)
									{
										    register_flag= SIMOK;
											SimInsertGsmStatus=1;//�п�
											Gsm_init_Status=SET_GSM_CERG;
									}
									else//û����һֱѭ����ֱ����������
									{
											register_flag= NOSIM;
											SimInsertGsmStatus=0;
											delay_ms(500);
									}
									break;
						case SET_GSM_CERG://�ȴ�CERG
							        #if  Debugpritn 
									UART_string_newline("�ȴ�CERG\r\n",2);
									#endif
									UART_string_newline("AT+CREG=1",1);
									clear_USART1buff();	
									UART_string_newline("AT+CREG?",1);//��ȡ����״̬
									delay_ms(1000);
									pos=strsearch(GSMUART_buff,"CREG:");
									if(pos)//0:δע���δ���� 1:��ע�� 2:δע�ᣬ�������� 4:δ֪ 5:��ע�ᣬ��������״̬              
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
											}//û��ע�ᵽ����
									}
									if(error_creg==100)Gsm_init_Status=SET_GSM_PARA;
									else if(error_creg>=10)Gsm_init_Status=INFO_GSM_POWER_OFF;
									break;
						case SET_GSM_PARA://���û�������
									#if  Debugpritn 
									UART_string_newline("���û�������\r\n",2);
									#endif
									if(set_GSMconfiguration()==0)
											Gsm_init_Status=GSM_INIT_GPRS;
									else
											Gsm_init_Status=INFO_GSM_POWER_OFF;
									break;
						case GSM_INIT_GPRS://��ʼ��GPRS
									#if  Debugpritn 
									UART_string_newline("��ʼ��GPRS\r\n",2);
									#endif							
									gprs_rex=CIPRXGET_Command();
									if(gprs_rex==0)
									   Gsm_init_Status=INFO_GSM_POWER_OFF;
									gprs_rex=0;
									gprs_rex=gsm_gprs_init();
									if(gprs_rex!=0)
									{ 
										#if  Debugpritn 
										if(gprs_rex==1) {UART_string_newline("��SIM���޷�����\r\n",2);register_flag= NOGPRS;}
										else if(gprs_rex==2) UART_string_newline("д��APN��ʧ��\r\n",2);
										else if(gprs_rex==3) UART_string_newline("APN���볡��ʧ�� \r\n",2);
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
									UART_string_newline("��ʼ��GPRS�ɹ�\r\n",2);
									#endif	
							    delay_ms(200);
						            return;//���뿪��ǰ����ʱ4s,Ϊ���Ժ����������������
				}
		}
}

//*************************************************
//����:���SIM���Ƿ����
//����: ����1�п� 0û��
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
//����:����GSMģ������
//����: ����0�ɹ�
u8 set_GSMconfiguration(void)
{
		u8 try_cnt=0;
		u8 returnflag=0;
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("ATE0V1",1);//�رջ���
				delay_ms(200);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=1;
				else
					break;
		}		
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("AT+CMGF=0",1);//����PDUģʽ,һ��Ҫ��֤SIM���Ѳ���
				delay_ms(200);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=2;		
				else
					break;
		}
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("AT+CNMI=0,0",1);//�趨��sms����ģʽ(��֪ͨ)
				delay_ms(200);
				if(!strsearch(GSMUART_buff,"OK"))
					returnflag=3;	
				else
					break;
		}
		clear_USART1buff();
		for(try_cnt=0;try_cnt<3;try_cnt++)
		{
				UART_string_newline("AT+CLTS=1",1);//�趨������������ʱ��
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
//����:����AT��GSMģ��,���AT�Ƿ�����
//����: ����0ʧ�� 1�ɹ�
u8 check_ATcommand(void)
{
		clear_USART1buff();
		UART_string_newline("AT",1);
		delay_ms(2000);
		if(strsearch(GSMUART_buff,"OK")!=0)return 1;
		return 0;
}

//����:��ȡGSM�¶�,��ѹ,�ź�,����״̬
void GSM_BasicData(void)
{
		static u8 cnt=0;
		u16 pos = 0;
		u16 temp = 0;	
		if(cnt%10==0)//ÿ����10�λ�ȡһ������
		{
				DeleteAllSMS();//ɾ��ȫ������
				#if SMS_Function
				clear_USART1buff();
				UART_string_newline("AT+COPS?",1);//��ѯ��Ӫ��
				delay_ms(500);
				if(strsearch(GSMUART_buff,"MOBILE"))SimOperator = ChinaMobile;//�й��ƶ�
				else if(strsearch(GSMUART_buff,"UNICOM"))SimOperator = ChinaUnicom;//�й���ͨ
				else SimOperator=NoOperator; 
				#endif
				clear_USART1buff(); 
				UART_string_newline("AT+CSQ",1);//��ȡ�ź�
				delay_ms(500);
				pos=strsearch(GSMUART_buff,"CSQ:");
				if(pos)
				{ 
					 temp=string_cint(GSMUART_buff+pos+4,2);
					 temp=(temp*32)/10;//תΪ�ٷֱȵ���ʽ
					 if(temp<=100)SysGsmRssi=temp;
				}
				clear_USART1buff();	
				UART_string_newline("AT+CREG?",1);//��ȡ����״̬
				delay_ms(500);
				pos=strsearch(GSMUART_buff,"CREG:");
				if(pos)               
				{
						if((GSMUART_buff[pos+5]=='1')&&((GSMUART_buff[pos+7]=='1')||(GSMUART_buff[pos+7]=='5')))
								g_gsm_status=GSM_ONLINE;
						else if((GSMUART_buff[pos+5]!='1'))
								UART_string_newline("AT+CREG=1",1);
						else 
								g_gsm_status=GSM_OFFLINE;//û��ע�ᵽ����
				}
				clear_USART1buff(); 
		}
		cnt++;
		if(cnt>100)cnt=0;
}

//***********************
//����:ɾ�����еĿ��еĴ洢�Ķ���
//����:1:�ɹ� 0:ʧ��
u8 DeleteAllSMS(void)
{
		u8 try_cnt=0,returnflag=1;
		clear_USART1buff();
		for(try_cnt=0;try_cnt<10;try_cnt++)
		{
				UART_string_newline("AT+CMGDA=6",1);//ɾ��������Ϣ
				delay_ms(500);
				if(!strsearch(GSMUART_buff,"OK"))returnflag=0;	
				else	
					break;
		}
		return returnflag;
}

//********************************************
//����:��ʼ��gprs����
//����:0:������Ŀͨ�� 1:��SIM��û������gprs���� 2:д��APN��ʧ��   3:APN���볡��ʧ�� 
u8 GPRS_Init_State=1;
u8 gsm_gprs_init(void)
{
		#define  ATTACH_GPRS  			  1//�ж��Ƿ���������gprs����
		#define  WRITE_APN_NAME			  2//д��APN��
		#define  START_CONNECT_GPRS		3//����ͨ��APN����GPRS
		u8 res=0,GPRS_INIT_ERROR_CNT=0;
		while(1)
		{
				delay_ms(100);
				switch(GPRS_Init_State)
				{
					case ATTACH_GPRS://�ж��Ƿ���������gprs����
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
					case WRITE_APN_NAME://д��APN��
								if(WriteApnName("CMNET"))
								{
										if(WriteApnName("CMNET"))return 2;//��һ��дʧ�ܣ���д
										else
												GPRS_Init_State=START_CONNECT_GPRS;
								}
								else
										GPRS_Init_State=START_CONNECT_GPRS;
								break;
					case START_CONNECT_GPRS://����ͨ��APN����GPRS
								if(StartApnGprs())return 3;										
								else 
								{
											if(http_init()==0)return 0;//��ʼ��http											
											else
												return 3;
								} 
				}
		}
}

//************************************************************************
//����: �жϵ�ǰsim����û������gprs����
//����: 1:������  0����
u8 GetSimAttachGprs(void)
{
		UART_string_newline("AT+CGATT=1",1);//д��֮ǰ����shut gprs
		delay_ms(2000);
    clear_USART1buff();
		UART_string_newline("AT+CGATT?",1);//��ȡ����
    delay_ms(100);
		if(strsearch(GSMUART_buff,"+CGATT"))
		{
				if(strsearch(GSMUART_buff,"0"))	
				{
						clear_USART1buff();
						UART_string_newline("AT+CIPSHUT",1);//д��֮ǰ����shut gprs
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
				return 0;//û����Ӧ
}
//************************************
//����:д��APN��������
//����:name:д�������
//����:0:�ɹ�д�� 1:ʧ��
u8 WriteApnName(u8 *name)
{
		u16 pos = 0;
    clear_USART1buff();
		UART_string_newline("AT+CIPSHUT",1);//д��֮ǰ����shut gprs
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"OK");   	
		if(!pos)return 1;//���ܹر�GPRSϵͳ
    clear_USART1buff();
		UART_string("AT+CSTT=",1);			//д��
		UART_string("\x22",1);
		UART_string(name,1);
		UART_string_newline("\x22,\x22\x22,\x22\x22",1);
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"OK");  	
		if(pos)return 0;
		else return 1;
}

//**************************************************************
//����:��ȡĿǰ��GPRS״̬
//����: 255:��ȡʧ�� 0: ��ʼ��״̬	 1:��������	 2:���ó���	 	
//����: 3:���ܳ�������  4:	��ñ���IP��ַ	 5:������
//����:6: ���ӳɹ� 7 :���ڹر� 8:�Ѿ��ر� 9:�������ͷ�
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
//����:ͨ��APN����GPRS ,ִ���Ժ����ȷ��GPRS��������״̬
//����:	 0:����ɹ�   1:����ʧ��
u8 StartApnGprs(void)
{
		u16 pos=0;
		u8 res=0;
		u8 times=0;
		startapn:
		while(times < 3)//���ִ������ APN�������,�������
		{
				times++;
				clear_USART1buff();
				res=GetCurrGprsStatus();	
				switch(res)
				{
						case 255://����ǻ�ȡ״̬ʧ��
						case 9:
						default:
										UART_string_newline("AT+CIPSHUT",1);//д��֮ǰ����shut gprs
										delay_ms(1000);
										pos=strsearch(GSMUART_buff,"OK");
										if(!pos)
										{
												goto startapn;				
										}
						case 0:	//��ԭʼ�ĳ�ʼ��״̬��			
										clear_USART1buff();
										UART_string_newline("AT+CSTT",1);//����APN
										delay_ms(1000);
										pos=strsearch(GSMUART_buff,"OK"); 
										if(!pos)
										{
												goto startapn;				
										}		
						case 1:
										clear_USART1buff();
										UART_string_newline("AT+CIICR",1);//�����
										delay_ms(1000);
										pos=strsearch(GSMUART_buff,"OK");
										if(!pos)
										{
												goto startapn;				
										}
						case 2://ʲô������Ҫ��
						case 3:	
						case 4:  
						case 8:			
										UART_string_newline("AT+CIFSR",1);//��ȡ����IP
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
//����: �ж��Ƿ�ɹ���ȡDNS��ַ
//����:	0:�ɹ�  1:��ȡʧ��
u8 IsGetDnsIp(void)
{
		u16 pos = 0;
    clear_USART1buff();
		UART_string_newline("AT+CDNSCFG?",1);			//��ȡDNS��ַ
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
//������д��APN��������  Bearer httpʹ��
//����: name:д�������
//����:	0:�ɹ�д��   1:ʧ��
u8 WriteApnName22(u8 *name)
{
		u16 pos=0;
    clear_USART1buff();
		UART_string("AT+SAPBR=3,1,\"APN\",\"",1);//д��	AT+SAPBR=3,1,"APN","CMNET"
		UART_string(name,1);
		UART_string_newline("\"",1);
    delay_ms(1000);
    pos=strsearch(GSMUART_buff,"OK");  	
		if(pos)return 0;
		else	return 1;
}

//**********************************************
//����:���FALSH�е�CRC�Ƿ�����кż���CRC������ֵһ�£������һ�£���ѭ��
void Check_CDKEY(void)
{
		u8 poscd1=0,poscd2=0,plength=0,pbuffer[32]={0};
		u16 pcdkey_crc=0,check_cdkkey_crc=0;
		clear_USART1buff();
		UART_string_newline("AT+CGSN",1);//��ѯ���к�
		delay_ms(50);
		if(GSMUART_buff[0]==0x0d&&GSMUART_buff[1]==0x0a)poscd1=3;
		poscd2=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0aOK\x0d\x0a");
		if(poscd1!=0&&poscd2!=0)
		{	
				CoypArraryAtoArraryB_LR(GSMUART_buff,pbuffer,poscd2-poscd1,poscd1-1);//���Ʋ�������
				plength=poscd2-poscd1;
				pcdkey_crc=get_crc16_value(pbuffer,plength);//����CRC
				check_cdkkey_crc=MyEE_ReadWord(CDKEY_CRC16Add);//��ȡFLASH��CRC
				if(pcdkey_crc!=check_cdkkey_crc)//�ж��Ƿ�һ��
				{
						GPRS_FALUT_STATUS=1;//GPRS��ʼ��ʧ��
						while(1);//��ѭ��
				}
		}
}


void TCPConnect(char* IP_Add,char* Port_Add)
{
		clear_USART1buff();
		UART_string_newline("AT+CIPCLOSE=1",1);//һ��Ҫȷ��TCP�Ѿ��Ͽ�
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
//����:��GSMUART_buff�в���p2�ַ���λ��
//����:�ַ���p2
//����:����λ��pos
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
	for(i=0;i<lenth;i++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART1,_data[i]); 
	} 
}
void UART2_BUF_SEND(char * _data,u16 lenth)
{
	u16 i;
	for(i=0;i<lenth;i++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,_data[i]); 
	} 
}

//*************************************************
//����:����GSMģ���TCPIPΪ�ֶ�ģ��,������GPRS��ʼ��ǰ�������
//����: ����0ʧ�� 1�ɹ�
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
	UART_string_newline("AT+GSN",1);//��ѯ���к�
	delay_ms(50);
	if(GSMUART_buff[0]==0x0d&&GSMUART_buff[1]==0x0a) poscd1=3;
	poscd2=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0aOK\x0d\x0a");
	if(poscd1!=0&&poscd2!=0)
	{	
		CoypArraryAtoArraryB_LR(GSMUART_buff,IMEIbuff,poscd2-poscd1,poscd1-1);//�����кŸ��Ƶ�cdkeybuffer
		IMEIlen=poscd2-poscd1; 
		register_flag=IMEIFLAG;
		#if Debugpritn
		UART_string("�豸ΨһIMEI�ţ�",2);
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
	UART_string("SIM NetworkTime��",2);
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
	UART_string_newline("AT+SAPBR=2,1",1);//��ѯGPRS�������Ƿ�����
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
						UART_string_newline((u8 *)"����ʱ�����ݽ��շ��������سɹ�\r\n",2);
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
	UART_string_newline("AT+SAPBR=2,1",1);//��ѯGPRS�������Ƿ�����
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
						UART_string_newline((u8 *)"����ʱ�����ݽ��շ��������سɹ�\r\n",2);
						#endif
						//������ʱ�䣬д��ȥSIM RTC
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
							UART_string_newline((u8 *)"����ʱ��д��SIM�ɹ�\r\n",2);
							#endif
						}
						else 
						{
							#if  Debugpritn 
							UART_string_newline((u8 *)"����ʱ��д��SIMʧ��\r\n",2);
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
	UART_string_newline("AT+SAPBR=2,1",1);//��ѯGPRS�������Ƿ�����
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
				UART_string_newline((u8 *)"����ʱ�����ݷ��ͳɹ�\r\n",2);
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
						UART_string_newline((u8 *)"����ʱ�����ݽ��շ��������سɹ�\r\n",2);
						#endif
						//������ʱ�䣬д��ȥSIM RTC
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
							UART_string_newline((u8 *)"����ʱ��д��SIM�ɹ�\r\n",2);
							#endif
						}
						else 
						{
							#if  Debugpritn 
							UART_string_newline((u8 *)"����ʱ��д��SIMʧ��\r\n",2);
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
		 //UART_string_newline((u8 *)"��ȡSIMʱ��ɹ�\r\n",2);
		clear_USART1buff();
		#endif
		return 1;
	}
    else 
	{
		clear_USART1buff();
		#if  Debugpritn 
		UART_string_newline((u8 *)"��ȡSIMʱ��ʧ��\r\n",2);
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
				Signal_Buf=(Signal_Buf*32)/10;//תΪ�ٷֱȵ���ʽ
				if(Signal_Buf<=100)GsmSignal=Signal_Buf;
		}
		return GsmSignal;
}




