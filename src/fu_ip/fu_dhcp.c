#include "string.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "mcuinit.h"
#include "delay.h"
#include "fu_udp.h"
#include "fu_dhcp.h"
#include "fu_send.h"
#include "fu_receive.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* �����Զ���ȡIP                                            		*/
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.3 2015��1��12��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

U8 xdata DHCP_Data[34]={0x01,0x01,0x06,0x00,0x45,0x7C,0x33,0xF5,0x00,0x00,0x80,0x00};
U8 code DHCP_Zero[202]={0};
U8 code DHCP_Msg[24]={0x63,0x82,0x53,0x63,0x35,0x01,0x00,0x0c,0x0f,0x46,0x75,0x68,
0x6F,0x6D,0x65,0x2E,0x6E,0x65,0x74,0x2F,0x46,0x75,0x49,0x50};
U8 DHCP_Flag=0;
U8 DHCP_OK=0;
U16	DHCP_Time=0;
U8 DHCP_Which=0;
U8 DHCP_Client[4]={0,0,0,0};
U8 DHCP_Server[4]={0,0,0,0};
/*******************************************/
/* ���ܣ�1������discover��ͷ          	   */
/* ���룺��					               */
/* �������							       */
/* ��ע��                                  */
/*******************************************/

void DHCP_Head_Discover()
{
  DHCP_Which=1;//discover
  memcpy(&DHCP_Data[28],FU_MAC,6);//���Ʊ���MAC
}

/*******************************************/
/* ���ܣ�3������request��ͷ          	   */
/* ���룺��					               */
/* �������							       */
/* ��ע��                                  */
/*******************************************/

void DHCP_Head_Requsest()
{
  DHCP_Which=3;//request
  memcpy(&DHCP_Data[28],FU_MAC,6);//���Ʊ���MAC
}

/*******************************************/
/* ���ܣ�1������discover��          	   */
/* ���룺��					               */
/* �������							       */
/* ��ע��                                  */
/*******************************************/

void DHCP_Send_Discover()
{
UDP_Port=68;//UDP���ض˿�
UDP_Re_Port=67;//UDPԶ�̶˿�
DHCP_Head_Discover();
Uart_Send_Byte(0xce);
memcpy(DATA,DHCP_Data,34);//�����ݸ��Ƶ�������
memcpy(&DATA[34],DHCP_Zero,202);//�����ݸ��Ƶ�������
memcpy(&DATA[236],DHCP_Msg,24);//�����ݸ��Ƶ�������
DATA[242]=DHCP_Which;
DATA[260]=0xff;
Send_UDP_Data_Bag(261);//����UDP���ݰ�
}

/*******************************************/
/* ���ܣ�3������request��            	   */
/* ���룺IP					               */
/* �������							       */
/* ��ע��                                  */
/*******************************************/

void DHCP_Send_Request(void)
{
UDP_Port=68;//UDP���ض˿�
UDP_Re_Port=67;//UDPԶ�̶˿�
DHCP_Head_Requsest();
memcpy(DATA,DHCP_Data,34);//�����ݸ��Ƶ�������
memcpy(&DATA[34],DHCP_Zero,202);//�����ݸ��Ƶ�������
memcpy(&DATA[236],DHCP_Msg,24);//�����ݸ��Ƶ�������
DATA[242]=DHCP_Which;
DATA[261]=0x32;
DATA[262]=0x04;
memcpy(&DATA[263],DHCP_Client,4);//�����ݸ��Ƶ�������
DATA[267]=0xff;
Send_UDP_Data_Bag(268);//����UDP���ݰ�
}


