#include "crc16.h"

extern u8 CommonBuf[COMMONBUFMAX];

//����:��ȡ���ݵ�CRC16У��ֵ
//����:*ptr����ָ�� len���ݳ���
//����:CRC16У��ֵ
u16 get_crc16_value(u8 *ptr,u16 len)
{
	#define CRC16_DATA 0xa001 //CRC16У��ֵ
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
//����:�����ַ�����ʾ��ʮ����ת��ʮ���ƣ����32λ
//����:����
//����:32λ��ʮ����
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
//����:�����ַ�����ʾ��ʮ����ת��ʮ���ƣ����32λ
//����:����
//����:32λ��ʮ����
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
//����:���㰴��ǰʹ�õĲ����ʵ�1���ַ���ͨѶʱ��
//����:
//����: ����1���ַ���ͨ��ʱ��Nms	(������1ms)
u32 SysDeviceBaud=115200;
u8 CalCurrentBaud1Char(void)
{
  u8 time = 0;
  time=11000/SysDeviceBaud + 1 ;	   //+1 ��Ϊ�˽���ǿ�ƽ�1����������  5.1 תΪ 6   0.5 תΪ 1
  return time;
} 

//*************************************************
//����:��p1�в���p2�ַ���λ��
//����:�ַ���p1,p2
//ע��:p1�в��ܿո�0x00 
//����:����λ��pos
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
//����:10���Ƶ��ַ���תΪint
//return 65535 :���ش���, int :����ת�����
//����:*p :�ַ�����ַ,ͬʱ�ǿ�ʼ��ַ,len,��ȡ�ַ�����λ������
u16 string_cint(u8 *p,u8 len)
{
  u8 j=0;
  u8 i=0;
  u16 data = 0;
  for(j=0;*(p+j)!=0x00;j++) //j�ڴ����Լ�¼�ַ�������
  {
    if(j>=10)
      break;
  }
  if(j<len) len=j;
  if(len<=0) return 65535;//���볤�Ȳ���ȷ
  if(len>=5) 
  {
    len=5;
    if(*p>0x36) return 65535;//����u16���Χ
    if(*(p+1)>0x35) return 65535;//����u16���Χ
    if(*(p+2)>0x35) return 65535;//����u16���Χ
    if(*(p+3)>0x33) return 65535;//����u16���Χ
    if(*(p+4)>0x34) return 65535;//����u16���Χ
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
      else  return 65535;//�ַ����а��������ַ���
  }
  return data;
}

//***************************************
//��������������ϵ�����
//����: Pdata��ַ Len���鳤��
//���أ���
void ClearArray(u8 *Pdata,u16 Len)
{
		u16 i=0;
		for(i=0;i<Len;i++)
			*(Pdata+i) = 0;
}

//***********************************************
//����:����������A�и��Ƶ�B��
void CoypArraryAtoArraryB(const u8 *Pa, u8 *Pb,u16 num)
{
		u16 i = 0;
		for(i = 0;i<num ;i++)
			*(Pb+i)	= *(Pa+i);
}

//**********************************************
//�����������������Ч���ȣ��ӿ�ʼ����Ϊֹ
//��������
//return������
u16 CalculateLen(const u8 * Pdata)
{
		u16 i=0;
		while(*(Pdata+i))i++;
		return i;
}

//*****************************************
//������"123456" תΪ 0x12,0x34,0x56
//������Pdata :�����ַ�����ַ  ,ptag :��������ַ ,�������ݳ���
//���أ�0:�ɹ�  1:���Ȳ���ȷ 2: ������ַ����������ַ�,����ȷ
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
//string_cint : 16���Ƶ��ַ���תΪint
//para: *p :�ַ�����ַ,ͬʱ�ǿ�ʼ��ַ,u8 len,��ȡ�ַ�����λ������
//return :-1 :ת��ʧ�� n:������ȷ���������0~ 65535
s32 HexStringCint(u8 *p,u8 len)
{
  u16 ncode[]={0x01,0x10,0x100,0x1000};
  u16 data=0,temp=0;
  u8 i=0;
  for(i=0;*(p+i)!=NULL;i++) //j�ڴ����Լ�¼�ַ�������
  {
			if(i>10)
			{
					i=10;
					break;
			}
  }
  if(i<len)len=i;
  if(len<=0);//���볤�Ȳ���ȷ 
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
//���������������е���Ч���ݳ��ȣ���δ��ʼ֪����⵽����Ϊֹ
//������*Pdata :���鿪ʼ��ַ ,MaxLen:���鶨��Ĵ�С
//����: len
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
//��һ���������ݣ�0-65535��ת��Ϊ�ַ���
//data��һ���޷����������ݣ�0-65535��
//Str_Int2str������ֵΪһ���ַ���ָ��(������)
u8 *U16IntToStr(u16 data)
{
		u8 i,k=0,n;
		u16 temp;
		u8 vl[5],j ; // ����Ҳ����65535�����������uint��;
		static u8	Str_Int2str[5];
		u8 CHAR_MAP_G[10] = {'0','1','2','3','4','5','6','7','8','9'};//ֻ�����ַ����ձ�
		u16 dv[] = {10000,1000,100,10,1};//ֻ�������
    Str_Int2str[0] =0;
    Str_Int2str[1] =0;
    Str_Int2str[2] =0;
    Str_Int2str[3] =0;
    Str_Int2str[4] =0;        
		temp = data;
    if(temp==0)
    { 
				Str_Int2str[0]='0';	 Str_Int2str[1]='\0';	//������㣬ֱ��ת��������
				return  Str_Int2str;
		}
    else  
	  { 
        for(i=0;i<5;i++)// ȡ��ÿλ�ϵ����ִ����vl[]
		    {
					 j= temp/dv[i];
					 temp  = temp%dv[i];
					 vl[i] = j;	
		    }
		}
	  i=0;// i����Ϊ0����Ϊ����Ҫ�����λ��ʼ��ʾ��
		while((vl[i]==0)&&(i<5))// ��⣬��һ������Ԫ�ص��±�					 
			i++;
		k=i;// ��ʱ�Ѿ��Ƿ�0�������ˣ�ȡ�±ꣻ		
		for(i=k,n=0; (i<5)&(n<5-k); i++,n++)
			Str_Int2str[n] = CHAR_MAP_G[vl[i]];// �ܹ���5-k����0Ԫ���˹����ѷ�0Ԫ��ȫ��Ū��disvl[n]��ȥ��
   	Str_Int2str[n]='\0';					//n+1���ַ�������
    return  Str_Int2str;
}

//*******************************************
//��һ���������ݣ�-32768-32768��ת��Ϊ�ַ���
//data��һ���з����������ݣ�-32768-32768��
//new_str������ֵΪһ���ַ���ָ��(������)
u8 *S16IntToStr(s16 data)
{
		static u8 new_str[10]={0};
		u8 i,k=0,n,f_bit=0;
		u16 temp;
		s16 temp_1=0;
		u8 vl[5],j ; // ����Ҳ����65535�����������uint��;
		static u8	Str_Int2str[5];
		u8 CHAR_MAP_G[10] = {'0','1','2','3','4','5','6','7','8','9'};//ֻ�����ַ����ձ�
		u16 dv[] = {10000,1000,100,10,1};//ֻ�������
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
				Str_Int2str[0]='0';	 Str_Int2str[1]='\0';	//������㣬ֱ��ת��������
				return  Str_Int2str;
		}
    else  
	  { 
        for(i=0;i<5;i++)// ȡ��ÿλ�ϵ����ִ����vl[]
		    {
					 j= temp/dv[i];
					 temp  = temp%dv[i];
					 vl[i] = j;	
		    }
		}
	  i=0;// i����Ϊ0����Ϊ����Ҫ�����λ��ʼ��ʾ��
		while((vl[i]==0)&&(i<5))// ��⣬��һ������Ԫ�ص��±�					 
			i++;
		k=i;// ��ʱ�Ѿ��Ƿ�0�������ˣ�ȡ�±ꣻ		
		for(i=k,n=0; (i<5)&(n<5-k); i++,n++)
			Str_Int2str[n] = CHAR_MAP_G[vl[i]];// �ܹ���5-k����0Ԫ���˹����ѷ�0Ԫ��ȫ��Ū��disvl[n]��ȥ��
   	Str_Int2str[n]='\0';					//n+1���ַ�������
		if(f_bit==1)
		{
			strcat((char *)new_str,(char *)Str_Int2str);
			return  new_str;
		}
		else
			return  Str_Int2str;
}

/***************************************************************************************
//��������      hextoasciihex ��u8����,תΪ16���Ƶ��ַ��� ��: 12-> "0C"
//��������      u8 h:��Ҫת������
//��ں���      ��������,���ص�����
//���ں���      ��
***************************************************************************************/
u8* hextoasciihex(u8 h)
{
		unsigned char a=0;
		unsigned char b=0;
		static u8 hex[3] ={0};			//����ת������
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
//��������      num_change
//��������      ���͵ĺ�����ż����
//��ں���      ���軥���ĺ��� ,���볤��Ϊ11
//����: ����Ҫ������ż������������Ƕ�λ�����򲹸�F�ٽ��л���
//����: p:��������ַ����ĵ�ַ 
u8 *num_change(u8 *ph)
{
		unsigned char i = 0;
		unsigned char temp = 0;
		unsigned char len = 0;                //���볤��
		u8 static PhNum[15];
		ClearArray(PhNum,15);
		len = CalculateLen(ph);
		for(i = 0; i<len;i++)                 //����
		{
			PhNum[i] = *(ph+i);
		}
		if(len %2 != 0)     //���������,������󲹻�1��F
		{
			PhNum[len] = 'F';
			len++;
		}
	 for(i = 0;i<len/2;i++)       //����
	 {
			temp=PhNum[2*i+1];
			PhNum[2*i+1]=PhNum[2*i];
			PhNum[2*i]=temp;
	 }
	 return PhNum;
}

//***************************************
//����������ת��16����
//������n ����λ����תΪ��λ�ַ�
//����:����ַ�
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
//����:��������A�и��Ƶ�B��,ָ����������
//����:PA=A,PB=B,num���ƶ��ٸ���LRΪ��ʼ��ַ
void CoypArraryAtoArraryB_LR(const u8 *Pa,u8 *Pb,u16 num,u16 LR)
{
		u16 i=0;
		for(i=0;i<num ;i++)
		{
			*(Pb+i)	= *(Pa+i+LR);
		}
}

//7bit����   
//����:pSrc-Դ���봮ָ��   nSrcLength-Դ���봮����
//���:pDst-Ŀ���ַ���ָ��   
//����:Ŀ���ַ�������   
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)   
{   
    int nSrc; //   Դ�ַ����ļ���ֵ   
    int nDst; //   Ŀ����봮�ļ���ֵ   
    int nByte; //   ��ǰ���ڴ���������ֽڵ���ţ���Χ��0-6   
    unsigned   char   nLeft; //   ��һ�ֽڲ��������   
    //����ֵ��ʼ��   
    nSrc   =   0;   
    nDst   =   0;   
    //�����ֽ���źͲ������ݳ�ʼ��   
    nByte   =   0;   
    nLeft   =   0;   
    //��Դ����ÿ7���ֽڷ�Ϊһ�飬��ѹ����8���ֽ�   
    //ѭ���ô�����̣�ֱ��Դ���ݱ�������   
    //������鲻��7�ֽڣ�Ҳ����ȷ����   
    while(nSrc<nSrcLength)   
    {   
			//��Դ�ֽ��ұ߲��������������ӣ�ȥ�����λ���õ�һ��Ŀ������ֽ�   
      *pDst   =   ((*pSrc   <<   nByte)   |   nLeft)   &   0x7f;   
      //�����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������   
      nLeft   =   *pSrc   >>   (7-nByte);   
      //   �޸�Ŀ�괮��ָ��ͼ���ֵ   
      pDst++;   
      nDst++;   
      //   �޸��ֽڼ���ֵ   
      nByte++;   
      //   ����һ������һ���ֽ�   
      if(nByte   ==   7)   
      {   
      //   ����õ�һ��Ŀ������ֽ�   
      *pDst   =   nLeft;   
      //   �޸�Ŀ�괮��ָ��ͼ���ֵ   
      pDst++;   
      nDst++;   
      //   �����ֽ���źͲ������ݳ�ʼ��   
      nByte   =   0;   
      nLeft   =   0;   
      } 
      //   �޸�Դ����ָ��ͼ���ֵ   
      pSrc++;   
      nSrc++;   
      }   
      //   ����ַ����Ӹ�������   
      *pDst   =   '\0';   
      //   ����Ŀ�괮����   
      return   nDst;   
}

//**************************************************************************************
//����:������unicode��,��ӵ�CommonBuf��
//����:src:Unicode ��Դ
//����:
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

