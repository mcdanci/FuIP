#include "fu_icmp.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 组ICMP数据包主要是Ping   			           				    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.0 2014年6月26日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

//类型主ping80 回ping 00 ，检验和2，标识符2，序号2，数据
U8 xdata ICMP[40]={0,0,0,0};
U16 xdata ICMP_Mark=0x0000;//ICMP的序号字段
/*******************************************/
/* 功能：ICMP 主ping数据填充  	      	   */
/* 输入：无   				               */
/* 输出：无							       */
/*******************************************/
void ICMP_Ping_Copy(void)
{
U8 i;
ICMP[0]=0x08;//主ping
ICMP[1]=0;//
ICMP[2]=0;
ICMP[3]=0;//检验和
ICMP[4]=0x00;//自定义
ICMP[5]=0x01;//自定义
ICMP_Mark++;//标示字段+1，唯一字段
if(ICMP_Mark==0xffff)
ICMP_Mark=0x0000;
ICMP[6]=ICMP_Mark/256;//
ICMP[7]=ICMP_Mark%256;//序号

for(i=0;i<23;i++)
ICMP[8+i]='a'+i;//任意内容
for(i=0;i<9;i++)
ICMP[31+i]='a'+i;//任意内容
}

/*******************************************/
/* 功能：ICMP 回ping数据填充  	      	   */
/* 输入：无   				               */
/* 输出：无							       */
/*******************************************/
void ICMP_Ping_Answer(void)
{
U8 i;
for(i=0;i<40;i++)
 {
  ICMP[i]=DATA[34+i];//ping
 }
 ICMP[0]=0;//回ping标志
 ICMP[2]=0;
 ICMP[3]=0;//检验和
 }


/********************************************/
/* 功能：检验和处理函数           			*/
/* 输入：无									*/
/* 输出：16位ICMP检验和						*/
/********************************************/

U16 ICMP_Check_Code(void)
{
/* U8 i;
 U16 tem;
 U32 checkcode=0;//检验和
 计算检验和 

	for(i=0;i<40;i=i+2)//ICMP 20
	{
	tem=ICMP[i]*256;
	checkcode+=tem+ICMP[i+1];
	}	  */

/* 如果进位就需要加到尾部 
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
return tem;//取反	*/

//下边这段只用上边31%的时间，结果是一样的。
//原来的代码是先累加，然后把高出的部份加到低位。
//下边的算法是在累加的过程中只要有进位就加到最低位
U8 i;
U16 tem=0;
//这里一定要注意ICMP的存储区，如果弄错了，呵呵，你的校验码就算不对了。
//如果不确定ICMP的存储区就不要指定指针的存储区，定义成U16  *p=(U16  *)&ICMP；
//这样效率会低一点点，但最多只用原来代码的44%的运行时间
   U16 xdata *p=(U16 xdata *)&ICMP;
    for(i=20;i>0;i--)
    {
        tem+=*p++;
        if(CY==1)
            tem++;//tem+=(U8)CY; 改成这样还能再省点时间。
    }
    tem=~tem;
    return tem;//取反，同一批数据，用了615个时钟周期



}

/********************************************/
/* 功能：组ICMP_Ping_Send数据包函数       	*/
/* 输入：无									*/
/* 输出：无									*/
/********************************************/

void ICMP_Ping_Send(void)
{
U16 tem;
//填充ICMP 40
ICMP_Ping_Copy();//组包
tem=ICMP_Check_Code();//得到检验和
ICMP[2]=tem/256;
ICMP[3]=tem%256;//检验和稍后补充
}

/********************************************/
/* 功能：组ICMP_Ping_Answer_Send数据包函数  */
/* 输入：无									*/
/* 输出：无									*/
/********************************************/

void ICMP_Ping_Answer_Send(void)
{
U16 tem;
//直接复制收到的ICMP 40
ICMP_Ping_Answer();//组包
tem=ICMP_Check_Code();//得到检验和
ICMP[2]=tem/256;
ICMP[3]=tem%256;//检验和稍后补充
}

