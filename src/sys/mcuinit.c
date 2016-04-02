#include <STC12C5A.H>
#include "mcuinit.h"
#include "delay.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_tcp.h"
#include "fu_send.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 单片机相关配置，I/O口定义，定时器，串口，中断等初始化函数 	    */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v1.2 2014年4月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

#define ETH_CPU_XTAL       		 22118400           // 晶振22.1184MHz
#define ETH_T0_CLOCK             ETH_CPU_XTAL / 12  // 定时器0 晶振12分频
#define ETH_T0_INT_RATE          100                // 定时器0中断频率 (Hz)
#define ETH_T0_RELOAD            65536 - (ETH_T0_CLOCK / ETH_T0_INT_RATE)

U16 Timer10ms=0;//10ms定时器标志


/* 串口0配置 */
void uart0_init()
{

 	SCON = 0x50;      //REN=1允许串行接受状态，串口工作模式1    	   
	TMOD = 0x20;      //定时器1工作方式2                       
		                                                  
	TH1  = 0xFA;	  //  波特率9600、数据位8、停止位1。效验位无 (22.1184M)
    TL1  = 0xFA; 
	TR1  = 1;         //开启定时器1                                                      
	ES   = 1;         //开串口中断                  
	EA   = 1;  		  //开总中断


}

/* INT0外部中断配置 */

void int0_init()
{

EX1=1;
IT1=1;//下降沿触发
EA=1;

}


/* 串口发送函数 */
void Uart_Send(U16 len)
{
    U16 i;
	for(i=0;i<len;i++)
 	{
	SBUF = DATA[i];	        //SUBF接受/发送缓冲器(又叫串行通信特殊功能寄存器)
    while(!TI);				// 等特数据传送	(TI发送中断标志)
	TI = 0;
	}					// 清除数据传送标志
}

void Uart_Send_Byte(U8 Byte)
{
    SBUF = Byte;	        //SUBF接受/发送缓冲器(又叫串行通信特殊功能寄存器)
    while(!TI);				// 等特数据传送	(TI发送中断标志)
	TI = 0;
}


/* 定时器0配置*/
void  timer0_init()
{
	// 定时器0初始化
    // 停止定时器0                                     
    TR0  = 0;          
    // 设置定时器0工作方式1     
    TMOD &= 0xF0;
    TMOD |= 0x01;

    // 重新载入初值    
    TH0 = ETH_T0_RELOAD >> 8; 
    TL0 = ETH_T0_RELOAD;
    TR0 = 1; // 开定时器0
    ET0 = 1; // 打开定时器0中断  
    EA  = 1; // 开总中断
 }			  

/*定时器0中断，判断是否超时*/
void timer0_isr(void) interrupt 1 using 1
{
    // 重新载入初值   
    TH0 = ETH_T0_RELOAD >> 8;
    TL0 = ETH_T0_RELOAD;

	if( Timer10ms>0 )//fuip定时轮询
		Timer10ms--;

	if(TCP_Clear>0)//时间到了就清除状态
	    {
		TCP_Clear--;
			if(TCP_Clear==0)
			 {
			 TCP_Sended=0x00;
			 TCP_Connected=0x00;
			 }
		 
		}


}


/* 数据包，INT0 中断函数  */
void int0_isr(void) interrupt 2 using 2
{

}


// 串口中断
void uart_isr(void) interrupt 4 using 3
{
RI=0;
} 