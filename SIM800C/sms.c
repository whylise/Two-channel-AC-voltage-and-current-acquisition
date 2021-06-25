#include "sms.h"

u8 CommonBuf[COMMONBUFMAX];//�������ͬ�ı��������ڲ�����Ҫ�õ�����RAM�������ݽ���ʱ���ã���ע�����ܱ�һֱ������

u16 Administrator_num_Flag=0;//����Ա�����־��1Ϊ����Ա��0Ϊ��ͨ�û�
u8 Smsdata[20]={0};//�������ݣ�ע�⣬���������ģ�ֻ����7BIT����
u8 sms_cmd_time_start=0;
u8 last_NUMBER_BUF[12]={0};//�߼��û���һ�β����ĺ���
u8 NeedToSMS=0;//�Ƿ���Ҫ�����ţ��ӷ�����GET��0Ϊ����Ҫ��1��Ҫ
u8 NumBer[12]={0};//��������

extern QueueHandle_t GSM_Message_Queue;	//��Ϣ���о��
extern QueueHandle_t FT_Message_Queue;//��Ϣ���о��
extern u8 GSMUART_buff[MAX_GSMUART];
extern u8 SimOperator;
extern CURR_RTCM FT_rtcm;
extern STATUS_RTCM FT_status;
extern u8 IsSMS;

//*********************************
//������������͹�������
void ClearCommonBuf(void)
{
		u16 i=0;
		for(i=0;i<COMMONBUFMAX;i++)CommonBuf[i]=0;
}

//****************************************************************************
//����:ͨ��PDUģʽ������ȡ����
//���أ�0 :�ɹ���ȡ 1:(����)û�ж��� 2:û���ҵ���Ч��index 3:���ص����ݴ��� 4:������û����Դ������Ϣ 
//5:����Ȩ�޺��뷢�͹�������Ϣ������   6:��ȡ�ɹ������ǲ���Ҫ���� 7: ɾ��ʧ�� 8:ɾ��ʧ�ܣ��˳��ȴ��´δ���
//10;�������Ϣ����Ҫɾ������ 11:���������Ϣ��ɾ������ 12:�ɹ���ȡ�ֻ�����
u8 readSMS(void)
{
		u16 pos=0,pos_end=0,i=0;
    u8 YEflag=0;
		u8 N_ReceNum[11]={0};
		clear_USART1buff();
		UART_string_newline("AT+CMGL=4,0",1);//�г����������� PDUģʽ 
		delay_ms(500);
		pos=strsearch(GSMUART_buff,"+CMGL:");
		if(pos!=0)
		{
				if(strsearch(GSMUART_buff,"05A10180F6")||strsearch(GSMUART_buff,"05800110F0"))
						YEflag=1;//10086��10010������
		}
		else if(strsearch(GSMUART_buff,"\x0d\x0aOK")==1)//û����
				return 1;
		else
				return 3;
		if(YEflag==1)//����10086��10010����������
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
				CoypArraryAtoArraryB(GSMUART_buff,CommonBuf,MAX_GSMUART);//��GSMUART_buff�е����ݱ��ݵ�CommonBuf��
				if(GetSimBalanceForBuf(CommonBuf)==0)//�ɹ���ȡ������
						return 10;
				else if(GetSimNumForBuf(CommonBuf)==0)//�ɹ���ȡ������
						return 12;
				else
					return 11;
		}
		else
		{
				if(pos)
				{
						pos_end=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0a");//��һ��
				}
				if(pos&&pos_end)
				{
						ClearArray(GSMUART_buff+pos_end+3,MAX_GSMUART-pos_end-4);//��������˵�һ������֮�������pdu����
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						CoypArraryAtoArraryB(GSMUART_buff,CommonBuf,MAX_GSMUART);//��GSMUART_buff�е����ݱ��ݵ�CommonBuf��
						pos=strsearch(CommonBuf,"0D9168");//�ж��Ƿ��ǹ��Һ����׼�ĺ���,����ͬ���뷢�͹��������
						if(!pos)
						{
								pos=strsearch(CommonBuf,"0BA1");//���������ͨ�����յ��������Ķ���
								pos=pos-2;
						}
						if((pos)&&(pos < pos_end))
						{
								for(i = 0;i<12;i++)N_ReceNum[i]=CommonBuf[pos+i+5];//��������ReceNum
								PhoneNumRecover(N_ReceNum);//������ָ�
								if(check_includs_num(N_ReceNum))//�ж��Ƿ���Ȩ�޵��û�
								{
										CheckSmsCmd(CommonBuf,N_ReceNum);//��������
										if(IsSMS==1)SMSCMD(Administrator_num_Flag,N_ReceNum);//����Ȩ�޴������ݣ��жϷ���ʲô���ݻ�ȥ
										DeleteAllSMS();
										return 0;//�ɹ���ȡ�������ɾ������
								}
								return 5;//����Ȩ���û������ģ�ɾ������
						}
						return 4; 
				}
				return 4;
		}
}

//*****************************************
//����:ɾ��ָ��index�Ķ���
//����:index:���index�ĵ�ַ
//����: 0:�ɹ�ɾ�� 1:index���� 2:ɾ��ʧ�� 
u8 DetelIndexSms(u8 *index)
{
    clear_USART1buff();
    if((*(index+0))&&((*(index+1))==0))//�жϷ��͵�index��1λ�� ���� 2λ��,ɾ�����յĶ���
    {
        UART_string("AT+CMGD=",1);          
        UART_Sendchar(index[0],1);
        UART_Sendchar(0x0d,1);
        UART_Sendchar(0x0a,1);
     }
     else if((*(index+0))&&(*(index+1)))
     {
        UART_string("AT+CMGD=",1);
        UART_Sendchar(index[0],1);
        UART_Sendchar(index[1],1);
        UART_Sendchar(0x0d,1);
        UART_Sendchar(0x0a,1);
     }
     else return 1;
		 delay_ms(500);
     if(strsearch(GSMUART_buff,"OK"))return 0;
     else return 2;		 
}

//************************************************************
//���������ڽ��ն��ŵ�ʱ�򣬻ָ�����,�ѻ�ȡ�ĺ�����ż�������Ұ�Fȥ��
//������*Num :��Żظ��ĺ���
//���أ�0:�ɹ�   1:����û�к���
u8 PhoneNumRecover(u8 *Num)
{
		u8 Len=0;
		u8 i=0;
		u8 temp=0;
		Len=CalculateLen(Num);//��ȡ�����������Ч����
		if((Len)&&(Len%2 == 0))
		{
				for(i=0;i<Len;i=i+2)
				{
						temp=*(Num+i);
						*(Num+i)=*(Num+i+1);
						*(Num+i+1)=temp;
				}
				*(Num+Len-1)=0x00;
		}
		else return 1;//���µĶ�Ϣ����,�����Ҳ���CMTI 
		return 0;
}

//***********************************************************************
//����:��ָ����buf�л�ȡ��Ч�������Ϣ
//����:pdata :�����Ϣ��buf(�Ѿ��Ƕ�������)
//����:0:��ȡ�ɹ�  1:������ַ���
u8 GetSimBalanceForBuf(u8 *pdata)
{
		BaseType_t err;
		u16 i=0,pos1=0,pos2=0,pos3=0;//"��"λ�� "���"λ�� "Ԫ"λ��
		u8 YEUnicode[64]={0},Queue_BUFF=0;    
		pos1 = strsearch(pdata,"5C0A656C");//"��",�ж��Ƿ�������ؼ���
		if(pos1)                //����"��",������λ����ǰ��. 
		{
				pos2=strsearch(pdata,"4F59989D");      //"���" ���ж��Ƿ�������ؼ���
				if(pos2)          //��30���ַ�����Ҫ����"���"����ؼ���
				{
						pos3 = strsearch(pdata,"5143");       //"Ԫ",�ж��Ƿ�������ؼ���
						if(pos3)           //��30���ַ�����Ҫ����"Ԫ"����ؼ���
						{
								if((*(pdata+pos3-5) == '0')&&(*(pdata+pos3-4) == '0'))//"Ԫ"�ؼ���ǰһ���� ����
										goto save;
						}
				}
		}
		return 1;
save:
		CoypArraryAtoArraryB_LR(pdata,YEUnicode,pos3-pos2+4,pos2-1);//����ָ������
		FLASH_Unlock(); 
		MyEE_WriteWord(BalanceLenAdd,pos3-pos2+4);//д����Flash
		for(i=0;i<(pos3-pos2+4);i++)MyEE_WriteWord(BalanceTextAdd+i,YEUnicode[i]);//д�������Flash
		FLASH_Lock();
		Queue_BUFF=POST_HTTP_SEFT;
		err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//�ϴ����
		return 0;
}

//***********************************************************************
//����:��ָ����buf�л�ȡ��Ч���ֻ�����
//����:pdata :�����Ϣ��buf(�Ѿ��Ƕ�������)
//����:0:��ȡ�ɹ�  1:������ַ���
u8 GetSimNumForBuf(u8 *pdata)
{
		u8 Queue_BUFF=0;
		u16 i=0,pos1=0;
		u8 NumUnicode[64]={0}; 
		BaseType_t err;
		pos1=strsearch(pdata,"53F778014E3A");//"����Ϊ",�ж��Ƿ�������ؼ���
		if(pos1)//����"����Ϊ",�Һ���Ϊ��λ����ǰ��. 
				goto Numsave;
		return 1;
Numsave:
		CoypArraryAtoArraryB_LR(pdata,NumUnicode,44,pos1+12-1);//����ָ������
		for(i=0;i<11;i++)NumBer[i]=NumUnicode[i*4+3];//ת�����뵽����
		Queue_BUFF=POST_HTTP_Num;
		err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//�ϴ�����
		return 0;
}


//***************************************************************************************
//����:���������unicode��(ֱ�Ӵ�CommonBuf�л�ȡ)���Լ�����,���Ͷ���
//����:pUnicode:unicode���ŵ�ַ Phnum:��Ҫ���͵ĺ���
//����: 0:���ͳɹ� 1:�����Unicode�ַ�������ȷ 2:���͵ĺ��볤�Ȳ���ȷ  3:û�з���CMGS OK,���ʹ���
u8 pdu_send_start(u8 *PhNum)
{
    u16 context_len=0,i=0,phlen=0,try_cnt=0;
    u8 SmsAmount = 0;//���Ͷ��ŵ�����
    u16 UnicodeActualLen=0,ThisUnicodeLen=0,ThisStartPos=0;//unicode����ʵ���� �����Ҫ���͵�Unicode�ĳ��� �����Ҫ���͵�Unicode�Ŀ�ʼλ��
	  u16 Random=11;//�����,��������Ψһʶ���־   
    if(!check_sms_unicode(CommonBuf,COMMONBUFMAX))//������ص���0�򲻽��뷢��, ��Ϊunicode�����
    {
				ClearCommonBuf();
				return 1; 
    }
    UART_string_newline("AT+CMGF=0",1);
    clear_USART1buff();
		delay_ms(300);
    phlen=CalculateLen(PhNum);//��ȡ���͵ĺ���ĳ���
    if((phlen!=11)&&(phlen!=5))return 2;      
    UnicodeActualLen=CalculateLenFromEnd(CommonBuf,COMMONBUFMAX);//��ȡunicode�볤��
    if(UnicodeActualLen <= SMSUnitLen)      //�ж���Ҫ��������
      SmsAmount=1;//1���ڿ��Է���
    else if(UnicodeActualLen <= SMSUnitLen*2)
      SmsAmount=2;//2���ڿ��Է���
    else if(UnicodeActualLen <= SMSUnitLen*3)
      SmsAmount=3;//3���ڿ��Է��� 
    else if(UnicodeActualLen <= SMSUnitLen*4)
      SmsAmount=4;//4���ڿ��Է��� 
    else
      return 1;//�����Unicode�벻��ȷ 
    for(i=0;i<SmsAmount;i++)
    {
				if((SmsAmount==1)&&(i==0))//1������
				{
						context_len=UnicodeActualLen;
						ThisUnicodeLen=UnicodeActualLen;
						ThisStartPos=0;  
				}
				else if(((SmsAmount==2)||(SmsAmount==3)||(SmsAmount==4))&&(i==0))//N�����ŵ�1�η���
				{
						context_len=SMSUnitLen+12;
						ThisUnicodeLen=SMSUnitLen;
						ThisStartPos=0;
				}
				else if(((SmsAmount==3)||(SmsAmount==4))&&(i==1))//N�����ŵ�2�η���
				{
						context_len=SMSUnitLen+12;
						ThisUnicodeLen=SMSUnitLen;
						ThisStartPos=SMSUnitLen;	
				}
				else if((SmsAmount==2)&&(i==1))//�������ŵ�2�η���
				{
						context_len=UnicodeActualLen-SMSUnitLen+12;
						ThisUnicodeLen=UnicodeActualLen-SMSUnitLen;
						ThisStartPos=SMSUnitLen; 
				}
				else if(((SmsAmount==4))&&(i==2))//N�����ŵ�3�η���
				{
						context_len=SMSUnitLen+12;
						ThisUnicodeLen=SMSUnitLen;
						ThisStartPos=SMSUnitLen*2;	
				}
				else if((SmsAmount==3)&&(i==2))
				{
						context_len=UnicodeActualLen-2*SMSUnitLen+12;
						ThisUnicodeLen=UnicodeActualLen-2*SMSUnitLen;
						ThisStartPos=SMSUnitLen*2; 
				}
				else if((SmsAmount==4)&&(i==3))//N�����ŵ�4�η���
				{
						context_len=UnicodeActualLen-3*SMSUnitLen+12;
						ThisUnicodeLen=UnicodeActualLen-3*SMSUnitLen;
						ThisStartPos=SMSUnitLen*3; 
				} 
				UART_string("AT+CMGS=",1);
				if(phlen == 11)
					UART_string_newline(U16IntToStr((32+context_len)/2-1),1); 
				else if(phlen == 5)
					UART_string_newline(U16IntToStr((24+context_len)/2-1),1); 
				clear_USART1buff();
				delay_ms(300);    
				if(SmsAmount == 1)//�����Ƿ���Ҫ��ͷ
					UART_string("0011",1);
				else if((SmsAmount == 2)||(SmsAmount == 3)||(SmsAmount == 4))
					UART_string("0051",1); 
				if(phlen == 5)            
					UART_string("0005A1",1);
				else if(phlen == 11)      
					UART_string("000D9168",1);
				UART_string(num_change(PhNum),1);//���;�����ż�����ĺ���
				UART_string("000800",1);//��Ч��Ϊ00 ��Ӧ����5����
				UART_string(hextoasciihex(context_len/2),1);  
				if(SmsAmount != 1)//�������Ҫ��ֵĶ��ţ�����Ҫ�ⲿ��
				{	
						UART_string("050003",1);//�̶� 
						UART_string(hextoasciihex(Random),1);//�����ţ�ͨ��ʱ��+10����
						UART_string(hextoasciihex(SmsAmount),1);//����������
						UART_string(hextoasciihex(i+1),1);//���ǵ�N��
				}
				UART_string_amount(CommonBuf+ThisStartPos,ThisUnicodeLen,1);   //��������
				delay_ms(100);
				UART_Sendchar(0x1A,1);
				for(try_cnt=0;try_cnt<10;try_cnt++)
				{
						delay_ms(1000);
						if(strsearch(GSMUART_buff,"+CMGS"))
							break;	
				}
				if(strsearch(GSMUART_buff,"+CMGS"));
				else 
					return 3;
    }
    return 0;//���ŷ��ͳɹ�

}

//**************************************************************
//���������ڼ�����ɵ�unicode���λ���Լ��ַ����Ƿ���ȷ��ע�⣬������㳤����ͨ����δ�˿�ʼ���֪����⵽����
//������*pdata   :��Ҫ�������ڴ��unicode������� ��max_len :���������󳤶�
//����: 1����ȷ 0:����ȷ
u8 check_sms_unicode(u8 *pdata,u16 Maxlen)
{
		u16 amount=0;
		amount=CalculateLenFromEnd(pdata,Maxlen);
		if((amount%4==0)&&(amount)&&(amount<=SMSUnitLen *4))
			return 1;
		else
			return 0;
}

//***********************************************************
//����:����Commonbuf�е����ݷ��Ͷ��ŵ�ָ���ĺ���
//����:TargetNum :Ŀ�����
//����:0:�ɹ�  1:��ȫʧ��  2:����ԭ���ʧ��
u8 SendSmsByCommonbuf(u8 *TargetNum)
{
	u8 res=0,err=0,i=0;
	for(i = 0;i<2;i++)
	{
			res=pdu_send_start(TargetNum);
			if(res == 0)
					return 0;
			else if(res ==3)//������ԭ��ʧ��
			{
					err++;//���η���ʧ��
					if(err==2)
							return 1;
			}
			else 
					return 2;//����ԭ��ʧ�� 
			delay_ms(100);
	 }
	 return 0;
}

//**********************************
//����:���Ͳ�ѯ���ָ��
void SendGetBalanceSms(void)
{
   ClearCommonBuf();
   if(SimOperator==ChinaMobile)//������ƶ�
      AsciiToUnicodeAppendCommonBuf("YE");
   else if(SimOperator==ChinaUnicom)//�������ͨ
      AsciiToUnicodeAppendCommonBuf("YE"); 
   if(SimOperator==ChinaMobile)
			SendSmsByCommonbuf("10086");
   else if(SimOperator==ChinaUnicom)
			SendSmsByCommonbuf("10010");
    ClearCommonBuf();
}

//**********************************
//����:���Ͳ�ѯ����ָ��
void SendGetNumSms(void)
{
    ClearCommonBuf();
    if(SimOperator == ChinaMobile)//������ƶ�
       AsciiToUnicodeAppendCommonBuf("BJ");
    else if(SimOperator == ChinaUnicom)//�������ͨ
       AsciiToUnicodeAppendCommonBuf("CXHM"); 
    if(SimOperator == ChinaMobile)
			 SendSmsByCommonbuf("10086");
    else if(SimOperator == ChinaUnicom)
			 SendSmsByCommonbuf("10010");
		ClearCommonBuf();
}

//**************************************************************************************
//����:��asciiתΪunicode�룬������ӵ�CommonBuf��
//����:src:��Ҫת�����ַ���
//����:
void AsciiToUnicodeAppendCommonBuf(u8 *src)
{
		u16 curLen = 0;
		curLen=CalculateLenFromEnd(CommonBuf,COMMONBUFMAX);
		AScSwitchUni(src,CommonBuf+curLen);
}

/***************************************************************************************
//��������      AScSwitchUni
//��������      ������ת����Uni
//��ں���      *p ָ�������  *p2
//���ں���      ƫ�ƶ� m
***************************************************************************************/
u8  AScSwitchUni(unsigned char *p,unsigned char *p2)
{
		u8 temp[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		u8 i=0,a=0,b=0,m=0;
		for(;*p!=0;p++)
			 temp[i++]=*p;
		for(i=0;temp[i]!=0;i++)
		{
				*p2++='0';
				*p2++='0';
				a=temp[i]/16%16;
				b=temp[i]%16;
				*p2++ = HexConvernum(a);
				*p2++ = HexConvernum(b);		
				m+=4;
		}
		return m;
}

//*********************************
//��⵱ǰ��sms �û��Ƿ���Ȩ���û�
//����0������Ȩ���û���1�ǣ�1���ϣ�Ϊ���û��Ĵ��˳��λ��
u8 check_includs_num(u8 *cur_num)
{
		u8 check_i=0,check_j=0,num_flag=0;
		u8 cnt=0; 
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//���ж��Ƿ����ڸ߼��û�
				{
						for(check_j=0;check_j<11;check_j++)
						{
							 if(cur_num[check_j]== MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh))num_flag=1;
							 else
							 {			
									num_flag=0;
									break;
							 }          
						}
						if(num_flag)//ѭ��11�κ��ҵ���ͬ�ĺ��� 
						{ 
								Administrator_num_Flag=1;//�˺���Ϊ�߼��û�������ԱȨ���û�
								break;//���ҵ���ɵĺ���
						}
				}
		}
		if(num_flag==0)
		{
				cnt=MyEE_ReadWord(MobileCntAdd);
				if(cnt!=0)
				{ 
						if(cnt>10)cnt=10;
						for(check_i=0;check_i<cnt;check_i++)//���ж��Ƿ����ڵͼ��û�
						{
								for(check_j=0;check_j<11;check_j++)
								{
									 if(cur_num[check_j]== MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh))num_flag=1;
									 else
									 {			
											num_flag=0;
											break;
									 }          
								}
								if(num_flag)//ѭ��11�κ��ҵ���ͬ�ĺ��� 
								{ 
										Administrator_num_Flag=0;//�˺���Ϊ�ͼ��û�
										break;//���ҵ���ɵĺ���
								}
						}
				}
		}
		if(num_flag==1)num_flag=check_i+1;//������ҵ���ɵĺ����򷵻ظú����λ�ü�1
		return num_flag;
}

//****************************************************************
//�������жϽ��ջ�����PDU���ݣ��ж��Ƿ�����Ч��ָ��
//������*pdata		 *ReceNum:���յ��ĺ���
//���أ�0:�ɹ���sms�е����ݴ���. 1:û�гɹ���������
u8 CheckSmsCmd(u8 *pdata,u8 *ReceNum)
{
		u16 pos=0,StartPos=0,DataLen=0,i=0;
		u8 Smsdatatemp[20]={0};
		for(i=0;i<20;i++)Smsdata[i]=0;
		pos=strsearch(pdata,"0D9168");
		if(!pos)
		{       
				pos=strsearch(pdata,"0BA1");//���������ͨ�����յ��������Ķ���
				pos=pos-2;
		}
		if(pos)
		{
				DataLen=((*(pdata+pos+35)-'0')*10+*(pdata+pos+36)-'0') *2;
				StartPos=pos+37;//�������ݿ�ʼ��λ��   
				if(DataLen<=40)//�������С��16,���п����Ǵ�ascii���ַ������͹�������7BIT����
				{     
						HexStringToHex(pdata+StartPos,Smsdatatemp,DataLen);
						gsmDecode7bit(Smsdatatemp,(char*)Smsdata,DataLen/2+1);
						return 0;
				}
		}
		return 1;
}


//****************************************************************
//��������Flash����ֻ����룬ÿ������֮���� �� ���
//������*DATABUF �ֻ�����ָ�룬flashmode ��ŵ��Ǹ߼��û����ǵͼ��û� 1Ϊ�߼� 0Ϊ�ͼ�
void PhoneNumlist(u8 *DATABUF,u8 flashmode)
{
		u16 num_i=0,num_cnt=0;
		if(flashmode==0)//��ŵͼ�Ȩ��
		{	
				FLASH_Unlock();
				for(num_i=0;DATABUF[num_i]!=0;num_i++)
				{
						if(DATABUF[num_i]==',')num_cnt++;
						else
								MyEE_WriteWord(MobileAdd+num_i,DATABUF[num_i]); 
				}
				MyEE_WriteWord(MobileCntAdd,num_cnt+1);
				FLASH_Lock();
		}
		else if(flashmode==1)//��Ÿ߼�Ȩ��
		{
				FLASH_Unlock();
				for(num_i=0;DATABUF[num_i]!=0;num_i++)
				{
						if(DATABUF[num_i]==',')num_cnt++;
						else
								MyEE_WriteWord(WhiteListAdd+num_i,DATABUF[num_i]); 
				}
				MyEE_WriteWord(WhiteListCntAdd,num_cnt+1);
				FLASH_Lock();
		}
}

//****************************************************************
//�����������û�Ȩ���ж϶������ݵ�ָ�����һ������
//������admin_level �û�Ȩ�� 1Ϊ�߼�Ȩ�� 0Ϊ�ͼ�Ȩ�� *SEND_NUMBER_SMSΪ�����ߵ��ֻ�����
void SMSCMD(u8 admin_level,u8 *SEND_NUMBER_SMS)
{
		u16 smscmd_i=0;
		u8 Queue_BUFF=0;
		BaseType_t err;
		static u8 last_cmd=0;//��һ��ָ��ı��� 1Ϊ�Լ죬2Ϊ��λ
		if((Smsdata[0]=='C'||Smsdata[0]=='c')&&(Smsdata[1]=='X'||Smsdata[1]=='x')&&(Smsdata[2]=='Z'||Smsdata[2]=='z')&&(Smsdata[3]=='T'||Smsdata[3]=='t'))//CXZT ��ѯ״̬
		{
				SMS_To_ZT();//̽������ǰ״̬ת��unicode������CommonBuf������
				if(admin_level==1)//�߼��û�ר��
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(��ѯ״̬)��ZJ(�Լ�)��FW(��λ)
				else if(admin_level==0)//�ͼ����û�ר��
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(��ѯ״̬)
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//���Ͷ��Ÿ���ǰ�ֻ�����
		}
		else if((Smsdata[0]=='Z'||Smsdata[0]=='z')&&(Smsdata[1]=='J'||Smsdata[1]=='j')&&admin_level==1)//ZJ �Լ� �߼��û�ר��
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
				UnicodeAppendCommonBuf("8bf7518d6b21786e8ba4662f54269700898181ea68c0ff0c");//���ٴ�ȷ���Ƿ���Ҫ�Լ죬
				UnicodeAppendCommonBuf("786e8ba48bf756de590d005900450053ff0c4e0d786e8ba48bf756de590d004e004f62164e245206949f51854e0d56de590d6b648bf76c4259316548");//ȷ����ظ�YES����ȷ����ظ�NO���������ڲ��ظ�������ʧЧ
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//���Ͷ��Ÿ���ǰ�ֻ�����
				strcpy((char*)last_NUMBER_BUF,(char*)SEND_NUMBER_SMS);//����ǰ�û����뱣��
				sms_cmd_time_start=1;//��ʼ��ʱ
				last_cmd=1;//��¼��ǰ�û���ָ��
		}
		else if((Smsdata[0]=='F'||Smsdata[0]=='f')&&(Smsdata[1]=='W'||Smsdata[1]=='w')&&admin_level==1)//FW ��λ �߼��û�ר��
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
				UnicodeAppendCommonBuf("8bf7518d6b21786e8ba4662f542697008981590d4f4dff0c");//���ٴ�ȷ���Ƿ���Ҫ��λ��
				UnicodeAppendCommonBuf("786e8ba48bf756de590d005900450053ff0c4e0d786e8ba48bf756de590d004e004f62164e245206949f51854e0d56de590d6b648bf76c4259316548");//ȷ����ظ�YES����ȷ����ظ�NO���������ڲ��ظ�������ʧЧ
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//���Ͷ��Ÿ���ǰ�ֻ�����
				strcpy((char*)last_NUMBER_BUF,(char*)SEND_NUMBER_SMS);//����ǰ�û����뱣��
				sms_cmd_time_start=1;//��ʼ��ʱ
				last_cmd=2;//��¼��ǰ�û���ָ��
		}
		else if((Smsdata[0]=='Y'||Smsdata[0]=='y')&&(Smsdata[1]=='E'||Smsdata[1]=='e')&&(Smsdata[2]=='S'||Smsdata[2]=='s')&&admin_level==1&&strsearch(last_NUMBER_BUF,SEND_NUMBER_SMS)&&sms_cmd_time_start==1)//�����������յ�ͬһ�û�������yes
		{
				sms_cmd_time_start=0;
				if(last_cmd==1)//��ʼ���͹����Լ���������
				{
						last_cmd=0;//���ָ���¼
						NeedToSMS=1;//�Լ���ɺ���Ҫ������
						Queue_BUFF=W_Self_Check_Task;err=xQueueSend(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//��̽���������Լ�����
				}
				else if(last_cmd==2)//��ʼ���͹��ڸ�λ���������
				{
						last_cmd=0;//���ָ���¼
						NeedToSMS=1;//��λ��ɺ���Ҫ������
						//���浽FLash����ز��������������Ͷ���
						FLASH_Unlock();
						for(smscmd_i=0;SEND_NUMBER_SMS[smscmd_i]!=0;smscmd_i++)MyEE_WriteWord(SMS_FW_NUM_Add+smscmd_i,SEND_NUMBER_SMS[smscmd_i]);//���浱ǰ��λ���ֻ����뵽Flash
						FLASH_Lock();
						Queue_BUFF=W_RESET_Task;
						err=xQueueSend(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//��̽�������͸�λ����
				}
				
		}
		else if((Smsdata[0]=='N'||Smsdata[0]=='n')&&(Smsdata[1]=='O'||Smsdata[1]=='o')&&admin_level==1&&strsearch(last_NUMBER_BUF,SEND_NUMBER_SMS)&&sms_cmd_time_start==1)
		{
				sms_cmd_time_start=0;//�����ʱ
				last_cmd=0;//���ָ���¼
		}
		else if(admin_level==1)//��������������ض��Ų���ָ���,�߼��û�
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
				UnicodeAppendCommonBuf("65e0654863074ee4000d000a64cd4f5c63074ee4ff1a000d000a");//��Чָ��+����+����ָ�+����
				UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(��ѯ״̬)��ZJ(�Լ�)��FW(��λ)
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//���Ͷ��Ÿ���ǰ�ֻ�����
		}
		else if(admin_level==0)//��������������ض��Ų���ָ���,�ͼ��û�
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
				UnicodeAppendCommonBuf("65e0654863074ee4000d000a64cd4f5c63074ee4ff1a000d000a00430058005a0054002867e58be272b660010029000d000a");//��Чָ��+���в���ָ�+����CXZT(��ѯ״̬)+����
				UnicodeAppendCommonBuf("598260f383b753d666f4591a64cd4f5c63074ee4ff0c8bf780547cfb516c53f87ba17406545883b753d667439650");//�����ȡ�������ָ�����ϵ��˾����Ա��ȡȨ��
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//���Ͷ��Ÿ���ǰ�ֻ�����
		}
}

//****************************************************************
//������̽������ǰ״̬ת��unicode������CommonBuf������
void SMS_To_ZT(void)
{
		u16 Address_Len=0,smscmd_i=0;
		u8 Address_buf[200]={0};
		ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
		UnicodeAppendCommonBuf("63a26d4b56685f53524d72b660014fe1606f000d000a");//̽������ǰ״̬��Ϣ+����
		UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//������˾��
		Address_Len=MyEE_ReadWord(SimNameLenAdd);
		for(smscmd_i=0;smscmd_i<200;smscmd_i++)
				Address_buf[smscmd_i]=0;
		for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
				Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
		UnicodeAppendCommonBuf(Address_buf);
		UnicodeAppendCommonBuf("000d000a");//����
		UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//��װλ�ã�
		Address_Len=MyEE_ReadWord(ADDRESSLenAdd);
		for(smscmd_i=0;smscmd_i<200;smscmd_i++)
				Address_buf[smscmd_i]=0;
		for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
				Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
		UnicodeAppendCommonBuf(Address_buf);
		UnicodeAppendCommonBuf("000d000a63a26d4b70b9ff1a68c06d4b503c002f62a58b669650503c000d000a");//����+̽��㣺���ֵ/������ֵ+����
		UnicodeAppendCommonBuf("6f0f75356d41ff1a");//©����:
		if(FT_status.RTCM_LS==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Lcurrent));
				UnicodeAppendCommonBuf("006d0041002f");//mA/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_LcAvalue));
				UnicodeAppendCommonBuf("006d0041ff1b000d000a");//mA��+����
		}
		UnicodeAppendCommonBuf("004176f875356d41ff1a");//A�����:
		if(FT_status.RTCM_CSA==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Acurrent)); 
				UnicodeAppendCommonBuf("0041002f");//A/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_AcAvalue)); 
				UnicodeAppendCommonBuf("0041ff1b000d000a");//A��+����
		}
		UnicodeAppendCommonBuf("004276f875356d41ff1a");//B�����:
		if(FT_status.RTCM_CSB==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Bcurrent)); 
				UnicodeAppendCommonBuf("0041002f");//A/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_BcAvalue)); 
				UnicodeAppendCommonBuf("0041ff1b000d000a");//A��+����
		}
		UnicodeAppendCommonBuf("004376f875356d41ff1a");//C�����:
		if(FT_status.RTCM_CSC==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Ccurrent)); 
				UnicodeAppendCommonBuf("0041002f");//A/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_CcAvalue)); 
				UnicodeAppendCommonBuf("0041ff1b000d000a");//A��+����
		}
		UnicodeAppendCommonBuf("004176f86e295ea6ff1a");//A���¶�:
		if(FT_status.RTCM_TS1==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Atemp));
				UnicodeAppendCommonBuf("5ea6002f");//��/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_AtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
		}
		UnicodeAppendCommonBuf("004276f86e295ea6ff1a");//B���¶�:
		if(FT_status.RTCM_TS2==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Btemp));
				UnicodeAppendCommonBuf("5ea6002f");//��/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_BtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
		}
		UnicodeAppendCommonBuf("004376f86e295ea6ff1a");//C���¶�:
		if(FT_status.RTCM_TS3==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Ctemp));
				UnicodeAppendCommonBuf("5ea6002f");//��/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_CtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
		}
		UnicodeAppendCommonBuf("73af58836e295ea6ff1a");//�����¶�:
		if(FT_status.RTCM_TS4==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//�ް�װ��+����
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Etemp));
				UnicodeAppendCommonBuf("5ea6002f");//��/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_EtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
		}
		UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
}

//****************************************************************
//�������������û�����ĳ�û������Լ����ָ��Ķ���
void SMS_To_ALL_Of_ZJ(void)
{
		u8 sendnum_buf[11]={0},Address_buf[200]={0};
		u16 Address_Len=0,smscmd_i=0;
		u8 check_i=0,check_j=0,cnt=0;
		//����߼��û����Ͷ���
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�߼��û�
				{
						for(check_j=0;check_j<11;check_j++)sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);	
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("75286237");//�û�
						AsciiToUnicodeAppendCommonBuf(last_NUMBER_BUF);
						UnicodeAppendCommonBuf("5bf9");//��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);//��ȡ��ַbuf
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c81ea68c064cd4f5c5df25b8c6210000d000a");//��̽���������Լ���������+����
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(��ѯ״̬)��ZJ(�Լ�)��FW(��λ)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�ͼ��û�
				{
						for(check_j=0;check_j<11;check_j++)sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("75286237");//�û�
						AsciiToUnicodeAppendCommonBuf(last_NUMBER_BUF);
						UnicodeAppendCommonBuf("5bf9");//��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);//��ȡ��ַbuf
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c81ea68c064cd4f5c5df25b8c6210000d000a");//��̽���������Լ���������+����
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(��ѯ״̬)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
}

//****************************************************************
//�������������û�����ĳ�û����и�λ����ָ��Ķ���
void SMS_To_ALL_Of_FW(void)
{
		u8 sendnum_buf[11]={0},flash_num[11]={0},Address_buf[200]={0};
		u16 Address_Len=0,smscmd_i=0;
		u8 check_i=0,check_j=0,cnt=0;
		for(check_j=0;check_j<11;check_j++)flash_num[check_j]=MyEE_ReadWord(SMS_FW_NUM_Add+check_j);
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�߼��û�
				{
						for(check_j=0;check_j<11;check_j++)
							 sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);	
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("75286237");//�û�
						AsciiToUnicodeAppendCommonBuf(flash_num);
						UnicodeAppendCommonBuf("5bf9");//��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//��ȡ��ַbuf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c590d4f4d64cd4f5c5df25b8c6210000d000a");//��̽�������и�λ���������+����
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(��ѯ״̬)��ZJ(�Լ�)��FW(��λ)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�ͼ��û�
				{
						for(check_j=0;check_j<11;check_j++)
								sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("75286237");//�û�
						AsciiToUnicodeAppendCommonBuf(flash_num);
						UnicodeAppendCommonBuf("5bf9");//��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//��ȡ��ַbuf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c590d4f4d64cd4f5c5df25b8c6210000d000a");//��̽�������и�λ���������+����
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(��ѯ״̬)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		FLASH_Unlock();
		MyEE_WriteWord(SMS_FW_Add,0x00);//д��־λ
		FLASH_Lock();
}

//****************************************************************
//�������������û����ͳ��ޱ����Ķ���
void SMS_To_ALL_Of_Alarming(void)
{
		u8 sendnum_buf[11]={0},Address_buf[200]={0};
		u8 check_i=0,check_j=0,cnt=0;
		u16 Address_Len=0,smscmd_i=0;
		//����߼��û����Ͷ���
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�߼��û�
				{
						for(check_j=0;check_j<11;check_j++)
							 sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);						
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f8d85965062a58b66000d000a");//̽�����������ޱ���+����
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//������˾��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						UnicodeAppendCommonBuf("000d000a");//����
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//��װλ�ã�
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//��ȡ��ַbuf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						SMS_Alarming();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT����ѯ״̬����ZJ���Լ죩��FW����λ��
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�ͼ��û�
				{
						for(check_j=0;check_j<11;check_j++)
								sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f8d85965062a58b66000d000a");//̽�����������ޱ���+����
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//������˾��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						UnicodeAppendCommonBuf("000d000a");//����
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//��װλ�ã�
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//��ȡ��ַbuf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						SMS_Alarming();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(��ѯ״̬)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
}

void SMS_Alarming(void)
{
				UnicodeAppendCommonBuf("000d000a62a58b6670b9ff1a62a58b66503c002f62a58b669650503c000d000a");//����+�����㣺����ֵ/������ֵ+����
				if(FT_status.RTCM_CSA==Status_Alarming)//A���������
				{
						UnicodeAppendCommonBuf("004176f875356d41ff1a");//A�������
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Acurrent));
						UnicodeAppendCommonBuf("0041002f");//A/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_AcAvalue));
						UnicodeAppendCommonBuf("0041ff1b000d000a");//A��+����
				}
				if(FT_status.RTCM_CSB==Status_Alarming)//B���������
				{
						UnicodeAppendCommonBuf("004276f875356d41ff1a");//B�������
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Bcurrent));
						UnicodeAppendCommonBuf("0041002f");//A/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_BcAvalue));
						UnicodeAppendCommonBuf("0041ff1b000d000a");//A��+����
				}
				if(FT_status.RTCM_CSC==Status_Alarming)//C���������
				{
						UnicodeAppendCommonBuf("004376f875356d41ff1a");//C�������
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Ccurrent));
						UnicodeAppendCommonBuf("0041002f");//A/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_CcAvalue));
						UnicodeAppendCommonBuf("0041ff1b000d000a");//A��+����
				}
				if(FT_status.RTCM_LS==Status_Alarming)//©��������
				{
						UnicodeAppendCommonBuf("6f0f75356d41ff1a");//©������
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Lcurrent));
						UnicodeAppendCommonBuf("006d0041002f");//mA/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_LcAvalue));
						UnicodeAppendCommonBuf("006d0041ff1b000d000a");//mA��+����
				}
				if(FT_status.RTCM_TS1==Status_Alarming)//A���¶ȳ���
				{
						UnicodeAppendCommonBuf("004176f86e295ea6503cff1a");//A���¶�ֵ��
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Atemp));
						UnicodeAppendCommonBuf("5ea6002f");//��/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_AtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
				}
				if(FT_status.RTCM_TS2==Status_Alarming)//B���¶ȳ���
				{
						UnicodeAppendCommonBuf("004276f86e295ea6503cff1a");//B���¶�ֵ��
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Btemp));
						UnicodeAppendCommonBuf("5ea6002f");//��/
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_BtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
				}
				if(FT_status.RTCM_TS3==Status_Alarming)//C���¶ȳ���
				{
						UnicodeAppendCommonBuf("004376f86e295ea6503cff1a");//C���¶�ֵ��
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Ctemp));
						UnicodeAppendCommonBuf("5ea6002f");//��/
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_CtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
				}
				if(FT_status.RTCM_TS4==Status_Alarming)//�����¶ȳ���
				{
						UnicodeAppendCommonBuf("73af58836e295ea6503cff1a");//�����¶�ֵ��
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Etemp));
						UnicodeAppendCommonBuf("5ea6002f");//��/
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_EtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//�ȣ�+����
				}
				UnicodeAppendCommonBuf("53ef80fd5b5857285b895168969060a3ff0c8bf753ca65f6639267e5ff0c4ee59632706b707eff01000d000a");//���ܴ��ڰ�ȫ�������뼰ʱ�Ų飬�Է����֣�+����
				UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
}

//****************************************************************
//�������������û����Ͷ��߹��ϱ����Ķ���
void SMS_To_ALL_Of_BrokenAndShort(void)
{
		u8 sendnum_buf[11]={0},Address_buf[200]={0};
		u8 check_i=0,check_j=0,cnt=0;
		u16 Address_Len=0,smscmd_i=0;
		//����߼��û����Ͷ���
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�߼��û�
				{
						for(check_j=0;check_j<11;check_j++)
							 sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);	
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f6545969c62a58b66000d000a");//̽�����������ϱ���+����
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//������˾��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						UnicodeAppendCommonBuf("000d000a");//����
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//��װλ�ã�
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//��ȡ��ַbuf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						SMS_Broken_Short();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(��ѯ״̬)��ZJ(�Լ�)��FW(��λ)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//�ͼ��û�
				{
						for(check_j=0;check_j<11;check_j++)
								sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//�������CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f6545969c62a58b66000d000a");//̽�����������ϱ���+����
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//������˾��
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����Ĺ�����˾
						UnicodeAppendCommonBuf("000d000a");//����
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//��װλ�ã�
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//��ȡ��ַbuf����
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//��ȡ��ַbuf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//̽�����İ�װλ��
						SMS_Broken_Short();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(��ѯ״̬)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
}

//****************************************************************
//�������������û����Ͷ�·���ϱ����Ķ���
void SMS_Broken_Short(void)
{
		UnicodeAppendCommonBuf("000d000a62a58b6670b9ff1a6545969c7c7b578b000d000a");//����+�����㣺��������+����
		if(FT_status.RTCM_LS==Status_Broken)
		{
				UnicodeAppendCommonBuf("6f0f75356d41ff1a65ad7ebf6545969cff1b000d000a");//©���������߹��ϣ�+����
		}
		else if(FT_status.RTCM_LS==Status_Short)
		{
				UnicodeAppendCommonBuf("6f0f75356d41ff1a77ed8def6545969cff1b000d000a");//©��������·���ϣ�+����
		}
		if(FT_status.RTCM_TS1==Status_Broken)
		{
				UnicodeAppendCommonBuf("004176f86e295ea6ff1a65ad7ebf6545969cff1b000d000a");//A���¶ȣ����߹��ϣ�+����
		}
		else if(FT_status.RTCM_TS1==Status_Short)
		{
				UnicodeAppendCommonBuf("004176f86e295ea6ff1a77ed8def6545969cff1b000d000a");//A���¶ȣ���·���ϣ�+����
		}
		if(FT_status.RTCM_TS2==Status_Broken)
		{
				UnicodeAppendCommonBuf("004276f86e295ea6ff1a65ad7ebf6545969cff1b000d000a");//B���¶ȣ����߹��ϣ�+����
		}
		else if(FT_status.RTCM_TS2==Status_Short)
		{
				UnicodeAppendCommonBuf("004276f86e295ea6ff1a77ed8def6545969cff1b000d000a");//B���¶ȣ���·���ϣ�+����
		}
		if(FT_status.RTCM_TS3==Status_Broken)
		{
				UnicodeAppendCommonBuf("004376f86e295ea6ff1a65ad7ebf6545969cff1b000d000a");//C���¶ȣ����߹��ϣ�+����
		}
		else if(FT_status.RTCM_TS3==Status_Short)
		{
				UnicodeAppendCommonBuf("004376f86e295ea6ff1a77ed8def6545969cff1b000d000a");//C���¶ȣ���·���ϣ�+����
		}
		if(FT_status.RTCM_TS4==Status_Broken)
		{
				UnicodeAppendCommonBuf("73af58836e295ea6ff1a65ad7ebf6545969cff1b000d000a");//�����¶ȣ����߹��ϣ�+����
		}
		else if(FT_status.RTCM_TS4==Status_Short)
		{
				UnicodeAppendCommonBuf("73af58836e295ea6ff1a77ed8def6545969cff1b000d000a");//�����¶ȣ���·���ϣ�+����
		}
		UnicodeAppendCommonBuf("53ef80fd5b5857285b895168969060a3ff0c8bf753ca65f6639267e5ff0c4ee59632706b707eff01000d000a");//���ܴ��ڰ�ȫ�������뼰ʱ�Ų飬�Է����֣�+����
		UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//����ָ�+����
}

