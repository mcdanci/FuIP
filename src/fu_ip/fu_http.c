#include "fu_http.h"
#include "fu_config.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ���ڴ�����յ�TCP,UDP���ݺ��ʶ��ִ�У��Զ��壩         		*/
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.2 2014��12��5��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

char* pSend;//����ָ��

/* ���ݰ� */
U8 code fu_get[109]={
"GET / HTTP/1.0\r\n"
"Host: baidu.com\r\n"
"Connection: close\r\n"
"Accept: text/html\r\n"
"User-Agent: Fuhome (STC12 MCU)\r\n"
"\r\n\r\n"};



U8 code fu_index[253]={//413
"HTTP/1.1 200 OK\r\n"
"Server: Fuhome server\r\n"
"love: only\r\n"
"Content-type: text/html\r\n\r\n"

"<html><body><center>"									  
"<h3>Fuhome.net FuIP2.2</h3>"
"<li><a href='open.html' target='view'>open</a></li>"
"<li><a href='close.html' target='view'>close</a></li>"
"</center></body></html>"
};
//[��20���ַ���ַ��fu_main[353]��ʼ����Ϊ��ʾ�ַ�]


U8 code fu_open[120]={//72
"HTTP/1.1 200 OK\r\n"
"Server: Fuhome server 2.2\r\n"
"love: only\r\n"
"Content-type: text/html\r\n\r\n"

"<html><body>LED opened!</body></html>"

};

U8 code fu_close[119]={//83
"HTTP/1.1 200 OK\r\n"
"Server: Fuhome server 2.2\r\n"
"love: only\r\n"
"Content-type: text/html\r\n\r\n"

"<html><body>LED closd!</body></html>"

};

