#include "fu_ip.h"
#include "fu_config.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* IP首部组包程序包括TCP UDP ICMP类型             				    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v1.2 2014年4月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

U8 xdata IP_Head[20]={0};//IP首部20字节
U16 xdata IP_Mark=0x1200;//IP首部的标示字段

/*******************************************/
/* 功能：填充IP首部20字节      		       */
/* 输入：IP数据报类型0 UDP,1 TCP,2 ICMP	   */
/* 输入：IP数据报长度UDP20+8+len ICMP20+len*/
/* 输出：无							       */
/*******************************************/

void IP_Head_Copy(U8 leixing,U16 IP_len)
{

U8 i;
IP_Head[0]=0x45;//版本 首部长度
IP_Head[1]=0;//TOS 一般服务
IP_Head[2]=IP_len/256;
IP_Head[3]=IP_len%256;//len=20+8+UDP_len,16位IP报总长度,20+40ICMP_Len ,20+20+TCP_Len
IP_Mark++;//标示字段+1，唯一字段
if(IP_Mark==0xffff)
IP_Mark=0x1200;
IP_Head[4]=IP_Mark/256;
IP_Head[5]=IP_Mark%256;
IP_Head[6]=0;
IP_Head[7]=0;//标志，偏移，分片
IP_Head[8]=FU_TTL;//生存周期

if(leixing==0)
IP_Head[9]=0x11;//17 UDP数据包
if(leixing==1)
IP_Head[9]=0x06;//6 TCP数据包
if(leixing==2)
IP_Head[9]=0x01;//01 ICMP

IP_Head[10]=0;
IP_Head[11]=0;//检验和

//源IP
for(i=0;i<4;i++)
	IP_Head[12+i]=FU_LoaclIP[i];
//目的IP
for(i=0;i<4;i++)
	IP_Head[16+i]=Re_IP[i];


}

/********************************************/
/* 功能：IP首部 检验和处理函数           	*/
/* 输入：无									*/
/* 输出：16位UDP检验和						*/
/********************************************/
U16 IP_Check_Code()
{
U8 i;
U16 tem;
U32 checkcode=0;

	for(i=0;i<20;i=i+2)
	{
	tem=IP_Head[i]*256;
	checkcode+=tem+IP_Head[i+1]; 
	}
	
/* 如果进位就需要加到尾部*/ 
	if(checkcode>0xFFFF)
	{
	tem=(U16)(checkcode>>16);//得到高位
	checkcode=checkcode&0xFFFF;//去掉高位
    checkcode=checkcode+tem;//加到低位

	tem=(U16)(checkcode>>16);//得到高位
    checkcode=checkcode+tem;//加到低位
	}
	
	tem=(U16)checkcode;
	
	tem=~tem;
return tem;//取反	 +

}

/********************************************/
/* 功能：IP首部组包                      	*/
/* 输入：IP包长度 20+8+len					*/
/* 输出：无					 				*/
/********************************************/
void IP_Send(U8 leixing,U16 IP_len)
{
U16 tem;
IP_Head_Copy(leixing,IP_len);
tem=IP_Check_Code();//IP首部检验和

IP_Head[10]=tem/256;
IP_Head[11]=tem%256;

}

/********************************************/
/* 功能：IP首部组包（回Ping专用）          	*/
/* 输入：IP包长度 20+8+len					*/
/* 输出：无         						*/
/********************************************/
void IP_Send_Ping_Back(U8 leixing,U16 IP_len)
{
U16 tem;
IP_Head_Copy(leixing,IP_len);
IP_Head[4]=DATA[18];
IP_Head[5]=DATA[19];//回Ping要跟主Ping一致
tem=IP_Check_Code();//IP首部检验和

IP_Head[10]=tem/256;
IP_Head[11]=tem%256;

}								   

