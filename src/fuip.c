/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 主程序   					                 				    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.2 2014年10月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

#include <STC12C5A.H>
#include "string.h"
#include "mcuinit.h"
#include "type.h"
#include "delay.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_arp.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_send.h"
#include "fu_receive.h"
#include "fu_app.h"
#include "fu_http.h"

main()
{

//各种状态量初始化
TCP_Connected=0x00;
TCP_Sended=0x00;
TCP_Clear=0x00;//tcp 状态清除
ARP_Flag=0x00;
Timer10ms=0;//时间限制


/* 硬件初始化 */
enc28j60_init();
/* 串口0配置 */
uart0_init();
/* 定时器0,10ms配置 */ 
timer0_init();
/* 外部中断0 */
int0_init();


/* 主动发送UDP数据 
Set_Re_IP(192,168,1,101);//设置远程IP
UDP_Port=8951;//UDP本地端口
UDP_Re_Port=2014;//UDP远程端口
memcpy(DATA,"HELLO!",6);//将数据复制到缓冲区
Send_UDP_Data_Bag(6);//发送UDP数据包
*/


	while(1)
	{	
	Data_Receive();
	} 
} 
