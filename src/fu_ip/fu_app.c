#include "string.h"
#include "fu_app.h"
#include "fu_config.h"
#include "mcuinit.h"
#include "delay.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_send.h"
#include "fu_receive.h"
#include "fu_http.h"

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

/*******************************************/
/* ���ܣ�����UDP��               		   */
/* ���룺��					               */
/* �������							       */
/* ��ע����Ч���ݴ�DATA[42]��ʼ	    	   */
/*******************************************/
void App_UDP_Bag(void)
{
	//����ʶ��
	 if (memcmp(&DATA[42],"open",4) ==0)
	 {
	 if(DATA[46]=='1')
	    led0=0;
	  if(DATA[46]=='2')
	    led1=0;
	  if(DATA[46]=='3')
	    led2=0;
	  if(DATA[46]=='4')
	    led3=0;
	
	   memcpy(DATA,"opened!",7);
		Send_UDP_Data_Bag(7);//����UDP���ݰ� 
	 }
	 	 


	   if (memcmp(&DATA[42],"close",5) ==0)
	   {

	  if(DATA[47]=='1')
	    led0=1;
	  if(DATA[47]=='2')
	    led1=1;
	  if(DATA[47]=='3')
	    led2=1;
	  if(DATA[47]=='4')
	    led3=1; 

        memcpy(DATA,"closed!",7);
		Send_UDP_Data_Bag(7);//����UDP���ݰ�
		}

	



}

/*******************************************/
/* ���ܣ�����TCP��               		   */
/* ���룺��					               */
/* �������							       */
/* ��ע����Ч���ݴ�DATA[54]��ʼ  		   */
/*******************************************/
void App_TCP_Bag(void)
{
	
		char datlen=0;

	 /* ��ͨTCP���� */
	 if (memcmp(&DATA[54],"open",4) ==0)	
	   {

		  if(DATA[58]=='1')
		    led0=0;
		  if(DATA[58]=='2')
		    led1=0;
		  if(DATA[58]=='3')
		    led2=0;
		  if(DATA[58]=='4')
		    led3=0;
		    
			TCP_Flag=(TCP_ACK|TCP_PSH);//��λ��ʶ
			 memcpy(DATA,"opened!",7);//�������ݵ���������
		   	Send_TCP_Bag(7);	
		 
		}
		
	   if (memcmp(&DATA[54],"close",5) ==0)
	   {

		  if(DATA[59]=='1')
		    led0=1;
		  if(DATA[59]=='2')
		    led1=1;
		  if(DATA[59]=='3')
		    led2=1;
		  if(DATA[59]=='4')
		    led3=1;
			
			TCP_Flag=(TCP_ACK|TCP_PSH);//��λ��ʶ
			 memcpy(DATA,"closed!",7);//�������ݵ���������
		   	Send_TCP_Bag(7); 
		}

	   	/* ��ҳweb����ʶ�� */
	   //index
	   if (memcmp(&DATA[54],"GET / ",6) ==0)
	   {
		 if(TCP_Buf==0)//�ϴ��Ѿ��������Ӧ
		 {
		 pSend=fu_index;
		 TCP_Send_Over(pSend,253);
		 }
	   }
       //open
	   else if (memcmp(&DATA[54],"GET /open",9) ==0)
	   {
		 led1=0;
		 led2=0;
		 if(TCP_Buf==0)
		 {
		 pSend=fu_open;
		 TCP_Send_Over(pSend,120);
		 }
	   }
	   //close
	   else if (memcmp(&DATA[54],"GET /close",10) ==0)
	   {
		 led1=1;
		 led2=1;
		 if(TCP_Buf==0)
		 {
		  pSend=fu_close;
		 TCP_Send_Over(pSend,119);
		 }
	   }
	  

}