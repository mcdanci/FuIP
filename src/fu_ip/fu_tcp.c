#include "fu_TCP.h"
#include "fu_config.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 组TCP首部   					                 				    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.2 2014年12月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

U16 TCP_Port=8952;//本地TCP 端口号
U16 TCP_Re_Port=0;//目的TCP 端口号
U8  xdata TCP_False[12]={0};//TCP伪首部，12字节
U8  xdata TCP_Head[20]={0};//TCP首部，长度20字节
U32 xdata TCP_Mark=0x1200;//TCP序号，唯一字段
U32 xdata TCP_CMark=0;//TCP确认序号，唯一字段
U8  TCP_Flag=0;//6个位标志
U8  TCP_Connected=0;//连接状态
U8  TCP_Sended=0;//发送标识
U8  TCP_Wflag=0;//连接类型 0 普通 1网页
U8  TCP_Clear=0;//tcp 状态清除
U16 TCP_Buf=0;//tcp发送缓冲长度
U16 TCP_Haved=0;//tcp已经发送长度

/*******************************************/
/* 功能：TCP 伪首部填充 12字节      	   */
/* 输入：TCP数据包长度				       */
/* 输出：无							       */
/*******************************************/
void TCP_Flase_Copy(U16 len)//len 为偶数
{
U8 i;
for(i=0;i<4;i++)
TCP_False[0+i]=FU_LoaclIP[i];//源IP

for(i=0;i<4;i++)
TCP_False[4+i]=Re_IP[i];//目的IP

TCP_False[8]=0;
TCP_False[9]=6;//6 TCP
TCP_False[10]=(20+len)/256;//TCP首部长+数据长
TCP_False[11]=(20+len)%256;//TCP首部长+数据长
}

/*******************************************/
/* 功能：TCP 首部填充  20字节       	   */
/* 输入：TCP数据包长度                     */
/* 输出：无							       */
/*******************************************/
void TCP_Head_Copy(void)
{
TCP_Head[0]=TCP_Port/256;
TCP_Head[1]=TCP_Port%256;//源端口
TCP_Head[2]=TCP_Re_Port/256;
TCP_Head[3]=TCP_Re_Port%256;//目的端口
TCP_Head[4]=TCP_Mark>>24;
TCP_Head[5]=TCP_Mark>>16;
TCP_Head[6]=TCP_Mark>>8;
TCP_Head[7]=TCP_Mark;//TCP序号
TCP_Head[8]=TCP_CMark>>24;
TCP_Head[9]=TCP_CMark>>16;
TCP_Head[10]=TCP_CMark>>8;
TCP_Head[11]=TCP_CMark;//TCP确认序号
TCP_Head[12]=0x50;//TCP首部长
TCP_Head[13]=TCP_Flag;//TCP6个位标志
TCP_Head[14]=(FU_LEN-58)/256;
TCP_Head[15]=(FU_LEN-58)%256;//TCP窗口大小 FU_LEN-14-20-20-4=最大能收的TCP数据
TCP_Head[16]=0;
TCP_Head[17]=0;//检验和
TCP_Head[18]=0;
TCP_Head[19]=0;//紧急指针
}

/********************************************/
/* 功能：检验和处理函数           			*/
/* 输入：无									*/
/* 输出：16位TCP检验和						*/
/********************************************/

U16 TCP_Check_Code(U16 len)
{
 U16 i;
 U16 tem;
 U32 checkcode=0;//检验和

/* 计算检验和 */

	for(i=0;i<12;i=i+2)//TCP伪首部12
	{
	tem=TCP_False[i]*256;
	checkcode+=tem+TCP_False[i+1];
	}
 	for(i=0;i<20;i=i+2)//TCP首部20
	{
	tem=TCP_Head[i]*256;
	checkcode+=tem+TCP_Head[i+1];
	}

	if(len%2==0)//TCP数据长度是偶数
	{
		for(i=0;i<len;i=i+2)
	    {   
		tem=DATA[i]*256;
		checkcode+=tem+DATA[i+1];
		}
	}
	else  //TCP数据长度是奇数
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
/* 功能：组TCP数据包函数           			*/
/* 输入：TCP 数据长度 len 		 			*/
/* 输出：无									*/
/********************************************/

void TCP_Send(U16 len)
{
U16 tem;
//填充TCP伪首部 12字节
TCP_Flase_Copy(len);//len 为偶数
//TCP 首部填充  20字节
TCP_Head_Copy();

tem=TCP_Check_Code(len);//得到检验和

TCP_Head[16]=tem/256;
TCP_Head[17]=tem%256;//检验和稍后补充
}


