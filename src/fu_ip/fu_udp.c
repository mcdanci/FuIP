#include "fu_udp.h"
#include "fu_config.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 组UDP首部包   					                 			    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.1 2014年10月15日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

U16 UDP_Port=8951;//本地UDP 端口号
U16 UDP_Re_Port=0;//目的UDP 端口号
U8 xdata UDP_False[12]={0};//UDP伪首部，12字节
U8 xdata UDP_Head[8]={0};//UDP首部，长度8字节

/*******************************************/
/* 功能：UDP 伪首部填充 12字节      	   */
/* 输入：UDP数据包长度				       */
/* 输出：无							       */
/*******************************************/
void UDP_Flase_Copy(U16 len)//len 为偶数
{
U8 i;
for(i=0;i<4;i++)
UDP_False[0+i]=FU_LoaclIP[i];//源IP

for(i=0;i<4;i++)
UDP_False[4+i]=Re_IP[i];//目的IP

UDP_False[8]=0;
UDP_False[9]=17;
UDP_False[10]=(8+len)/256;//UDP首部长+数据长
UDP_False[11]=(8+len)%256;//UDP首部长+数据长
}

/*******************************************/
/* 功能：UDP 首部填充  8字节       		   */
/* 输入：UDP数据包长度                     */
/* 输出：无							       */
/*******************************************/
void UDP_Head_Copy(U16 len)
{
UDP_Head[0]=UDP_Port/256;
UDP_Head[1]=UDP_Port%256;//源端口
UDP_Head[2]=UDP_Re_Port/256;
UDP_Head[3]=UDP_Re_Port%256;//目的端口
UDP_Head[4]=(len+8)/256;
UDP_Head[5]=(len+8)%256;//UDP长度=UDP首部+数据
UDP_Head[6]=0;
UDP_Head[7]=0;//检验和稍后补充
}

/********************************************/
/* 功能：检验和处理函数           			*/
/* 输入：无									*/
/* 输出：16位UDP检验和						*/
/********************************************/

U16 UDP_Check_Code(U16 len)
{
 U16 i;
 U16 tem;
 U32 checkcode=0;//检验和

/* 计算检验和 */

	for(i=0;i<12;i=i+2)//UDP伪首部12
	{
	tem=UDP_False[i]*256;
	checkcode+=tem+UDP_False[i+1];
	}
 	for(i=0;i<8;i=i+2)//UDP首部8
	{
	tem=UDP_Head[i]*256;
	checkcode+=tem+UDP_Head[i+1];
	}
	
	if(len%2==0)//UDP数据长度是偶数
	{
		for(i=0;i<len;i=i+2)
	    {   
		tem=DATA[i]*256;
		checkcode+=tem+DATA[i+1];
		}
	}
	else//UDP数据长度是奇数
	{	 
	     DATA[len]=0;
	   for(i=0;i<len+1;i=i+2)
	    {   
		tem=DATA[i]*256;
		checkcode+=tem+DATA[i+1];
		}
	
	}
	
/*如果进位就需要加到尾部 */
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
return tem;//取反
}

/********************************************/
/* 功能：组UDP数据包函数           			*/
/* 输入：UDP 数据长度 len      			    */
/* 输出：无									*/
/********************************************/

void UDP_Send(U16 len)
{
U16 tem;
//填充UDP伪首部 12字节
UDP_Flase_Copy(len);//len 为偶数
//UDP 首部填充  8字节
UDP_Head_Copy(len);

tem=UDP_Check_Code(len);//得到检验和

UDP_Head[6]=tem/256;
UDP_Head[7]=tem%256;//检验和稍后补充
}


