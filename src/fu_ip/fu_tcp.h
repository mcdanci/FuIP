#ifndef __fu_TCP_H__
#define __fu_TCP_H__
#include "type.h"
#include "fu_config.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 组TCP首部   					                 				    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v1.2 2014年4月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

#define TCP_URG 0x20
#define TCP_ACK 0x10
#define TCP_PSH 0x08
#define TCP_RST 0x04
#define TCP_SYN 0x02
#define TCP_FIN 0x01

extern U16 TCP_Port;//本地TCP 端口号
extern U16 TCP_Re_Port;//目的TCP 端口号
extern U8 xdata TCP_False[12];//TCP伪首部，12字节
extern U8 xdata TCP_Head[20];//TCP首部，长度20字节
extern U32 xdata TCP_Mark;//TCP序号，唯一字段
extern U32 xdata TCP_CMark;//TCP确认序号，唯一字段
extern U8 TCP_Flag;//6个位标志
extern U8 TCP_Connected;//握手标志 3成功可以发数据了
extern U8 TCP_Sended;//数据发送收到回应了
extern U8 TCP_Wflag;//连接类型 0 普通 1网页
extern U8 TCP_Clear;//tcp 状态清除
extern U16 TCP_Buf;//tcp发送缓冲长度
extern U16 TCP_Haved;//tcp已经发送长度


/*******************************************/
/* 功能：TCP 首部填充  20字节       	   */
/* 输入：TCP数据包长度                     */
/* 输出：无							       */
/*******************************************/
void TCP_Head_Copy(void);

/*******************************************/
/* 功能：填充TCP伪首部 12字节      		   */
/* 输入：数据数组长度				       */
/* 输出：无							       */
/*******************************************/
void TCP_Flase_Copy(U16 len);//len 为偶数

/********************************************/
/* 功能：检验和处理函数           			*/
/* 输入：无									*/
/* 输出：16位TCP检验和						*/
/********************************************/
U16 TCP_Check_Code(U16 len);

/********************************************/
/* 功能：组TCP数据包函数           			*/
/* 输入：无									*/
/* 输出：无									*/
/********************************************/
void TCP_Send(U16 len);

#endif