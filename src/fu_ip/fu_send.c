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
#include "fu_send.h"
#include "fu_receive.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 发送各种数据报程序，ARP Ping TCP UDP            				    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.0 2014年6月29日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

/*******************************************/
/* 功能：设置远程IP                		   */
/* 输入：    					           */
/* 输出：无							       */
/*******************************************/

void Set_Re_IP(U8 Reip1,U8 Reip2,U8 Reip3,U8 Reip4)    
 {
	Re_IP[0]=Reip1;//目的IP1
	Re_IP[1]=Reip2;//目的IP2
	Re_IP[2]=Reip3;//目的IP3
	Re_IP[3]=Reip4;//目的IP4
 }

 /*******************************************/
/* 功能：发送APR包            		       */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：涉及被呼叫远程主机的IP地址		   */
/*******************************************/
void Send_ARP_Bag(void)
{
	U8 i,flag=0;
	//判断是内网还是外网
	for(i=0;i<3;i++)
	{
	if(FU_LoaclIP[i]!=Re_IP[i])
	flag=1;
	
	}
	
	
	if(flag==0)	//内网网
	{
		//目的IP
		for(i=0;i<4;i++)
		{
		ARP_Data[24+i]=Re_IP[i];
		}
	
		Uart_Send_Byte(0xb0);
	 }

	if(flag==1)	//外网
	{
		//目的IP
		for(i=0;i<4;i++)
		{
		ARP_Data[24+i]=FU_GateWay[i];
		}

		Uart_Send_Byte(0xb1);
	 }

	//目的MAC地址FF 广播
		for(i=0;i<6;i++)
		{
		EN_Head[i]=0xFF;
		}

	ARP_Ask(1);//组ARP包 //1请求	2应答

	EN_Head_Copy(1);//EN以太网头，1 ARP类型
	ARP_Packet_Send();//发送ARP包
}


/*******************************************/
/* 功能：发送APR应答包            		   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：涉及被呼叫远程主机的IP地址		   */
/*******************************************/
void Answer_ARP_Bag(void)
{
	U8 i;

	//目的IP
	//目的MAC
	for(i=0;i<6;i++)
	{
 	EN_Head[i]=Re_MAC[i];
	}

	ARP_Ask(2);//1组ARP请求包 2应答包


	EN_Head_Copy(1);//EN以太网头，1 ARP类型
	ARP_Packet_Send();//发送ARP包
}

/*******************************************/
/* 功能：主动ARP                		   */
/* 输入：目的IP					           */
/* 输出：状态值	21成功				       */
/*******************************************/
void ARP(void)
{

	while(ARP_Flag!=0x21)//得到回答
	{

		Send_ARP_Bag();
		Delay_5ms(100);
		Data_Receive();

	}
 
}



/*******************************************/
/* 功能：发送TCP包            		       */
/* 输入：数据长度len	         	       */
/* 输出：无							       */
/* 备注：涉及远程IP,远程MAC,远程,本地端口号*/
/*******************************************/
void Send_TCP_Bag(U16 len)
{	
	U8 i;
    //目的IP
	//目标MAC
	for(i=0;i<6;i++)
	{
	EN_Head[i]=Re_MAC[i];
	}

	TCP_Send(len);//组TCP包数据长度,远程端口
	IP_Send(1,40+len);//0 UDP,1 TCP,2 ICMP组IP头， 20+20+len
	EN_Head_Copy(0);//EN以太网头，0 IP类型 1 ARP
	TCP_Packet_Send(len);//发送TCP包
}


/*******************************************/
/* 功能：发送UDP包            		       */
/* 输入：数据长度	   		               */
/* 输出：无							       */
/* 备注：涉及远程IP,远程MAC,远程,本地端口号*/
/*******************************************/
void Send_UDP_Bag(U16 len)
{	
	U8 i=0;	
	//目的IP
    //目的MAC地址
	for(i=0;i<6;i++)
	{
	EN_Head[i]=Re_MAC[i];
	}

	UDP_Send(len);//组UDP包,远程端口2014
	IP_Send(0,28+len);//0 UDP,1 TCP,2 ICMP组IP头,len
	EN_Head_Copy(0);//EN以太网头，0 IP类型 1 ARP
	UDP_Packet_Send(len);//发送UDP包
}

/*******************************************/
/* 功能：发送UDP数据包            		   */
/* 输入：数据长度	   		               */
/* 输出：无							       */
/* 备注：涉及远程IP,远程MAC,远程,本地端口号*/
/*******************************************/
void Send_UDP_Data_Bag(U16 len)
{
    if(ARP_Flag!=0x21)
	ARP();
    if(ARP_Flag==0x21)//如果ARP远程MAC成功
	{
	Send_UDP_Bag(len);	
	}
}

/*******************************************/
/* 功能：发送ICMP_Ping主包            	   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：涉及远程IP,远程MAC,远程,本地端口号*/
/*******************************************/
void Send_ICMP_Ping_Bag(void)
{	
	U8 i;
	//目的IP
    //目的MAC地址
	if(ARP_Flag!=0x21)
	ARP();
    if(ARP_Flag==0x21)//如果ARP远程MAC成功
	{
		//目的MAC地址
		for(i=0;i<6;i++)
		{
		EN_Head[i]=Re_MAC[i];
		}
		ICMP_Ping_Send();//组ICMP_Ping包
		IP_Send(2,60);//0 UDP,1 TCP,2 ICMP组IP头
		EN_Head_Copy(0);//EN以太网头，0 IP类型 1ARP
		
		ICMP_Ping_Packet_Send();//发送ICMP_Ping包	
	}



	
}


/*******************************************/
/* 功能：发送ICMP_Ping回包     		       */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：涉及远程IP,远程MAC,远程,本地端口号*/
/*******************************************/
void Send_ICMP_Ping_Back_Bag(void)
{	
	 U8 i;
	//目的IP
    //目的MAC地址
	for(i=0;i<6;i++)
	{
	EN_Head[i]=Re_MAC[i];
	}
	ICMP_Ping_Answer_Send();//组ICMP_Ping包
	IP_Send_Ping_Back(2,60);//0 UDP,1 TCP,2 ICMP组IP头
	EN_Head_Copy(0);//EN以太网头，0 IP类型 1ARP
	
	ICMP_Ping_Packet_Send();//发送ICMP_Ping包
}
