#include <STC12C5A.H>
#include "mcuinit.h"
#include "delay.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_enthernet.h"
#include "fu_arp.h"
#include "fu_icmp.h"
#include "fu_ip.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_dhcp.h"
#include "fu_send.h"
#include "fu_receive.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ���͸������ݱ�����ARP DHCP Ping TCP UDP            			*/
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.3 2015��1��12��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

/*******************************************/
/* ���ܣ�����Զ��IP                		   */
/* ���룺    					           */
/* �������							       */
/*******************************************/

void Set_Re_IP(U8 Reip1,U8 Reip2,U8 Reip3,U8 Reip4)    
 {
	Re_IP[0]=Reip1;//Ŀ��IP1
	Re_IP[1]=Reip2;//Ŀ��IP2
	Re_IP[2]=Reip3;//Ŀ��IP3
	Re_IP[3]=Reip4;//Ŀ��IP4
 }

 /*******************************************/
/* ���ܣ�����APR��            		       */
/* ���룺��					               */
/* �������							       */
/* ��ע���漰������Զ��������IP��ַ		   */
/*******************************************/
void Send_ARP_Bag(void)
{
	U8 i,flag=0;
	//�ж���������������
	for(i=0;i<3;i++)
	{
	if(FU_LoaclIP[i]!=Re_IP[i])
	flag=1;
	
	}
	
	
	if(flag==0)	//������
	{
		//Ŀ��IP
		for(i=0;i<4;i++)
		{
		ARP_Data[24+i]=Re_IP[i];
		}
	
		Uart_Send_Byte(0xb0);
	 }

	if(flag==1)	//����
	{
		//Ŀ��IP
		for(i=0;i<4;i++)
		{
		ARP_Data[24+i]=FU_GateWay[i];
		}

		Uart_Send_Byte(0xb1);
	 }

	//Ŀ��MAC��ַFF �㲥
		for(i=0;i<6;i++)
		{
		EN_Head[i]=0xFF;
		}

	ARP_Ask(1);//��ARP�� //1����	2Ӧ��

	EN_Head_Copy(1);//EN��̫��ͷ��1 ARP����
	ARP_Packet_Send();//����ARP��
}


/*******************************************/
/* ���ܣ�����APRӦ���            		   */
/* ���룺��					               */
/* �������							       */
/* ��ע���漰������Զ��������IP��ַ		   */
/*******************************************/
void Answer_ARP_Bag(void)
{
	U8 i;

	//Ŀ��IP
	//Ŀ��MAC
	for(i=0;i<6;i++)
	{
 	EN_Head[i]=Re_MAC[i];
	}

	ARP_Ask(2);//1��ARP����� 2Ӧ���


	EN_Head_Copy(1);//EN��̫��ͷ��1 ARP����
	ARP_Packet_Send();//����ARP��
}

/*******************************************/
/* ���ܣ�����ARP                		   */
/* ���룺��  					           */
/* �����״ֵ̬	21�ɹ�				       */
/*******************************************/
void ARP(void)
{

	while(ARP_Flag!=0x21)//�õ��ش�
	{

		Send_ARP_Bag();
		Delay_5ms(100);
		Data_Receive();

	}
 
}

/*******************************************/
/* ���ܣ�����DHCP                		   */
/* ���룺Ŀ��IP					           */
/* �����״ֵ̬	21�ɹ�				       */
/*******************************************/
void DHCP(void)
{
	DHCP_Flag=0x00;
	while(DHCP_Flag!=0x03)//�õ��ش�
	{
		
			if(DHCP_Flag==0x00)
				{
				DHCP_Send_Discover();	
				Delay_5ms(100);
				Data_Receive();
				}

			Uart_Send_Byte(0xc5);

			if(DHCP_Flag==0x01)
			    DHCP_Flag=0x02;

		    if(DHCP_Flag==0x02)
				{
				DHCP_Send_Request();	
				Delay_5ms(100);
				Data_Receive();
				}
	   	
	}
 	
}



/*******************************************/
/* ���ܣ�����TCP��            		       */
/* ���룺���ݳ���len	         	       */
/* �������							       */
/* ��ע���漰Զ��IP,Զ��MAC,Զ��,���ض˿ں�*/
/*******************************************/
void Send_TCP_Bag(U16 len)
{	
	U8 i;
    //Ŀ��IP
	//Ŀ��MAC
	for(i=0;i<6;i++)
	{
	EN_Head[i]=Re_MAC[i];
	}

	TCP_Send(len);//��TCP�����ݳ���,Զ�̶˿�
	IP_Send(1,40+len);//0 UDP,1 TCP,2 ICMP��IPͷ�� 20+20+len
	EN_Head_Copy(0);//EN��̫��ͷ��0 IP���� 1 ARP
	TCP_Packet_Send(len);//����TCP��
}


/*******************************************/
/* ���ܣ�����UDP��            		       */
/* ���룺���ݳ���	   		               */
/* �������							       */
/* ��ע���漰Զ��IP,Զ��MAC,Զ��,���ض˿ں�*/
/*******************************************/
void Send_UDP_Bag(U16 len)
{	
	U8 i=0;	
	//Ŀ��IP
    //Ŀ��MAC��ַ
	for(i=0;i<6;i++)
	{
	EN_Head[i]=Re_MAC[i];
	}

	UDP_Send(len);//��UDP��,Զ�̶˿�2014
	IP_Send(0,28+len);//0 UDP,1 TCP,2 ICMP��IPͷ,len
	EN_Head_Copy(0);//EN��̫��ͷ��0 IP���� 1 ARP
	UDP_Packet_Send(len);//����UDP��
}

/*******************************************/
/* ���ܣ�����UDP���ݰ�            		   */
/* ���룺���ݳ���	   		               */
/* �������							       */
/* ��ע���漰Զ��IP,Զ��MAC,Զ��,���ض˿ں�*/
/*******************************************/
void Send_UDP_Data_Bag(U16 len)
{
    if(ARP_Flag!=0x21)
	ARP();
    if(ARP_Flag==0x21)//���ARPԶ��MAC�ɹ�
	{
	Send_UDP_Bag(len);	
	}
}

/*******************************************/
/* ���ܣ�����UDP���ݰ�            		   */
/* ���룺���ݳ���	   		               */
/* �������							       */
/* ��ע���漰Զ��IP,Զ��MAC,Զ��,���ض˿ں�*/
/*******************************************/

/*******************************************/
/* ���ܣ�����ICMP_Ping����            	   */
/* ���룺��					               */
/* �������							       */
/* ��ע���漰Զ��IP,Զ��MAC,Զ��,���ض˿ں�*/
/*******************************************/
void Send_ICMP_Ping_Bag(void)
{	
	U8 i;
	//Ŀ��IP
    //Ŀ��MAC��ַ
	if(ARP_Flag!=0x21)
	ARP();
    if(ARP_Flag==0x21)//���ARPԶ��MAC�ɹ�
	{
		//Ŀ��MAC��ַ
		for(i=0;i<6;i++)
		{
		EN_Head[i]=Re_MAC[i];
		}
		ICMP_Ping_Send();//��ICMP_Ping��
		IP_Send(2,60);//0 UDP,1 TCP,2 ICMP��IPͷ
		EN_Head_Copy(0);//EN��̫��ͷ��0 IP���� 1ARP
		
		ICMP_Ping_Packet_Send();//����ICMP_Ping��	
	}



	
}


/*******************************************/
/* ���ܣ�����ICMP_Ping�ذ�     		       */
/* ���룺��					               */
/* �������							       */
/* ��ע���漰Զ��IP,Զ��MAC,Զ��,���ض˿ں�*/
/*******************************************/
void Send_ICMP_Ping_Back_Bag(void)
{	
	 U8 i;
	//Ŀ��IP
    //Ŀ��MAC��ַ
	for(i=0;i<6;i++)
	{
	EN_Head[i]=Re_MAC[i];
	}
	ICMP_Ping_Answer_Send();//��ICMP_Ping��
	IP_Send_Ping_Back(2,60);//0 UDP,1 TCP,2 ICMP��IPͷ
	EN_Head_Copy(0);//EN��̫��ͷ��0 IP���� 1ARP
	
	ICMP_Ping_Packet_Send();//����ICMP_Ping��
}


/*******************************************/
/* ���ܣ������ʼ��                		   */
/* ���룺��					               */
/* �������   				               */
/*******************************************/
void net_init(void)
{
 Uart_Send_Byte(0xc0);
 ARP_Flag=0x21;
 DHCP();
 ARP_Flag=0x00;
}
