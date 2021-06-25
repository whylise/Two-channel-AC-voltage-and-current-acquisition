#include "http.h"

extern u8 GSMUART_buff[MAX_GSMUART];

//**************************************************************
//����:http��ʼ������
//����:bearer init
//����:	0:�ɹ�   others:ʧ�� 
u8 http_init(void)
{
		UART_string_newline("AT+SAPBR=3,1,\"Contype\",\"GPRS\"",1);//���ó��س���1
		delay_ms(500); 
		UART_string_newline("AT+SAPBR=3,1,\"APN\",\"CMNET\"",1);//���ó��س���1
		delay_ms(500);
		UART_string_newline("AT+SAPBR=1,1",1);//����һ��GPRS������
		delay_ms(500); 
		clear_USART1buff();
		UART_string_newline("AT+SAPBR=2,1",1);//��ѯGPRS������
		delay_ms(500);  
		if(!strsearch(GSMUART_buff,"0.0.0.0"))return 0;//�ж��Ƿ񼤻�ɹ�
		return 1;
}

//************************
//����: д��apn  bearer 
//����:	0:�ɹ�   
u8 write_bearer_apn(void)
{
		if(WriteApnName22("CMNET"))//д��APN��
				WriteApnName22("CMNET");//��һ��д��ʧ��,��д
		return 0;
}
