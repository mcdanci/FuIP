#include "string.h"
#include "fu_app.h"
#include "fu_config.h"
#include "mcuinit.h"
#include "delay.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_send.h"
#include "fu_receive.h"
#include "fu_http.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 用于处理接收到TCP,UDP数据后的识别，执行（自定义）         		*/
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.2 2014年12月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

/*******************************************/
/* 功能：处理UDP包               		   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：有效数据从DATA[42]开始	    	   */
/*******************************************/
void App_UDP_Bag(void)
{
	//命令识别
	 if (memcmp(&DATA[42],"open",4) ==0)
	 {
	 if(DATA[46]=='1')
	    led0=0;
	  if(DATA[46]=='2')
	    led1=0;
	  if(DATA[46]=='3')
	    led2=0;
	  if(DATA[46]=='4')
	    led3=0;
	
	   memcpy(DATA,"opened!",7);
		Send_UDP_Data_Bag(7);//发送UDP数据包 
	 }
	 	 


	   if (memcmp(&DATA[42],"close",5) ==0)
	   {

	  if(DATA[47]=='1')
	    led0=1;
	  if(DATA[47]=='2')
	    led1=1;
	  if(DATA[47]=='3')
	    led2=1;
	  if(DATA[47]=='4')
	    led3=1; 

        memcpy(DATA,"closed!",7);
		Send_UDP_Data_Bag(7);//发送UDP数据包
		}

	



}

/*******************************************/
/* 功能：处理TCP包               		   */
/* 输入：无					               */
/* 输出：无							       */
/* 备注：有效数据从DATA[54]开始  		   */
/*******************************************/
void App_TCP_Bag(void)
{
	
		char datlen=0;

	 /* 普通TCP数据 */
	 if (memcmp(&DATA[54],"open",4) ==0)	
	   {

		  if(DATA[58]=='1')
		    led0=0;
		  if(DATA[58]=='2')
		    led1=0;
		  if(DATA[58]=='3')
		    led2=0;
		  if(DATA[58]=='4')
		    led3=0;
		    
			TCP_Flag=(TCP_ACK|TCP_PSH);//置位标识
			 memcpy(DATA,"opened!",7);//复制数据到发送数组
		   	Send_TCP_Bag(7);	
		 
		}
		
	   if (memcmp(&DATA[54],"close",5) ==0)
	   {

		  if(DATA[59]=='1')
		    led0=1;
		  if(DATA[59]=='2')
		    led1=1;
		  if(DATA[59]=='3')
		    led2=1;
		  if(DATA[59]=='4')
		    led3=1;
			
			TCP_Flag=(TCP_ACK|TCP_PSH);//置位标识
			 memcpy(DATA,"closed!",7);//复制数据到发送数组
		   	Send_TCP_Bag(7); 
		}

	   	/* 网页web命令识别 */
	   //index
	   if (memcmp(&DATA[54],"GET / ",6) ==0)
	   {
		 if(TCP_Buf==0)//上次已经发完才响应
		 {
		 pSend=fu_index;
		 TCP_Send_Over(pSend,253);
		 }
	   }
       //open
	   else if (memcmp(&DATA[54],"GET /open",9) ==0)
	   {
		 led1=0;
		 led2=0;
		 if(TCP_Buf==0)
		 {
		 pSend=fu_open;
		 TCP_Send_Over(pSend,120);
		 }
	   }
	   //close
	   else if (memcmp(&DATA[54],"GET /close",10) ==0)
	   {
		 led1=1;
		 led2=1;
		 if(TCP_Buf==0)
		 {
		  pSend=fu_close;
		 TCP_Send_Over(pSend,119);
		 }
	   }
	  

}