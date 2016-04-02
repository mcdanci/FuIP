#ifndef __fu_config_H__
#define __fu_config_H__
#include <STC12C5A.H>

#include "type.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 配置信息，包括本地IP,MAC,子网掩码，默认网关，及相关缓存开辟		*/
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.0 2014年6月26日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

/*IP 相关参数 宏定义设置*/


#define FU_TTL 	255	 //IP数据报生存周期
#define FU_LEN  321  //发送数据缓冲区有效数据长度

extern U8 xdata FU_LoaclIP[4];//本地IP地址
extern U8 code FU_SubnetMask[4];//子网掩码
extern U8 xdata FU_GateWay[4]; //默认网关
extern U8 code FU_MAC[6];//本地MAC
extern U8 Re_IP[4];//目的IP
extern U8 Re_MAC[6];//目的MAC
extern U8 xdata DATA[FU_LEN];//收发缓存

#endif 