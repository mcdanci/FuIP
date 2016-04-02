#ifndef __mcuinit_H__
#define __mcuinit_H__

#include "type.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 单片机相关配置，I/O口定义，定时器，串口，中断等初始化函数 	    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v1.3 2014年6月16日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

/* 关于单片机 I/O口的设定*/

sbit CSN  = P1^4 ;// enc28j60-- CS
sbit SIN  = P1^5 ;// MOSI
sbit SON  = P1^6 ;// MISO
sbit SCKN = P1^7 ;// SCK

//sbit RSTN = P1^3 ;// RST 自行决定复位方式


//自己定义啦
sbit led0=P2^0;
sbit led1=P2^1;
sbit led2=P2^2;
sbit led3=P2^3;

extern U16 Timer10ms;

/* MCU寄存器配置 */
void mcu_set();
/* INT0外部中断配置 */
void int0_init();
/* 串口0配置 */
void uart0_init();
/* 定时器0配置*/
void  timer0_init();


/* 串口发送函数 */
void Uart_Send(U16 len);
void Uart_Send_Byte(U8 Byte);

#endif