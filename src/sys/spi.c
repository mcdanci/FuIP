#include <STC12C5A.H>

#include "spi.h"
#include "mcuinit.h"
#include"delay.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* SPI读写函数，移植时，无论是硬件SPI还是模拟，均可      			*/
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v1.2 2014年4月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

void WriteByte(U8 temp)
{
  U8 i;

  for(i=0;i<8;i++)
  {
	SCKN=0;//Delay_10us();
    SIN = temp & 0x80;
	SCKN=1;//Delay_10us();
	temp <<=1;
  }
  SCKN=0;
  
}

U8 ReadByte(void)
{
  U8 i,dat1;

  SCKN=0;dat1=0;
  for(i=0;i<8;i++)
  {	
	SCKN=1;//Delay_10us();
	dat1 <<=1;
    dat1 |= SON; 
	SCKN=0;	//Delay_10us();
  }
  
  return dat1;
}

