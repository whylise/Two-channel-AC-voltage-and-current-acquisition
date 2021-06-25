#include "sms.h"

u8 CommonBuf[COMMONBUFMAX];//定义个共同的变量，用于部分需要用到大量RAM进行数据交换时候用，备注：不能被一直暂用着

u16 Administrator_num_Flag=0;//管理员号码标志，1为管理员，0为普通用户
u8 Smsdata[20]={0};//短信内容，注意，不能有中文，只能是7BIT编码
u8 sms_cmd_time_start=0;
u8 last_NUMBER_BUF[12]={0};//高级用户上一次操作的号码
u8 NeedToSMS=0;//是否需要发短信，从服务器GET，0为不需要，1需要
u8 NumBer[12]={0};//本机号码

extern QueueHandle_t GSM_Message_Queue;	//信息队列句柄
extern QueueHandle_t FT_Message_Queue;//信息队列句柄
extern u8 GSMUART_buff[MAX_GSMUART];
extern u8 SimOperator;
extern CURR_RTCM FT_rtcm;
extern STATUS_RTCM FT_status;
extern u8 IsSMS;

//*********************************
//描述：清楚大型公共变量
void ClearCommonBuf(void)
{
		u16 i=0;
		for(i=0;i<COMMONBUFMAX;i++)CommonBuf[i]=0;
}

//****************************************************************************
//描述:通过PDU模式主动读取短信
//返回：0 :成功读取 1:(常规)没有短信 2:没有找到有效的index 3:返回的数据错误 4:数据中没有来源号码信息 
//5:不是权限号码发送过来的信息，抛弃   6:获取成功，但是不需要处理 7: 删除失败 8:删除失败，退出等待下次处理
//10;是余额信息，需要删除短信 11:不是余额信息，删除短信 12:成功获取手机号码
u8 readSMS(void)
{
		u16 pos=0,pos_end=0,i=0;
    u8 YEflag=0;
		u8 N_ReceNum[11]={0};
		clear_USART1buff();
		UART_string_newline("AT+CMGL=4,0",1);//列出所有条短信 PDU模式 
		delay_ms(500);
		pos=strsearch(GSMUART_buff,"+CMGL:");
		if(pos!=0)
		{
				if(strsearch(GSMUART_buff,"05A10180F6")||strsearch(GSMUART_buff,"05800110F0"))
						YEflag=1;//10086或10010发来的
		}
		else if(strsearch(GSMUART_buff,"\x0d\x0aOK")==1)//没短信
				return 1;
		else
				return 3;
		if(YEflag==1)//处理10086或10010发来的内容
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
				CoypArraryAtoArraryB(GSMUART_buff,CommonBuf,MAX_GSMUART);//把GSMUART_buff中的数据备份到CommonBuf中
				if(GetSimBalanceForBuf(CommonBuf)==0)//成功获取并保存
						return 10;
				else if(GetSimNumForBuf(CommonBuf)==0)//成功获取并保存
						return 12;
				else
					return 11;
		}
		else
		{
				if(pos)
				{
						pos_end=strsearch(GSMUART_buff,"\x0d\x0a\x0d\x0a");//第一条
				}
				if(pos&&pos_end)
				{
						ClearArray(GSMUART_buff+pos_end+3,MAX_GSMUART-pos_end-4);//清除，除了第一个短信之外的其他pdu数据
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						CoypArraryAtoArraryB(GSMUART_buff,CommonBuf,MAX_GSMUART);//把GSMUART_buff中的数据备份到CommonBuf中
						pos=strsearch(CommonBuf,"0D9168");//判断是否是国家号码标准的号码,即不同号码发送过来的入口
						if(!pos)
						{
								pos=strsearch(CommonBuf,"0BA1");//如果卡是联通卡，收到的正常的短信
								pos=pos-2;
						}
						if((pos)&&(pos < pos_end))
						{
								for(i = 0;i<12;i++)N_ReceNum[i]=CommonBuf[pos+i+5];//将号码存进ReceNum
								PhoneNumRecover(N_ReceNum);//将号码恢复
								if(check_includs_num(N_ReceNum))//判断是否有权限的用户
								{
										CheckSmsCmd(CommonBuf,N_ReceNum);//解析数据
										if(IsSMS==1)SMSCMD(Administrator_num_Flag,N_ReceNum);//根据权限处理数据，判断发送什么数据回去
										DeleteAllSMS();
										return 0;//成功获取并处理后删除短信
								}
								return 5;//不是权限用户发来的，删除短信
						}
						return 4; 
				}
				return 4;
		}
}

//*****************************************
//描述:删除指定index的短信
//参数:index:存放index的地址
//返回: 0:成功删除 1:index错误 2:删除失败 
u8 DetelIndexSms(u8 *index)
{
    clear_USART1buff();
    if((*(index+0))&&((*(index+1))==0))//判断发送的index的1位数 还是 2位数,删除接收的短信
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
//描述：用于接收短信的时候，恢复号码,把获取的号码奇偶互换，且把F去掉
//参数：*Num :存放回复的号码
//返回：0:成功   1:数组没有号码
u8 PhoneNumRecover(u8 *Num)
{
		u8 Len=0;
		u8 i=0;
		u8 temp=0;
		Len=CalculateLen(Num);//获取号码数组的有效长度
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
		else return 1;//有新的短息进入,但又找不到CMTI 
		return 0;
}

//***********************************************************************
//描述:从指定的buf中获取有效的余额信息
//参数:pdata :存放信息的buf(已经是短信内容)
//返回:0:获取成功  1:错误的字符串
u8 GetSimBalanceForBuf(u8 *pdata)
{
		BaseType_t err;
		u16 i=0,pos1=0,pos2=0,pos3=0;//"尊敬"位置 "余额"位置 "元"位置
		u8 YEUnicode[64]={0},Queue_BUFF=0;    
		pos1 = strsearch(pdata,"5C0A656C");//"尊敬",判断是否有这个关键字
		if(pos1)                //存在"尊敬",且尊敬是位于最前面. 
		{
				pos2=strsearch(pdata,"4F59989D");      //"余额" ，判断是否有这个关键字
				if(pos2)          //在30个字符内需要出现"余额"这个关键字
				{
						pos3 = strsearch(pdata,"5143");       //"元",判断是否有这个关键字
						if(pos3)           //在30个字符内需要出现"元"这个关键字
						{
								if((*(pdata+pos3-5) == '0')&&(*(pdata+pos3-4) == '0'))//"元"关键字前一定是 数字
										goto save;
						}
				}
		}
		return 1;
save:
		CoypArraryAtoArraryB_LR(pdata,YEUnicode,pos3-pos2+4,pos2-1);//复制指定数组
		FLASH_Unlock(); 
		MyEE_WriteWord(BalanceLenAdd,pos3-pos2+4);//写余额长度Flash
		for(i=0;i<(pos3-pos2+4);i++)MyEE_WriteWord(BalanceTextAdd+i,YEUnicode[i]);//写余额内容Flash
		FLASH_Lock();
		Queue_BUFF=POST_HTTP_SEFT;
		err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//上传余额
		return 0;
}

//***********************************************************************
//描述:从指定的buf中获取有效的手机号码
//参数:pdata :存放信息的buf(已经是短信内容)
//返回:0:获取成功  1:错误的字符串
u8 GetSimNumForBuf(u8 *pdata)
{
		u8 Queue_BUFF=0;
		u16 i=0,pos1=0;
		u8 NumUnicode[64]={0}; 
		BaseType_t err;
		pos1=strsearch(pdata,"53F778014E3A");//"号码为",判断是否有这个关键字
		if(pos1)//存在"号码为",且号码为是位于最前面. 
				goto Numsave;
		return 1;
Numsave:
		CoypArraryAtoArraryB_LR(pdata,NumUnicode,44,pos1+12-1);//复制指定数组
		for(i=0;i<11;i++)NumBer[i]=NumUnicode[i*4+3];//转化号码到数组
		Queue_BUFF=POST_HTTP_Num;
		err=xQueueSend(GSM_Message_Queue,&Queue_BUFF,portMAX_DELAY);//上传号码
		return 0;
}


//***************************************************************************************
//描述:根据输入的unicode码(直接从CommonBuf中获取)，以及号码,发送短信
//参数:pUnicode:unicode码存放地址 Phnum:需要发送的号码
//返回: 0:发送成功 1:输入的Unicode字符串不正确 2:发送的号码长度不正确  3:没有返回CMGS OK,发送错误
u8 pdu_send_start(u8 *PhNum)
{
    u16 context_len=0,i=0,phlen=0,try_cnt=0;
    u8 SmsAmount = 0;//发送短信的条数
    u16 UnicodeActualLen=0,ThisUnicodeLen=0,ThisStartPos=0;//unicode的真实长度 这次需要发送的Unicode的长度 这次需要发送的Unicode的开始位置
	  u16 Random=11;//随机数,用于生成唯一识别标志   
    if(!check_sms_unicode(CommonBuf,COMMONBUFMAX))//如果返回的是0则不进入发送, 因为unicode码错误
    {
				ClearCommonBuf();
				return 1; 
    }
    UART_string_newline("AT+CMGF=0",1);
    clear_USART1buff();
		delay_ms(300);
    phlen=CalculateLen(PhNum);//获取发送的号码的长度
    if((phlen!=11)&&(phlen!=5))return 2;      
    UnicodeActualLen=CalculateLenFromEnd(CommonBuf,COMMONBUFMAX);//获取unicode码长度
    if(UnicodeActualLen <= SMSUnitLen)      //判断需要几条短信
      SmsAmount=1;//1条内可以发送
    else if(UnicodeActualLen <= SMSUnitLen*2)
      SmsAmount=2;//2条内可以发送
    else if(UnicodeActualLen <= SMSUnitLen*3)
      SmsAmount=3;//3条内可以发送 
    else if(UnicodeActualLen <= SMSUnitLen*4)
      SmsAmount=4;//4条内可以发送 
    else
      return 1;//输入的Unicode码不正确 
    for(i=0;i<SmsAmount;i++)
    {
				if((SmsAmount==1)&&(i==0))//1条短信
				{
						context_len=UnicodeActualLen;
						ThisUnicodeLen=UnicodeActualLen;
						ThisStartPos=0;  
				}
				else if(((SmsAmount==2)||(SmsAmount==3)||(SmsAmount==4))&&(i==0))//N条短信第1次发送
				{
						context_len=SMSUnitLen+12;
						ThisUnicodeLen=SMSUnitLen;
						ThisStartPos=0;
				}
				else if(((SmsAmount==3)||(SmsAmount==4))&&(i==1))//N条短信第2次发送
				{
						context_len=SMSUnitLen+12;
						ThisUnicodeLen=SMSUnitLen;
						ThisStartPos=SMSUnitLen;	
				}
				else if((SmsAmount==2)&&(i==1))//两条短信第2次发送
				{
						context_len=UnicodeActualLen-SMSUnitLen+12;
						ThisUnicodeLen=UnicodeActualLen-SMSUnitLen;
						ThisStartPos=SMSUnitLen; 
				}
				else if(((SmsAmount==4))&&(i==2))//N条短信第3次发送
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
				else if((SmsAmount==4)&&(i==3))//N条短信第4次发送
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
				if(SmsAmount == 1)//发送是否需要报头
					UART_string("0011",1);
				else if((SmsAmount == 2)||(SmsAmount == 3)||(SmsAmount == 4))
					UART_string("0051",1); 
				if(phlen == 5)            
					UART_string("0005A1",1);
				else if(phlen == 11)      
					UART_string("000D9168",1);
				UART_string(num_change(PhNum),1);//发送经过奇偶互换的号码
				UART_string("000800",1);//有效期为00 则应该是5分钟
				UART_string(hextoasciihex(context_len/2),1);  
				if(SmsAmount != 1)//如果是需要拆分的短信，才需要这部分
				{	
						UART_string("050003",1);//固定 
						UART_string(hextoasciihex(Random),1);//随机编号，通过时间+10进行
						UART_string(hextoasciihex(SmsAmount),1);//短信总条数
						UART_string(hextoasciihex(i+1),1);//这是第N条
				}
				UART_string_amount(CommonBuf+ThisStartPos,ThisUnicodeLen,1);   //发送数据
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
    return 0;//短信发送成功

}

//**************************************************************
//描述：用于检测生成的unicode码的位数以及字符串是否正确，注意，这里计算长度是通过从未端开始检测知道检测到数据
//参数：*pdata   :需要检测的用于存放unicode码的数组 ，max_len :该数组的最大长度
//返回: 1：正确 0:不正确
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
//描述:根据Commonbuf中的数据发送短信到指定的号码
//参数:TargetNum :目标号码
//返回:0:成功  1:完全失败  2:其他原因的失败
u8 SendSmsByCommonbuf(u8 *TargetNum)
{
	u8 res=0,err=0,i=0;
	for(i = 0;i<2;i++)
	{
			res=pdu_send_start(TargetNum);
			if(res == 0)
					return 0;
			else if(res ==3)//因网络原因失败
			{
					err++;//本次发送失败
					if(err==2)
							return 1;
			}
			else 
					return 2;//其他原因失败 
			delay_ms(100);
	 }
	 return 0;
}

//**********************************
//描述:发送查询余额指令
void SendGetBalanceSms(void)
{
   ClearCommonBuf();
   if(SimOperator==ChinaMobile)//如果是移动
      AsciiToUnicodeAppendCommonBuf("YE");
   else if(SimOperator==ChinaUnicom)//如果是联通
      AsciiToUnicodeAppendCommonBuf("YE"); 
   if(SimOperator==ChinaMobile)
			SendSmsByCommonbuf("10086");
   else if(SimOperator==ChinaUnicom)
			SendSmsByCommonbuf("10010");
    ClearCommonBuf();
}

//**********************************
//描述:发送查询号码指令
void SendGetNumSms(void)
{
    ClearCommonBuf();
    if(SimOperator == ChinaMobile)//如果是移动
       AsciiToUnicodeAppendCommonBuf("BJ");
    else if(SimOperator == ChinaUnicom)//如果是联通
       AsciiToUnicodeAppendCommonBuf("CXHM"); 
    if(SimOperator == ChinaMobile)
			 SendSmsByCommonbuf("10086");
    else if(SimOperator == ChinaUnicom)
			 SendSmsByCommonbuf("10010");
		ClearCommonBuf();
}

//**************************************************************************************
//描述:把ascii转为unicode码，并且添加到CommonBuf中
//参数:src:需要转换的字符串
//返回:
void AsciiToUnicodeAppendCommonBuf(u8 *src)
{
		u16 curLen = 0;
		curLen=CalculateLenFromEnd(CommonBuf,COMMONBUFMAX);
		AScSwitchUni(src,CommonBuf+curLen);
}

/***************************************************************************************
//函数名称      AScSwitchUni
//函数功能      把整数转化成Uni
//入口函数      *p 指向的数组  *p2
//出口函数      偏移度 m
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
//检测当前的sms 用户是否是权限用户
//返回0，非有权限用户，1是，1以上，为该用户的存放顺序位置
u8 check_includs_num(u8 *cur_num)
{
		u8 check_i=0,check_j=0,num_flag=0;
		u8 cnt=0; 
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//先判断是否属于高级用户
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
						if(num_flag)//循环11次后找到相同的号码 
						{ 
								Administrator_num_Flag=1;//此号码为高级用户，管理员权限用户
								break;//查找到许可的号码
						}
				}
		}
		if(num_flag==0)
		{
				cnt=MyEE_ReadWord(MobileCntAdd);
				if(cnt!=0)
				{ 
						if(cnt>10)cnt=10;
						for(check_i=0;check_i<cnt;check_i++)//先判断是否属于低级用户
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
								if(num_flag)//循环11次后找到相同的号码 
								{ 
										Administrator_num_Flag=0;//此号码为低级用户
										break;//查找到许可的号码
								}
						}
				}
		}
		if(num_flag==1)num_flag=check_i+1;//如果查找到许可的号码则返回该号码的位置加1
		return num_flag;
}

//****************************************************************
//描述：判断接收回来的PDU数据，判断是否是有效的指令
//参数：*pdata		 *ReceNum:接收到的号码
//返回：0:成功把sms中的内容处理. 1:没有成功处理数据
u8 CheckSmsCmd(u8 *pdata,u8 *ReceNum)
{
		u16 pos=0,StartPos=0,DataLen=0,i=0;
		u8 Smsdatatemp[20]={0};
		for(i=0;i<20;i++)Smsdata[i]=0;
		pos=strsearch(pdata,"0D9168");
		if(!pos)
		{       
				pos=strsearch(pdata,"0BA1");//如果卡是联通卡，收到的正常的短信
				pos=pos-2;
		}
		if(pos)
		{
				DataLen=((*(pdata+pos+35)-'0')*10+*(pdata+pos+36)-'0') *2;
				StartPos=pos+37;//保存数据开始的位置   
				if(DataLen<=40)//如果数据小于16,则有可能是纯ascii的字符，发送过来的是7BIT编码
				{     
						HexStringToHex(pdata+StartPos,Smsdatatemp,DataLen);
						gsmDecode7bit(Smsdatatemp,(char*)Smsdata,DataLen/2+1);
						return 0;
				}
		}
		return 1;
}


//****************************************************************
//描述：向Flash存放手机号码，每个号码之间用 ， 间隔
//参数：*DATABUF 手机号码指针，flashmode 存放的是高级用户还是低级用户 1为高级 0为低级
void PhoneNumlist(u8 *DATABUF,u8 flashmode)
{
		u16 num_i=0,num_cnt=0;
		if(flashmode==0)//存放低级权限
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
		else if(flashmode==1)//存放高级权限
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
//描述：根据用户权限判断短信内容的指令并作下一步处理
//参数：admin_level 用户权限 1为高级权限 0为低级权限 *SEND_NUMBER_SMS为发送者的手机号码
void SMSCMD(u8 admin_level,u8 *SEND_NUMBER_SMS)
{
		u16 smscmd_i=0;
		u8 Queue_BUFF=0;
		BaseType_t err;
		static u8 last_cmd=0;//上一次指令的变量 1为自检，2为复位
		if((Smsdata[0]=='C'||Smsdata[0]=='c')&&(Smsdata[1]=='X'||Smsdata[1]=='x')&&(Smsdata[2]=='Z'||Smsdata[2]=='z')&&(Smsdata[3]=='T'||Smsdata[3]=='t'))//CXZT 查询状态
		{
				SMS_To_ZT();//探测器当前状态转成unicode并放在CommonBuf数组中
				if(admin_level==1)//高级用户专属
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(查询状态)；ZJ(自检)；FW(复位)
				else if(admin_level==0)//低级级用户专属
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(查询状态)
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//发送短信给当前手机号码
		}
		else if((Smsdata[0]=='Z'||Smsdata[0]=='z')&&(Smsdata[1]=='J'||Smsdata[1]=='j')&&admin_level==1)//ZJ 自检 高级用户专属
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
				UnicodeAppendCommonBuf("8bf7518d6b21786e8ba4662f54269700898181ea68c0ff0c");//请再次确认是否需要自检，
				UnicodeAppendCommonBuf("786e8ba48bf756de590d005900450053ff0c4e0d786e8ba48bf756de590d004e004f62164e245206949f51854e0d56de590d6b648bf76c4259316548");//确认请回复YES，不确认请回复NO或两分钟内不回复此请求失效
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//发送短信给当前手机号码
				strcpy((char*)last_NUMBER_BUF,(char*)SEND_NUMBER_SMS);//将当前用户号码保存
				sms_cmd_time_start=1;//开始计时
				last_cmd=1;//记录当前用户的指令
		}
		else if((Smsdata[0]=='F'||Smsdata[0]=='f')&&(Smsdata[1]=='W'||Smsdata[1]=='w')&&admin_level==1)//FW 复位 高级用户专属
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
				UnicodeAppendCommonBuf("8bf7518d6b21786e8ba4662f542697008981590d4f4dff0c");//请再次确认是否需要复位，
				UnicodeAppendCommonBuf("786e8ba48bf756de590d005900450053ff0c4e0d786e8ba48bf756de590d004e004f62164e245206949f51854e0d56de590d6b648bf76c4259316548");//确认请回复YES，不确认请回复NO或两分钟内不回复此请求失效
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//发送短信给当前手机号码
				strcpy((char*)last_NUMBER_BUF,(char*)SEND_NUMBER_SMS);//将当前用户号码保存
				sms_cmd_time_start=1;//开始计时
				last_cmd=2;//记录当前用户的指令
		}
		else if((Smsdata[0]=='Y'||Smsdata[0]=='y')&&(Smsdata[1]=='E'||Smsdata[1]=='e')&&(Smsdata[2]=='S'||Smsdata[2]=='s')&&admin_level==1&&strsearch(last_NUMBER_BUF,SEND_NUMBER_SMS)&&sms_cmd_time_start==1)//在两分钟内收到同一用户发来的yes
		{
				sms_cmd_time_start=0;
				if(last_cmd==1)//开始发送关于自检的相关任务
				{
						last_cmd=0;//清除指令记录
						NeedToSMS=1;//自检完成后需要发短信
						Queue_BUFF=W_Self_Check_Task;err=xQueueSend(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//向探测器发送自检任务
				}
				else if(last_cmd==2)//开始发送关于复位的相关任务
				{
						last_cmd=0;//清除指令记录
						NeedToSMS=1;//复位完成后需要发短信
						//保存到FLash的相关参数，待重启后发送短信
						FLASH_Unlock();
						for(smscmd_i=0;SEND_NUMBER_SMS[smscmd_i]!=0;smscmd_i++)MyEE_WriteWord(SMS_FW_NUM_Add+smscmd_i,SEND_NUMBER_SMS[smscmd_i]);//保存当前复位的手机号码到Flash
						FLASH_Lock();
						Queue_BUFF=W_RESET_Task;
						err=xQueueSend(FT_Message_Queue,&Queue_BUFF,portMAX_DELAY);//向探测器发送复位任务
				}
				
		}
		else if((Smsdata[0]=='N'||Smsdata[0]=='n')&&(Smsdata[1]=='O'||Smsdata[1]=='o')&&admin_level==1&&strsearch(last_NUMBER_BUF,SEND_NUMBER_SMS)&&sms_cmd_time_start==1)
		{
				sms_cmd_time_start=0;//清除计时
				last_cmd=0;//清除指令记录
		}
		else if(admin_level==1)//无意义操作，返回短信操作指令表,高级用户
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
				UnicodeAppendCommonBuf("65e0654863074ee4000d000a64cd4f5c63074ee4ff1a000d000a");//无效指令+换行+操作指令：+换行
				UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(查询状态)；ZJ(自检)；FW(复位)
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//发送短信给当前手机号码
		}
		else if(admin_level==0)//无意义操作，返回短信操作指令表,低级用户
		{
				ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
				UnicodeAppendCommonBuf("65e0654863074ee4000d000a64cd4f5c63074ee4ff1a000d000a00430058005a0054002867e58be272b660010029000d000a");//无效指令+换行操作指令：+换行CXZT(查询状态)+换行
				UnicodeAppendCommonBuf("598260f383b753d666f4591a64cd4f5c63074ee4ff0c8bf780547cfb516c53f87ba17406545883b753d667439650");//如想获取更多操作指令，请联系公司管理员获取权限
				SendSmsByCommonbuf(SEND_NUMBER_SMS);//发送短信给当前手机号码
		}
}

//****************************************************************
//描述：探测器当前状态转成unicode并放在CommonBuf数组中
void SMS_To_ZT(void)
{
		u16 Address_Len=0,smscmd_i=0;
		u8 Address_buf[200]={0};
		ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
		UnicodeAppendCommonBuf("63a26d4b56685f53524d72b660014fe1606f000d000a");//探测器当前状态信息+换行
		UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//归属公司：
		Address_Len=MyEE_ReadWord(SimNameLenAdd);
		for(smscmd_i=0;smscmd_i<200;smscmd_i++)
				Address_buf[smscmd_i]=0;
		for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
				Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
		UnicodeAppendCommonBuf(Address_buf);
		UnicodeAppendCommonBuf("000d000a");//换行
		UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//安装位置：
		Address_Len=MyEE_ReadWord(ADDRESSLenAdd);
		for(smscmd_i=0;smscmd_i<200;smscmd_i++)
				Address_buf[smscmd_i]=0;
		for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
				Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
		UnicodeAppendCommonBuf(Address_buf);
		UnicodeAppendCommonBuf("000d000a63a26d4b70b9ff1a68c06d4b503c002f62a58b669650503c000d000a");//换行+探测点：检测值/报警限值+换行
		UnicodeAppendCommonBuf("6f0f75356d41ff1a");//漏电流:
		if(FT_status.RTCM_LS==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Lcurrent));
				UnicodeAppendCommonBuf("006d0041002f");//mA/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_LcAvalue));
				UnicodeAppendCommonBuf("006d0041ff1b000d000a");//mA；+换行
		}
		UnicodeAppendCommonBuf("004176f875356d41ff1a");//A相电流:
		if(FT_status.RTCM_CSA==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Acurrent)); 
				UnicodeAppendCommonBuf("0041002f");//A/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_AcAvalue)); 
				UnicodeAppendCommonBuf("0041ff1b000d000a");//A；+换行
		}
		UnicodeAppendCommonBuf("004276f875356d41ff1a");//B相电流:
		if(FT_status.RTCM_CSB==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Bcurrent)); 
				UnicodeAppendCommonBuf("0041002f");//A/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_BcAvalue)); 
				UnicodeAppendCommonBuf("0041ff1b000d000a");//A；+换行
		}
		UnicodeAppendCommonBuf("004376f875356d41ff1a");//C相电流:
		if(FT_status.RTCM_CSC==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Ccurrent)); 
				UnicodeAppendCommonBuf("0041002f");//A/
				AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_CcAvalue)); 
				UnicodeAppendCommonBuf("0041ff1b000d000a");//A；+换行
		}
		UnicodeAppendCommonBuf("004176f86e295ea6ff1a");//A相温度:
		if(FT_status.RTCM_TS1==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Atemp));
				UnicodeAppendCommonBuf("5ea6002f");//度/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_AtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
		}
		UnicodeAppendCommonBuf("004276f86e295ea6ff1a");//B相温度:
		if(FT_status.RTCM_TS2==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Btemp));
				UnicodeAppendCommonBuf("5ea6002f");//度/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_BtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
		}
		UnicodeAppendCommonBuf("004376f86e295ea6ff1a");//C相温度:
		if(FT_status.RTCM_TS3==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Ctemp));
				UnicodeAppendCommonBuf("5ea6002f");//度/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_CtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
		}
		UnicodeAppendCommonBuf("73af58836e295ea6ff1a");//环境温度:
		if(FT_status.RTCM_TS4==Statue_NONE)UnicodeAppendCommonBuf("65e05b8988c5ff1b000d000a");//无安装；+换行
		else
		{
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Etemp));
				UnicodeAppendCommonBuf("5ea6002f");//度/
				AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_EtAvalue)); 
				UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
		}
		UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
}

//****************************************************************
//描述：向所有用户发送某用户进行自检操作指令的短信
void SMS_To_ALL_Of_ZJ(void)
{
		u8 sendnum_buf[11]={0},Address_buf[200]={0};
		u16 Address_Len=0,smscmd_i=0;
		u8 check_i=0,check_j=0,cnt=0;
		//先向高级用户发送短信
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//高级用户
				{
						for(check_j=0;check_j<11;check_j++)sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);	
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("75286237");//用户
						AsciiToUnicodeAppendCommonBuf(last_NUMBER_BUF);
						UnicodeAppendCommonBuf("5bf9");//对
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);//读取地址buf
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c81ea68c064cd4f5c5df25b8c6210000d000a");//的探测器进行自检操作已完成+换行
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(查询状态)；ZJ(自检)；FW(复位)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//低级用户
				{
						for(check_j=0;check_j<11;check_j++)sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("75286237");//用户
						AsciiToUnicodeAppendCommonBuf(last_NUMBER_BUF);
						UnicodeAppendCommonBuf("5bf9");//对
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);//读取地址buf
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c81ea68c064cd4f5c5df25b8c6210000d000a");//的探测器进行自检操作已完成+换行
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(查询状态)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
}

//****************************************************************
//描述：向所有用户发送某用户进行复位操作指令的短信
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
				for(check_i=0;check_i<cnt;check_i++)//高级用户
				{
						for(check_j=0;check_j<11;check_j++)
							 sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);	
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("75286237");//用户
						AsciiToUnicodeAppendCommonBuf(flash_num);
						UnicodeAppendCommonBuf("5bf9");//对
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//读取地址buf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c590d4f4d64cd4f5c5df25b8c6210000d000a");//的探测器进行复位操作已完成+换行
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(查询状态)；ZJ(自检)；FW(复位)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//低级用户
				{
						for(check_j=0;check_j<11;check_j++)
								sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("75286237");//用户
						AsciiToUnicodeAppendCommonBuf(flash_num);
						UnicodeAppendCommonBuf("5bf9");//对
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//读取地址buf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						UnicodeAppendCommonBuf("768463a26d4b56688fdb884c590d4f4d64cd4f5c5df25b8c6210000d000a");//的探测器进行复位操作已完成+换行
						UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(查询状态)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		FLASH_Unlock();
		MyEE_WriteWord(SMS_FW_Add,0x00);//写标志位
		FLASH_Lock();
}

//****************************************************************
//描述：向所有用户发送超限报警的短信
void SMS_To_ALL_Of_Alarming(void)
{
		u8 sendnum_buf[11]={0},Address_buf[200]={0};
		u8 check_i=0,check_j=0,cnt=0;
		u16 Address_Len=0,smscmd_i=0;
		//先向高级用户发送短信
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//高级用户
				{
						for(check_j=0;check_j<11;check_j++)
							 sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);						
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f8d85965062a58b66000d000a");//探测器发生超限报警+换行
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//归属公司：
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						UnicodeAppendCommonBuf("000d000a");//换行
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//安装位置：
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//读取地址buf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						SMS_Alarming();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT（查询状态）；ZJ（自检）；FW（复位）
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//低级用户
				{
						for(check_j=0;check_j<11;check_j++)
								sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f8d85965062a58b66000d000a");//探测器发生超限报警+换行
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//归属公司：
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						UnicodeAppendCommonBuf("000d000a");//换行
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//安装位置：
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//读取地址buf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						SMS_Alarming();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(查询状态)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
}

void SMS_Alarming(void)
{
				UnicodeAppendCommonBuf("000d000a62a58b6670b9ff1a62a58b66503c002f62a58b669650503c000d000a");//换行+报警点：报警值/报警限值+换行
				if(FT_status.RTCM_CSA==Status_Alarming)//A相电流超限
				{
						UnicodeAppendCommonBuf("004176f875356d41ff1a");//A相电流：
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Acurrent));
						UnicodeAppendCommonBuf("0041002f");//A/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_AcAvalue));
						UnicodeAppendCommonBuf("0041ff1b000d000a");//A；+换行
				}
				if(FT_status.RTCM_CSB==Status_Alarming)//B相电流超限
				{
						UnicodeAppendCommonBuf("004276f875356d41ff1a");//B相电流：
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Bcurrent));
						UnicodeAppendCommonBuf("0041002f");//A/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_BcAvalue));
						UnicodeAppendCommonBuf("0041ff1b000d000a");//A；+换行
				}
				if(FT_status.RTCM_CSC==Status_Alarming)//C相电流超限
				{
						UnicodeAppendCommonBuf("004376f875356d41ff1a");//C相电流：
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Ccurrent));
						UnicodeAppendCommonBuf("0041002f");//A/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_CcAvalue));
						UnicodeAppendCommonBuf("0041ff1b000d000a");//A；+换行
				}
				if(FT_status.RTCM_LS==Status_Alarming)//漏电流超限
				{
						UnicodeAppendCommonBuf("6f0f75356d41ff1a");//漏电流：
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Lcurrent));
						UnicodeAppendCommonBuf("006d0041002f");//mA/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_LcAvalue));
						UnicodeAppendCommonBuf("006d0041ff1b000d000a");//mA；+换行
				}
				if(FT_status.RTCM_TS1==Status_Alarming)//A相温度超限
				{
						UnicodeAppendCommonBuf("004176f86e295ea6503cff1a");//A相温度值：
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_Atemp));
						UnicodeAppendCommonBuf("5ea6002f");//度/
						AsciiToUnicodeAppendCommonBuf(U16IntToStr(FT_rtcm.R_RTCM_AtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
				}
				if(FT_status.RTCM_TS2==Status_Alarming)//B相温度超限
				{
						UnicodeAppendCommonBuf("004276f86e295ea6503cff1a");//B相温度值：
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Btemp));
						UnicodeAppendCommonBuf("5ea6002f");//度/
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_BtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
				}
				if(FT_status.RTCM_TS3==Status_Alarming)//C相温度超限
				{
						UnicodeAppendCommonBuf("004376f86e295ea6503cff1a");//C相温度值：
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Ctemp));
						UnicodeAppendCommonBuf("5ea6002f");//度/
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_CtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
				}
				if(FT_status.RTCM_TS4==Status_Alarming)//环境温度超限
				{
						UnicodeAppendCommonBuf("73af58836e295ea6503cff1a");//环境温度值：
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_Etemp));
						UnicodeAppendCommonBuf("5ea6002f");//度/
						AsciiToUnicodeAppendCommonBuf(S16IntToStr(FT_rtcm.R_RTCM_EtAvalue));
						UnicodeAppendCommonBuf("5ea6ff1b000d000a");//度；+换行
				}
				UnicodeAppendCommonBuf("53ef80fd5b5857285b895168969060a3ff0c8bf753ca65f6639267e5ff0c4ee59632706b707eff01000d000a");//可能存在安全隐患，请及时排查，以防火灾！+换行
				UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
}

//****************************************************************
//描述：向所有用户发送断线故障报警的短信
void SMS_To_ALL_Of_BrokenAndShort(void)
{
		u8 sendnum_buf[11]={0},Address_buf[200]={0};
		u8 check_i=0,check_j=0,cnt=0;
		u16 Address_Len=0,smscmd_i=0;
		//先向高级用户发送短信
		cnt=MyEE_ReadWord(WhiteListCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//高级用户
				{
						for(check_j=0;check_j<11;check_j++)
							 sendnum_buf[check_j]=MyEE_ReadWord(WhiteListAdd+check_j+check_i*SizeOfValidPh);	
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f6545969c62a58b66000d000a");//探测器发生故障报警+换行
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//归属公司：
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						UnicodeAppendCommonBuf("000d000a");//换行
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//安装位置：
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//读取地址buf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						SMS_Broken_Short();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029ff1b005a004a002881ea68c00029ff1b004600570028590d4f4d0029");//CXZT(查询状态)；ZJ(自检)；FW(复位)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
		cnt=MyEE_ReadWord(MobileCntAdd);
		if(cnt!=0)
		{
				if(cnt>10)cnt=10;
				for(check_i=0;check_i<cnt;check_i++)//低级用户
				{
						for(check_j=0;check_j<11;check_j++)
								sendnum_buf[check_j]=MyEE_ReadWord(MobileAdd+check_j+check_i*SizeOfValidPh); 
						ClearArray(CommonBuf,COMMONBUFMAX);//首先清除CommonBuf
						UnicodeAppendCommonBuf("63a26d4b566853d1751f6545969c62a58b66000d000a");//探测器发生故障报警+换行
						UnicodeAppendCommonBuf("5f525c5e516c53f8ff1a");//归属公司：
						Address_Len=MyEE_ReadWord(SimNameLenAdd);
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)
								Address_buf[smscmd_i]=MyEE_ReadWord(SimNameAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的归属公司
						UnicodeAppendCommonBuf("000d000a");//换行
						UnicodeAppendCommonBuf("5b8988c54f4d7f6eff1a");//安装位置：
						Address_Len=MyEE_ReadWord(ADDRESSLenAdd);//读取地址buf长度
						for(smscmd_i=0;smscmd_i<200;smscmd_i++)
								Address_buf[smscmd_i]=0;
						for(smscmd_i=0;smscmd_i<Address_Len;smscmd_i++)//读取地址buf
								Address_buf[smscmd_i]=MyEE_ReadWord(ADDRESSAdd+smscmd_i);
						UnicodeAppendCommonBuf(Address_buf);//探测器的安装位置
						SMS_Broken_Short();
						UnicodeAppendCommonBuf("00430058005a0054002867e58be272b660010029");//CXZT(查询状态)
						SendSmsByCommonbuf(sendnum_buf);
				}
		}
}

//****************************************************************
//描述：向所有用户发送短路故障报警的短信
void SMS_Broken_Short(void)
{
		UnicodeAppendCommonBuf("000d000a62a58b6670b9ff1a6545969c7c7b578b000d000a");//换行+报警点：故障类型+换行
		if(FT_status.RTCM_LS==Status_Broken)
		{
				UnicodeAppendCommonBuf("6f0f75356d41ff1a65ad7ebf6545969cff1b000d000a");//漏电流：断线故障；+换行
		}
		else if(FT_status.RTCM_LS==Status_Short)
		{
				UnicodeAppendCommonBuf("6f0f75356d41ff1a77ed8def6545969cff1b000d000a");//漏电流：短路故障；+换行
		}
		if(FT_status.RTCM_TS1==Status_Broken)
		{
				UnicodeAppendCommonBuf("004176f86e295ea6ff1a65ad7ebf6545969cff1b000d000a");//A相温度：断线故障；+换行
		}
		else if(FT_status.RTCM_TS1==Status_Short)
		{
				UnicodeAppendCommonBuf("004176f86e295ea6ff1a77ed8def6545969cff1b000d000a");//A相温度：短路故障；+换行
		}
		if(FT_status.RTCM_TS2==Status_Broken)
		{
				UnicodeAppendCommonBuf("004276f86e295ea6ff1a65ad7ebf6545969cff1b000d000a");//B相温度：断线故障；+换行
		}
		else if(FT_status.RTCM_TS2==Status_Short)
		{
				UnicodeAppendCommonBuf("004276f86e295ea6ff1a77ed8def6545969cff1b000d000a");//B相温度：短路故障；+换行
		}
		if(FT_status.RTCM_TS3==Status_Broken)
		{
				UnicodeAppendCommonBuf("004376f86e295ea6ff1a65ad7ebf6545969cff1b000d000a");//C相温度：断线故障；+换行
		}
		else if(FT_status.RTCM_TS3==Status_Short)
		{
				UnicodeAppendCommonBuf("004376f86e295ea6ff1a77ed8def6545969cff1b000d000a");//C相温度：短路故障；+换行
		}
		if(FT_status.RTCM_TS4==Status_Broken)
		{
				UnicodeAppendCommonBuf("73af58836e295ea6ff1a65ad7ebf6545969cff1b000d000a");//环境温度：断线故障；+换行
		}
		else if(FT_status.RTCM_TS4==Status_Short)
		{
				UnicodeAppendCommonBuf("73af58836e295ea6ff1a77ed8def6545969cff1b000d000a");//环境温度：短路故障；+换行
		}
		UnicodeAppendCommonBuf("53ef80fd5b5857285b895168969060a3ff0c8bf753ca65f6639267e5ff0c4ee59632706b707eff01000d000a");//可能存在安全隐患，请及时排查，以防火灾！+换行
		UnicodeAppendCommonBuf("64cd4f5c63074ee4ff1a000d000a");//操作指令：+换行
}

