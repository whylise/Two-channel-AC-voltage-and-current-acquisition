#include "crc16.h"

extern u8 CommonBuf[COMMONBUFMAX];

//描述:获取数据的CRC16校验值
//参数:*ptr数组指针 len数据长度
//返回:CRC16校验值
u16 get_crc16_value(u8 *ptr,u16 len)
{
	#define CRC16_DATA 0xa001 //CRC16校验值
	u16 i,flag,value;
	u8 j;
	value=0xffff;
	for(i=0;i<len;i++)
	{
		value=*ptr^value;
		for(j=0;j<8;j++)
		{
			flag=value&0x0001;
			value=value>>1;
			if(flag)value=value^CRC16_DATA;
		}
		*ptr++;
	}
	return value;
}

//*************************************************
//描述:将以字符串表示的十进制转成十进制，最大32位
//参数:数组
//返回:32位的十进制
u32 stingcnt_to_uint32(u8 *ptr)
{
		u32 x=0,y=0;
		u8 i=0;
		for(x=i=0;ptr[i];i++)
		{
				y=ptr[i]-'0';
				x*=10;
				x+=y;
		}
		return x;
}

//*************************************************
//描述:将以字符串表示的十进制转成十进制，最大32位
//参数:数组
//返回:32位的十进制
s32 stingcnt_to_sint32(u8 *ptr)
{
		s32 x=0,y=0;
		u8 i=0;
		if(ptr[0]=='-')
		{
				for(x=0,i=1;ptr[i];i++)
				{
						y=ptr[i]-'0';
						x*=10;
						x+=y;
				}
				x*=-1;
		}
		else
		{
				for(x=i=0;ptr[i];i++)
				{
						y=ptr[i]-'0';
						x*=10;
						x+=y;
				}
		}
		return x;
}

//*************************************************
//描述:计算按当前使用的波特率的1个字符的通讯时间
//参数:
//返回: 返回1个字符的通信时间Nms	(最少是1ms)
u32 SysDeviceBaud=115200;
u8 CalCurrentBaud1Char(void)
{
  u8 time = 0;
  time=11000/SysDeviceBaud + 1 ;	   //+1 是为了进行强制进1的整数，如  5.1 转为 6   0.5 转为 1
  return time;
} 

//*************************************************
//描述:从p1中查找p2字符串位置
//参数:字符串p1,p2
//注意:p1中不能空格0x00 
//返回:返回位置pos
u16 strsearch(const u8 *p1,const u8 *p2)
{
		u16 p1_len=0,p2_len=0;
		u16 i=0,j=0,k=0;
		u16 pos=0;
		if(!*p2)return 0;
		p1_len=strlen((char const *)p1);
		p2_len=strlen((char const *)p2);
		for(i=0;i<p1_len;i++)
		{
			if(*(p1+i)==*(p2))
			{
				for(k=i,j=0;j<p2_len;k++,j++)
				{
					if(*(p1+k)==*(p2+j))
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

//**************************************
//描述:10进制的字符串转为int
//return 65535 :返回错误, int :返回转换结果
//参数:*p :字符串地址,同时是开始地址,len,读取字符串的位数长度
u16 string_cint(u8 *p,u8 len)
{
  u8 j=0;
  u8 i=0;
  u16 data = 0;
  for(j=0;*(p+j)!=0x00;j++) //j在此用以记录字符串长度
  {
    if(j>=10)
      break;
  }
  if(j<len) len=j;
  if(len<=0) return 65535;//输入长度不正确
  if(len>=5) 
  {
    len=5;
    if(*p>0x36) return 65535;//超出u16最大范围
    if(*(p+1)>0x35) return 65535;//超出u16最大范围
    if(*(p+2)>0x35) return 65535;//超出u16最大范围
    if(*(p+3)>0x33) return 65535;//超出u16最大范围
    if(*(p+4)>0x34) return 65535;//超出u16最大范围
  }
  for(;i<len;i++)
  {
    if(*(p+i)==0x00) return data; 
      if((*(p+i))>=0x30&&(*(p+i))<=0x39)
      {
        u16 x=1;
        for(j=0;j<len-i-1;j++) x=x*10;
        data=data+(*(p+i)-0x30)*x;        
      }
      else  return 65535;//字符串中包含非数字符号
  }
  return data;
}

//***************************************
//描述：清除数组上的数据
//参数: Pdata地址 Len数组长度
//返回：无
void ClearArray(u8 *Pdata,u16 Len)
{
		u16 i=0;
		for(i=0;i<Len;i++)
			*(Pdata+i) = 0;
}

//***********************************************
//描述:复制数组重A中复制到B中
void CoypArraryAtoArraryB(const u8 *Pa, u8 *Pb,u16 num)
{
		u16 i = 0;
		for(i = 0;i<num ;i++)
			*(Pb+i)	= *(Pa+i);
}

//**********************************************
//描述：计算数组的有效长度，从开始到空为止
//参数：无
//return：长度
u16 CalculateLen(const u8 * Pdata)
{
		u16 i=0;
		while(*(Pdata+i))i++;
		return i;
}

//*****************************************
//描述："123456" 转为 0x12,0x34,0x56
//参数：Pdata :输入字符串地址  ,ptag :输出数组地址 ,输入数据长度
//返回：0:成功  1:长度不正确 2: 输入的字符串含其他字符,不正确
u8 HexStringToHex(u8 *pdata,u8 *ptag,u16 len)
{
		u16 i=0;
		s32 temp=0;
		if((len%2==0)&&(len))
		{
			for(i=0;i<len/2;i++)
			{
				temp=HexStringCint((pdata+i*2),2);
				if(temp!=-1)*(ptag+i)=temp;
				else return 2;
			}
			return 0;
		}
		else return 1;
}

//**************************************
//string_cint : 16进制的字符串转为int
//para: *p :字符串地址,同时是开始地址,u8 len,读取字符串的位数长度
//return :-1 :转换失败 n:返回正确结果整数：0~ 65535
s32 HexStringCint(u8 *p,u8 len)
{
  u16 ncode[]={0x01,0x10,0x100,0x1000};
  u16 data=0,temp=0;
  u8 i=0;
  for(i=0;*(p+i)!=NULL;i++) //j在此用以记录字符串长度
  {
			if(i>10)
			{
					i=10;
					break;
			}
  }
  if(i<len)len=i;
  if(len<=0);//输入长度不正确 
  if(len>=4)len=4;
  for(i=0;i<len;i++)
  {
			if((*(p+i) >= 0x30)&&(*(p+i) <= 0x39))
			{
				 temp = ncode[len-i-1];
				 data += (*(p+i)-'0')*temp; 
			}
			else if((*(p+i) >= 0x41)&&(*(p+i) <= 0x46))
			{
				 temp = ncode[len-i-1];
				 data += (*(p+i)-0x37)*temp;
			}
			else if((*(p+i) >= 0x61)&&(*(p+i) <= 0x66))
			{
				 temp = ncode[len-i-1];
				 data += (*(p+i)-0x57)*temp;
			}
			else return -1;
  }
  return data;
}

//*********************************************************************
//描述：计算数组中的有效数据长度，从未开始知道检测到数据为止
//参数：*Pdata :数组开始地址 ,MaxLen:数组定义的大小
//返回: len
u16 CalculateLenFromEnd(const u8 *Pdata,u16 MaxLen)
{
		u16 i=0;
		for(i=MaxLen;i>0;i--)
		{
			if(*(Pdata+i-1)!=0x00)
				return i;
		}
		return 0;
}

//*******************************************
//将一个整形数据（0-65535）转化为字符串
//data：一个无符号整形数据（0-65535）
//Str_Int2str：返回值为一个字符串指针(数组名)
u8 *U16IntToStr(u16 data)
{
		u8 i,k=0,n;
		u16 temp;
		u8 vl[5],j ; // 最大的也就是65535这个数（对于uint）;
		static u8	Str_Int2str[5];
		u8 CHAR_MAP_G[10] = {'0','1','2','3','4','5','6','7','8','9'};//只读，字符对照表
		u16 dv[] = {10000,1000,100,10,1};//只读，码表
    Str_Int2str[0] =0;
    Str_Int2str[1] =0;
    Str_Int2str[2] =0;
    Str_Int2str[3] =0;
    Str_Int2str[4] =0;        
		temp = data;
    if(temp==0)
    { 
				Str_Int2str[0]='0';	 Str_Int2str[1]='\0';	//如果是零，直接转换，返回
				return  Str_Int2str;
		}
    else  
	  { 
        for(i=0;i<5;i++)// 取出每位上的数字存放在vl[]
		    {
					 j= temp/dv[i];
					 temp  = temp%dv[i];
					 vl[i] = j;	
		    }
		}
	  i=0;// i又置为0，因为下面要从最高位开始显示；
		while((vl[i]==0)&&(i<5))// 检测，第一个非零元素的下标					 
			i++;
		k=i;// 此时已经是非0的数字了，取下标；		
		for(i=k,n=0; (i<5)&(n<5-k); i++,n++)
			Str_Int2str[n] = CHAR_MAP_G[vl[i]];// 总共有5-k个非0元素了哈。把非0元素全部弄到disvl[n]中去了
   	Str_Int2str[n]='\0';					//n+1送字符结束符
    return  Str_Int2str;
}

//*******************************************
//将一个整形数据（-32768-32768）转化为字符串
//data：一个有符号整形数据（-32768-32768）
//new_str：返回值为一个字符串指针(数组名)
u8 *S16IntToStr(s16 data)
{
		static u8 new_str[10]={0};
		u8 i,k=0,n,f_bit=0;
		u16 temp;
		s16 temp_1=0;
		u8 vl[5],j ; // 最大的也就是65535这个数（对于uint）;
		static u8	Str_Int2str[5];
		u8 CHAR_MAP_G[10] = {'0','1','2','3','4','5','6','7','8','9'};//只读，字符对照表
		u16 dv[] = {10000,1000,100,10,1};//只读，码表
		for(i=0;i<10;i++)new_str[i]=0;
		new_str[0]='-';
    Str_Int2str[0] =0;
    Str_Int2str[1] =0;
    Str_Int2str[2] =0;
    Str_Int2str[3] =0;
    Str_Int2str[4] =0; 
		temp_1=data;
		if(temp_1<0)
		{
			temp_1=temp_1*-1;
			f_bit=1;
		}
		temp = temp_1;
    if(temp==0)
    { 
				Str_Int2str[0]='0';	 Str_Int2str[1]='\0';	//如果是零，直接转换，返回
				return  Str_Int2str;
		}
    else  
	  { 
        for(i=0;i<5;i++)// 取出每位上的数字存放在vl[]
		    {
					 j= temp/dv[i];
					 temp  = temp%dv[i];
					 vl[i] = j;	
		    }
		}
	  i=0;// i又置为0，因为下面要从最高位开始显示；
		while((vl[i]==0)&&(i<5))// 检测，第一个非零元素的下标					 
			i++;
		k=i;// 此时已经是非0的数字了，取下标；		
		for(i=k,n=0; (i<5)&(n<5-k); i++,n++)
			Str_Int2str[n] = CHAR_MAP_G[vl[i]];// 总共有5-k个非0元素了哈。把非0元素全部弄到disvl[n]中去了
   	Str_Int2str[n]='\0';					//n+1送字符结束符
		if(f_bit==1)
		{
			strcat((char *)new_str,(char *)Str_Int2str);
			return  new_str;
		}
		else
			return  Str_Int2str;
}

/***************************************************************************************
//函数名称      hextoasciihex 把u8整数,转为16进制的字符串 如: 12-> "0C"
//函数功能      u8 h:需要转换的数
//入口函数      所需整数,返回的数组
//出口函数      无
***************************************************************************************/
u8* hextoasciihex(u8 h)
{
		unsigned char a=0;
		unsigned char b=0;
		static u8 hex[3] ={0};			//定义转换缓存
		hex[0] =0;
		hex[1] =0;
		hex[2] =0;
		a=h/16%16;
		b=h%16;
		if(a<10)
			hex[0]=a+0x30;
		else
			hex[0]=a+0x37;
		if(b<10)
			hex[1]=b+0x30;
		else
			hex[1]=b+0x37;
		return hex;
}

//***************************************************************************************
//函数名称      num_change
//函数功能      发送的号码奇偶互换
//入口函数      所需互换的号码 ,号码长度为11
//描述: 把需要号码奇偶互换，如果不是二位数的则补个F再进行互换
//参数: p:输入号码字符串的地址 
u8 *num_change(u8 *ph)
{
		unsigned char i = 0;
		unsigned char temp = 0;
		unsigned char len = 0;                //号码长度
		u8 static PhNum[15];
		ClearArray(PhNum,15);
		len = CalculateLen(ph);
		for(i = 0; i<len;i++)                 //复制
		{
			PhNum[i] = *(ph+i);
		}
		if(len %2 != 0)     //如果是奇数,则在最后补回1个F
		{
			PhNum[len] = 'F';
			len++;
		}
	 for(i = 0;i<len/2;i++)       //互换
	 {
			temp=PhNum[2*i+1];
			PhNum[2*i+1]=PhNum[2*i];
			PhNum[2*i]=temp;
	 }
	 return PhNum;
}

//***************************************
//描述：数字转换16进制
//参数：n ：单位整数转为单位字符
//返回:输出字符
u8 HexConvernum(u8 n)
{  
  if(n<10)
    return (n+'0');
  else if(n < 16)
    return (n + 0x37);
  else
    return 0;
}

//***********************************************
//描述:复制数组A中复制到B中,指定复制内容
//参数:PA=A,PB=B,num复制多少个，LR为开始地址
void CoypArraryAtoArraryB_LR(const u8 *Pa,u8 *Pb,u16 num,u16 LR)
{
		u16 i=0;
		for(i=0;i<num ;i++)
		{
			*(Pb+i)	= *(Pa+i+LR);
		}
}

//7bit解码   
//输入:pSrc-源编码串指针   nSrcLength-源编码串长度
//输出:pDst-目标字符串指针   
//返回:目标字符串长度   
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)   
{   
    int nSrc; //   源字符串的计数值   
    int nDst; //   目标解码串的计数值   
    int nByte; //   当前正在处理的组内字节的序号，范围是0-6   
    unsigned   char   nLeft; //   上一字节残余的数据   
    //计数值初始化   
    nSrc   =   0;   
    nDst   =   0;   
    //组内字节序号和残余数据初始化   
    nByte   =   0;   
    nLeft   =   0;   
    //将源数据每7个字节分为一组，解压缩成8个字节   
    //循环该处理过程，直至源数据被处理完   
    //如果分组不到7字节，也能正确处理   
    while(nSrc<nSrcLength)   
    {   
			//将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节   
      *pDst   =   ((*pSrc   <<   nByte)   |   nLeft)   &   0x7f;   
      //将该字节剩下的左边部分，作为残余数据保存起来   
      nLeft   =   *pSrc   >>   (7-nByte);   
      //   修改目标串的指针和计数值   
      pDst++;   
      nDst++;   
      //   修改字节计数值   
      nByte++;   
      //   到了一组的最后一个字节   
      if(nByte   ==   7)   
      {   
      //   额外得到一个目标解码字节   
      *pDst   =   nLeft;   
      //   修改目标串的指针和计数值   
      pDst++;   
      nDst++;   
      //   组内字节序号和残余数据初始化   
      nByte   =   0;   
      nLeft   =   0;   
      } 
      //   修改源串的指针和计数值   
      pSrc++;   
      nSrc++;   
      }   
      //   输出字符串加个结束符   
      *pDst   =   '\0';   
      //   返回目标串长度   
      return   nDst;   
}

//**************************************************************************************
//描述:把现有unicode码,添加到CommonBuf中
//参数:src:Unicode 码源
//返回:
u8 UnicodeAppendCommonBuf(const u8 *src)
{
		u16 curLen=0,SrcLen=0,i=0;
		curLen=CalculateLenFromEnd(CommonBuf,COMMONBUFMAX);
		SrcLen=CalculateLen(src);
		if(SrcLen%4==0)
		{
			for(i=0;i<SrcLen;i++)
				CommonBuf[curLen+i] = *(src+i);
		}
		return 0;
}

