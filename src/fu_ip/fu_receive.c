#include <STC12C5A.H>
#include "string.h"
#include "mcuinit.h"
#include "fu_receive.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_arp.h"
#include "fu_send.h"
#include "fu_icmp.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_app.h"
#include "fu_http.h"
#include "fu_dhcp.h"
/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 接收以太网数据报，并识别类别ARP请求，TCP请求，TCP,UDP数据	    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v2.3 2015年1月12日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

/********************************************/
/* 功能：接收数据，存入缓冲区           	*/
/* 输入：无								    */
/* 输出：标志  								*/
/********************************************/
U8 Data_Receive(void)
{  U16 len;
   U8 temp=0xFF;
   len=enc28j60PacketReceive(FU_LEN, DATA);	 //接收数据
   //Uart_Send(len);//串口发送收到的网卡数据，用于调试使用
   // 判断数据包类型然后交由处理
   if(len>18)//确保更新包了才进行下列处理，18是最小数据包 14 以太网首部+4 CRC
   { 
   if(DATA[13]==0x06)//ARP包
   		temp=ARP_Answer();//被问，对方回答
   if(DATA[13]==0x00)//IP包
        temp=IP_Answer(len);//3 ICMP  4 TCP  5UDP 
	
  Uart_Send_Byte(temp);//串口发送状态量，用于调试

   }
   
   return temp;  
}

/*******************************************/
/* 功能：处理ARP数据报      		       */
/* 输入：无					               */
/* 输出：0 本机ARP请求成功了 1应答别人     */
/*******************************************/
U8 ARP_Answer(void)
{
  U8 flag=0,i;
	 //被问
	 if(DATA[21]==1)
	 {
			
	   for(i=0;i<4;i++)
	 	{
		if(FU_LoaclIP[i]!=DATA[38+i])
		flag=1;//不是问本机IP
	 	}

		//如果是问本机，则应答
		if(flag==0)
		{   

		for(i=0;i<6;i++)
		 	{
			Re_MAC[i]=DATA[6+i];
			}
		for(i=0;i<4;i++)
		 	{
			Re_IP[i]=DATA[28+i];//远程主机IP
			}

		Answer_ARP_Bag();//应答包
		
		return 0x20;//回答别人
		}
	  	
	 }
	 
	 //得到对方回答
	 if(DATA[21]==2)
	  {
	  
			
	   for(i=0;i<4;i++)
	 	{
		if(FU_LoaclIP[i]!=DATA[38+i])
		flag=1;//不是问本机IP
	 	}

		//如果是回答给本机的，存起来MAC
		if(flag==0)
		{   

			for(i=0;i<6;i++)
		 	{
			Re_MAC[i]=DATA[6+i];
			}
			ARP_Flag=0x21;
			return 0x21;//得到回答
		
		}
	  
	  }
	  return 2;
}

/*******************************************/
/* 功能：处理IP数据报      		           */
/* 输入：DATA>54就是有数据的 			   */
/* 输出：3 ICMP 4 TCP 5 UDP	其他6          */
/*******************************************/
U8 IP_Answer(U16 len)
{
	U8 flag=0,tcpflag=0,i;
	U32 temp=0;
	U16 Port_tem;
		
		
		/* 判断是不是DHCP */
		if(0x11==DATA[23]&&DHCP_OK==0)
		{
		   //UDP_Port
		    Port_tem=DATA[34]*256+DATA[35];//远程端口
			if(Port_tem==67&&DHCP_Flag!=0x03)//为DHCP包
			{
			     flag=0;
				 for(i=0;i<4;i++)
				 {
					if(DHCP_Data[4]!=DATA[46])
					{
					flag=1;//不是问本机
					return 0x71;
					}
				 } 
				 	 
				//收到OFFER
				if(DHCP_Flag==0&&DATA[284]==2)
					{ 
					   DHCP_Flag=1;
					   for(i=0;i<4;i++)
					 	{
						DHCP_Server[i]=DATA[26+i];//远程主机(网关)IP
						}
	
						//有效数据从DATA[42]开始
						for(i=0;i<4;i++)
					 	{
						DHCP_Client[i]=DATA[58+i];//分配给本机IP
						}
					  return 0x72;	
					}
				 //收到ACK
				 if(DHCP_Flag==2&&DATA[284]==5)
					{ 
					    DHCP_Flag=3;  
					    for(i=0;i<4;i++)
					 	{
						FU_GateWay[i]=DATA[26+i];//远程主机(网关)IP
						}
	
						//有效数据从DATA[42]开始
						for(i=0;i<4;i++)
					 	{
						FU_LoaclIP[i]=DATA[58+i];//分配给本机IP
						}
                        
                        DHCP_OK=1;
						return 0x73;		
					}
				  //收到NSK
				  if(DHCP_Flag==2&&DATA[284]==6)
					 {
					 DHCP_Flag=0x00;
					 return 0x74;
					 }

			  return 0x75;
			}
		 	  
		   return 0x70;
		}//DHCP结束


	   flag=0;
	   for(i=0;i<6;i++)
	 	{
		if(FU_MAC[i]!=DATA[i])//认以太网首部即可
		flag=1;//不是问本机MAC
	 	}
			
	   for(i=0;i<4;i++)
	 	{
		if(FU_LoaclIP[i]!=DATA[30+i])
		flag=1;//不是问本机IP
	 	}

		//IP检验和暂不检验
		
		

		//如果是回答给本机的，存起来
		if(flag==0)
		{
			 //TCP检验和，TCP_Port暂不检验
		     
		 for(i=0;i<6;i++)
		 	{
			Re_MAC[i]=DATA[6+i];//远程主机MAC
			}
		 for(i=0;i<4;i++)
		 	{
			Re_IP[i]=DATA[26+i];//远程主机IP
			}
		

			//3 ICMP(仅识别ping请求) 
			if(0x01==DATA[23]&&0x08==DATA[34])
			{
			 //回应ping响应
			 Send_ICMP_Ping_Back_Bag();
			 return 3;

			}
			//4 TCP 
			if(0x06==DATA[23])
			{

			  
			  /* 判断TCP各种包 */
			   	TCP_Re_Port=DATA[34]*256+DATA[35];//远程端口
				
				TCP_Port=DATA[36]*256+DATA[37];//本地端口

				if(TCP_Port==80)
				   TCP_Wflag=1;//网页
				else
				   TCP_Wflag=0;//普通
			  /* 处理TCP各个情况的过程 */

			    //跟本机握手包(1)
				if(DATA[47]==TCP_SYN)
				 {
				  TCP_Mark=0x1200;//序号

				  temp+=(U32)(DATA[38]*16777216);
				  temp+=(U32)(DATA[39]*65536);
				  temp+=(U32)(DATA[40]*256);
				  temp+=(U32)(DATA[41]);
				  TCP_CMark=temp+1;//确认序号+1

				  TCP_Connected=0x02;//被握手第二包
				  
				  //发送第2次回应人家的握手包
				  TCP_Flag=(TCP_ACK|TCP_SYN);
				  Send_TCP_Bag(0);

				  return 0x41;
				 }

 				//收到数据包(4)
				if((DATA[47]&TCP_PSH)||(len>100))
				 {
				  temp=0;
				  temp+=(U32)(DATA[42]*16777216);
				  temp+=(U32)(DATA[43]*65536);
				  temp+=(U32)(DATA[44]*256);
				  temp+=(U32)(DATA[45]);
				  TCP_Mark=temp;//序号 

				  temp=0;
				  temp+=(U32)(DATA[38]*16777216);
				  temp+=(U32)(DATA[39]*65536);
				  temp+=(U32)(DATA[40]*256);
				  temp+=(U32)(DATA[41]);
				  TCP_CMark=temp+(DATA[16]*256+DATA[17]-40);//确认序号=加字长				  

				   TCP_Flag=TCP_ACK;
				   Send_TCP_Bag(0);
                 
				   App_TCP_Bag();//处理数据做出动作


				 // Uart_Send_Byte(0xE4);//串口发送状态量，用于调试
				 return 0x44;
				 }

				   //收到发出的数据的回应(5)
				 if((TCP_Sended==1)&&(DATA[47]&TCP_ACK))
				 {
				  
				  TCP_Sended=2;//发送成功
				  temp=0;
				  temp+=(U32)(DATA[42]*16777216);
				  temp+=(U32)(DATA[43]*65536);
				  temp+=(U32)(DATA[44]*256);
				  temp+=(U32)(DATA[45]);
				  TCP_Mark=temp;//序号 

				  temp=0;
				  temp+=(U32)(DATA[38]*16777216);
				  temp+=(U32)(DATA[39]*65536);
				  temp+=(U32)(DATA[40]*256);
				  temp+=(U32)(DATA[41]);
				  TCP_CMark=temp;//确认序号照抄

				  if(TCP_Buf==0&&TCP_Wflag==1)//如果网页连接，且对方收到最后一包数据
				   	{
					TCP_Flag=(TCP_ACK|TCP_FIN);//最后一包
					TCP_Connected=0x01;//断开
					Send_TCP_Bag(0);

			    	//	Uart_Send_Byte(0xF5);//串口发送状态量，用于调试

					TCP_Clear=20;//200ms之后清除

					}

				   // Uart_Send_Byte(0xE5);//串口发送状态量，用于调试
				   
					TCP_Send_Over(pSend,0);


				 
				  return 0x45;
				 }

				 //收到断开连接(6)
				 if((DATA[47]&TCP_FIN)==TCP_FIN) 
				   
				   {
					  temp=0;
					  temp+=(U32)(DATA[42]*16777216);
					  temp+=(U32)(DATA[43]*65536);
					  temp+=(U32)(DATA[44]*256);
					  temp+=(U32)(DATA[45]);
					  TCP_Mark=temp;//序号 
	
					  temp=0;
					  temp+=(U32)(DATA[38]*16777216);
					  temp+=(U32)(DATA[39]*65536);
					  temp+=(U32)(DATA[40]*256);
					  temp+=(U32)(DATA[41]);
					  TCP_CMark=temp+1;//确认序号
					  
					  if(TCP_Connected==0x01)//本机主动断开的
					  {
					  TCP_Connected=0x00;//断开
					  //发送断开握手的包
	 				  TCP_Flag=TCP_ACK;
					  Send_TCP_Bag(0);
					  //Uart_Send_Byte(0xF6);
					  }
					  else//对方断开
					  {
					   TCP_Connected=0x01;//断开
					   TCP_Flag=TCP_ACK;  
					   Send_TCP_Bag(0);

					   TCP_Flag=(TCP_ACK|TCP_FIN);
					   Send_TCP_Bag(0);
					   //Uart_Send_Byte(0xE6);
					  }
	
					 
				   	 return 0x46;
				   }
				  
				  //重置连接(0)
				if((DATA[47]&TCP_RST)==TCP_RST)
				{
				  temp=0;
				  temp+=(U32)(DATA[42]*16777216);
				  temp+=(U32)(DATA[43]*65536);
				  temp+=(U32)(DATA[44]*256);
				  temp+=(U32)(DATA[45]);
				  TCP_Mark=temp;//序号 

				  TCP_CMark=0;//确认序号
				  TCP_Connected=0x00;
				  TCP_Sended=0x00;
				 return 0x40;
				}  


			   }//TCP结束
 
			 //5 UDP 
			if(0x11==DATA[23])
			{
			   //UDP检验和，UDP_Port暂不检验
			    UDP_Re_Port=DATA[34]*256+DATA[35];//远程IP
				UDP_Port=DATA[36]*256+DATA[37];//本地IP
			  	  App_UDP_Bag();//处理数据做出动作
			 	  return 0x05;

			}//UDP结束


			 return 0x04;//不是 TCP UDP ICMP_Ping
		  }
	return 0x06;//不是给自己的包
}


/*******************************************/
/* 功能:固定数据如:网页数据发送完成与否函数*/
/* 输入：0 此处不需要加塞，N<255加塞       */
/* 输出：                                  */
/*******************************************/

void TCP_Send_Over(char* p,U16 len)
{	 

	 if(len==0)//发送剩余数据
	 {
	      if(TCP_Buf==0)
		  return ;//直接退出
	      
		  //Uart_Send_Byte(0xEE);
		   
	      TCP_Flag=(TCP_ACK|TCP_PSH);

		   //如果长度不足，直接发
		   if(TCP_Buf<=254)
		   {
		    
			memcpy(DATA,p+TCP_Haved,TCP_Buf);//复制数据到发送数组
		   	Send_TCP_Bag(TCP_Buf);

			 
			 TCP_Haved+=TCP_Buf;
			 TCP_Buf=0;
			 TCP_Sended=1;
		   }
		   else	//长度大于
		   {
		    memcpy(DATA,p+TCP_Haved,254);//复制数据到发送数组
			Send_TCP_Bag(254);

			TCP_Haved+=254;
			TCP_Buf-=254;
			TCP_Sended=1;
			}
	 }
	else
	 { 
	   TCP_Buf=len;
	   TCP_Haved=0;

	  // Uart_Send_Byte(0xEE);

       TCP_Flag=(TCP_ACK|TCP_PSH);
		 

		   //如果长度不足，直接发
		   if(len<=254)
		   {

		    memcpy(DATA,p,len);//复制数据到发送数组
		   	Send_TCP_Bag(len);

			 TCP_Haved=len;
			 TCP_Buf=0;			 
			 TCP_Sended=1;
		    }
		   else	//长度大于
		   {
		    memcpy(DATA,p,254);//复制数据到发送数组
			Send_TCP_Bag(254);	
			TCP_Haved+=254;
			TCP_Buf=len-254;
			TCP_Sended=1;
		   }

	 }

}
