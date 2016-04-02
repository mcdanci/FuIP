#ifndef __fu_dhcp_H__
#define __fu_dhcp_H__

#include "type.h"
#include "fu_config.h"
#include "mcuinit.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 用于处理接收到TCP,UDP数据后的识别，执行（自定义）         		*/
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.3 2014年1月12日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/
extern U8 xdata DHCP_Data[34];
extern U8 code DHCP_Zero[202];
extern U8 DHCP_Flag;//四个阶段1 发出请求discover  2收到应答提供offer IP 3发出选择 request 4收到确认信息ack
extern U8 DHCP_OK;
extern U16 DHCP_Time;//15分钟一次request
extern U8 DHCP_Client[4];
extern U8 DHCP_Server[4];


/*******************************************/
/* 功能：1、发送discover包          	   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：                                  */
/*******************************************/

void DHCP_Send_Discover();

/*******************************************/
/* 功能：3、发送request包            	   */
/* 输入：IP					               */
/* 输出：无							       */
/* 备注：                                  */
/*******************************************/

void DHCP_Send_Request(void);


#endif 