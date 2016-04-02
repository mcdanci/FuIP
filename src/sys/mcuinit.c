#include <STC12C5A.H>
#include "mcuinit.h"
#include "delay.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_tcp.h"
#include "fu_send.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ��Ƭ��������ã�I/O�ڶ��壬��ʱ�������ڣ��жϵȳ�ʼ������ 	    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v1.2 2014��4��5��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

#define ETH_CPU_XTAL       		 22118400           // ����22.1184MHz
#define ETH_T0_CLOCK             ETH_CPU_XTAL / 12  // ��ʱ��0 ����12��Ƶ
#define ETH_T0_INT_RATE          100                // ��ʱ��0�ж�Ƶ�� (Hz)
#define ETH_T0_RELOAD            65536 - (ETH_T0_CLOCK / ETH_T0_INT_RATE)

U16 Timer10ms=0;//10ms��ʱ����־


/* ����0���� */
void uart0_init()
{

 	SCON = 0x50;      //REN=1�����н���״̬�����ڹ���ģʽ1    	   
	TMOD = 0x20;      //��ʱ��1������ʽ2                       
		                                                  
	TH1  = 0xFA;	  //  ������9600������λ8��ֹͣλ1��Ч��λ�� (22.1184M)
    TL1  = 0xFA; 
	TR1  = 1;         //������ʱ��1                                                      
	ES   = 1;         //�������ж�                  
	EA   = 1;  		  //�����ж�


}

/* INT0�ⲿ�ж����� */

void int0_init()
{

EX1=1;
IT1=1;//�½��ش���
EA=1;

}


/* ���ڷ��ͺ��� */
void Uart_Send(U16 len)
{
    U16 i;
	for(i=0;i<len;i++)
 	{
	SBUF = DATA[i];	        //SUBF����/���ͻ�����(�ֽд���ͨ�����⹦�ܼĴ���)
    while(!TI);				// �������ݴ���	(TI�����жϱ�־)
	TI = 0;
	}					// ������ݴ��ͱ�־
}

void Uart_Send_Byte(U8 Byte)
{
    SBUF = Byte;	        //SUBF����/���ͻ�����(�ֽд���ͨ�����⹦�ܼĴ���)
    while(!TI);				// �������ݴ���	(TI�����жϱ�־)
	TI = 0;
}


/* ��ʱ��0����*/
void  timer0_init()
{
	// ��ʱ��0��ʼ��
    // ֹͣ��ʱ��0                                     
    TR0  = 0;          
    // ���ö�ʱ��0������ʽ1     
    TMOD &= 0xF0;
    TMOD |= 0x01;

    // ���������ֵ    
    TH0 = ETH_T0_RELOAD >> 8; 
    TL0 = ETH_T0_RELOAD;
    TR0 = 1; // ����ʱ��0
    ET0 = 1; // �򿪶�ʱ��0�ж�  
    EA  = 1; // �����ж�
 }			  

/*��ʱ��0�жϣ��ж��Ƿ�ʱ*/
void timer0_isr(void) interrupt 1 using 1
{
    // ���������ֵ   
    TH0 = ETH_T0_RELOAD >> 8;
    TL0 = ETH_T0_RELOAD;

	if( Timer10ms>0 )//fuip��ʱ��ѯ
		Timer10ms--;

	if(TCP_Clear>0)//ʱ�䵽�˾����״̬
	    {
		TCP_Clear--;
			if(TCP_Clear==0)
			 {
			 TCP_Sended=0x00;
			 TCP_Connected=0x00;
			 }
		 
		}


}


/* ���ݰ���INT0 �жϺ���  */
void int0_isr(void) interrupt 2 using 2
{

}


// �����ж�
void uart_isr(void) interrupt 4 using 3
{
RI=0;
} 