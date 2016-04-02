#ifndef __mcuinit_H__
#define __mcuinit_H__

#include "type.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ��Ƭ��������ã�I/O�ڶ��壬��ʱ�������ڣ��жϵȳ�ʼ������ 	    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v1.3 2014��6��16��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

/* ���ڵ�Ƭ�� I/O�ڵ��趨*/

sbit CSN  = P1^4 ;// enc28j60-- CS
sbit SIN  = P1^5 ;// MOSI
sbit SON  = P1^6 ;// MISO
sbit SCKN = P1^7 ;// SCK

//sbit RSTN = P1^3 ;// RST ���о�����λ��ʽ


//�Լ�������
sbit led0=P2^0;
sbit led1=P2^1;
sbit led2=P2^2;
sbit led3=P2^3;

extern U16 Timer10ms;

/* MCU�Ĵ������� */
void mcu_set();
/* INT0�ⲿ�ж����� */
void int0_init();
/* ����0���� */
void uart0_init();
/* ��ʱ��0����*/
void  timer0_init();


/* ���ڷ��ͺ��� */
void Uart_Send(U16 len);
void Uart_Send_Byte(U8 Byte);

#endif