/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ������   					                 				    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.2 2014��10��5��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

#include <STC12C5A.H>
#include "string.h"
#include "mcuinit.h"
#include "type.h"
#include "delay.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_arp.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_send.h"
#include "fu_receive.h"
#include "fu_app.h"
#include "fu_http.h"

main()
{

//����״̬����ʼ��
TCP_Connected=0x00;
TCP_Sended=0x00;
TCP_Clear=0x00;//tcp ״̬���
ARP_Flag=0x00;
Timer10ms=0;//ʱ������


/* Ӳ����ʼ�� */
enc28j60_init();
/* ����0���� */
uart0_init();
/* ��ʱ��0,10ms���� */ 
timer0_init();
/* �ⲿ�ж�0 */
int0_init();


/* ��������UDP���� 
Set_Re_IP(192,168,1,101);//����Զ��IP
UDP_Port=8951;//UDP���ض˿�
UDP_Re_Port=2014;//UDPԶ�̶˿�
memcpy(DATA,"HELLO!",6);//�����ݸ��Ƶ�������
Send_UDP_Data_Bag(6);//����UDP���ݰ�
*/


	while(1)
	{	
	Data_Receive();
	} 
} 
