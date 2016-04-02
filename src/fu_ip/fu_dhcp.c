#include "string.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "mcuinit.h"
#include "delay.h"
#include "fu_udp.h"
#include "fu_dhcp.h"
#include "fu_send.h"
#include "fu_receive.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 用于自动获取IP                                            		*/
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.3 2015年1月12日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

U8 xdata DHCP_Data[34]={0x01,0x01,0x06,0x00,0x45,0x7C,0x33,0xF5,0x00,0x00,0x80,0x00};
U8 code DHCP_Zero[202]={0};
U8 code DHCP_Msg[24]={0x63,0x82,0x53,0x63,0x35,0x01,0x00,0x0c,0x0f,0x46,0x75,0x68,
0x6F,0x6D,0x65,0x2E,0x6E,0x65,0x74,0x2F,0x46,0x75,0x49,0x50};
U8 DHCP_Flag=0;
U8 DHCP_OK=0;
U16	DHCP_Time=0;
U8 DHCP_Which=0;
U8 DHCP_Client[4]={0,0,0,0};
U8 DHCP_Server[4]={0,0,0,0};
/*******************************************/
/* 功能：1、生成discover包头          	   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：                                  */
/*******************************************/

void DHCP_Head_Discover()
{
  DHCP_Which=1;//discover
  memcpy(&DHCP_Data[28],FU_MAC,6);//复制本机MAC
}

/*******************************************/
/* 功能：3、生成request包头          	   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：                                  */
/*******************************************/

void DHCP_Head_Requsest()
{
  DHCP_Which=3;//request
  memcpy(&DHCP_Data[28],FU_MAC,6);//复制本机MAC
}

/*******************************************/
/* 功能：1、发送discover包          	   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：                                  */
/*******************************************/

void DHCP_Send_Discover()
{
UDP_Port=68;//UDP本地端口
UDP_Re_Port=67;//UDP远程端口
DHCP_Head_Discover();
Uart_Send_Byte(0xce);
memcpy(DATA,DHCP_Data,34);//将数据复制到缓冲区
memcpy(&DATA[34],DHCP_Zero,202);//将数据复制到缓冲区
memcpy(&DATA[236],DHCP_Msg,24);//将数据复制到缓冲区
DATA[242]=DHCP_Which;
DATA[260]=0xff;
Send_UDP_Data_Bag(261);//发送UDP数据包
}

/*******************************************/
/* 功能：3、发送request包            	   */
/* 输入：IP					               */
/* 输出：无							       */
/* 备注：                                  */
/*******************************************/

void DHCP_Send_Request(void)
{
UDP_Port=68;//UDP本地端口
UDP_Re_Port=67;//UDP远程端口
DHCP_Head_Requsest();
memcpy(DATA,DHCP_Data,34);//将数据复制到缓冲区
memcpy(&DATA[34],DHCP_Zero,202);//将数据复制到缓冲区
memcpy(&DATA[236],DHCP_Msg,24);//将数据复制到缓冲区
DATA[242]=DHCP_Which;
DATA[261]=0x32;
DATA[262]=0x04;
memcpy(&DATA[263],DHCP_Client,4);//将数据复制到缓冲区
DATA[267]=0xff;
Send_UDP_Data_Bag(268);//发送UDP数据包
}


