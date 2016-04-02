#include "fu_udp.h"
#include "fu_config.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ��UDP�ײ���   					                 			    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.1 2014��10��15��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

U16 UDP_Port=8951;//����UDP �˿ں�
U16 UDP_Re_Port=0;//Ŀ��UDP �˿ں�
U8 xdata UDP_False[12]={0};//UDPα�ײ���12�ֽ�
U8 xdata UDP_Head[8]={0};//UDP�ײ�������8�ֽ�

/*******************************************/
/* ���ܣ�UDP α�ײ���� 12�ֽ�      	   */
/* ���룺UDP���ݰ�����				       */
/* �������							       */
/*******************************************/
void UDP_Flase_Copy(U16 len)//len Ϊż��
{
U8 i;
for(i=0;i<4;i++)
UDP_False[0+i]=FU_LoaclIP[i];//ԴIP

for(i=0;i<4;i++)
UDP_False[4+i]=Re_IP[i];//Ŀ��IP

UDP_False[8]=0;
UDP_False[9]=17;
UDP_False[10]=(8+len)/256;//UDP�ײ���+���ݳ�
UDP_False[11]=(8+len)%256;//UDP�ײ���+���ݳ�
}

/*******************************************/
/* ���ܣ�UDP �ײ����  8�ֽ�       		   */
/* ���룺UDP���ݰ�����                     */
/* �������							       */
/*******************************************/
void UDP_Head_Copy(U16 len)
{
UDP_Head[0]=UDP_Port/256;
UDP_Head[1]=UDP_Port%256;//Դ�˿�
UDP_Head[2]=UDP_Re_Port/256;
UDP_Head[3]=UDP_Re_Port%256;//Ŀ�Ķ˿�
UDP_Head[4]=(len+8)/256;
UDP_Head[5]=(len+8)%256;//UDP����=UDP�ײ�+����
UDP_Head[6]=0;
UDP_Head[7]=0;//������Ժ󲹳�
}

/********************************************/
/* ���ܣ�����ʹ�����           			*/
/* ���룺��									*/
/* �����16λUDP�����						*/
/********************************************/

U16 UDP_Check_Code(U16 len)
{
 U16 i;
 U16 tem;
 U32 checkcode=0;//�����

/* �������� */

	for(i=0;i<12;i=i+2)//UDPα�ײ�12
	{
	tem=UDP_False[i]*256;
	checkcode+=tem+UDP_False[i+1];
	}
 	for(i=0;i<8;i=i+2)//UDP�ײ�8
	{
	tem=UDP_Head[i]*256;
	checkcode+=tem+UDP_Head[i+1];
	}
	
	if(len%2==0)//UDP���ݳ�����ż��
	{
		for(i=0;i<len;i=i+2)
	    {   
		tem=DATA[i]*256;
		checkcode+=tem+DATA[i+1];
		}
	}
	else//UDP���ݳ���������
	{	 
	     DATA[len]=0;
	   for(i=0;i<len+1;i=i+2)
	    {   
		tem=DATA[i]*256;
		checkcode+=tem+DATA[i+1];
		}
	
	}
	
/*�����λ����Ҫ�ӵ�β�� */
	if(checkcode>0xFFFF)
	{
	tem=(U16)(checkcode>>16);//�õ���λ
	checkcode=checkcode&0xFFFF;//ȥ����λ
    checkcode=checkcode+tem;//�ӵ���λ

	tem=(U16)(checkcode>>16);//�õ���λ
    checkcode=checkcode+tem;//�ӵ���λ
	}
	tem=(U16)checkcode;
	tem=~tem;
return tem;//ȡ��
}

/********************************************/
/* ���ܣ���UDP���ݰ�����           			*/
/* ���룺UDP ���ݳ��� len      			    */
/* �������									*/
/********************************************/

void UDP_Send(U16 len)
{
U16 tem;
//���UDPα�ײ� 12�ֽ�
UDP_Flase_Copy(len);//len Ϊż��
//UDP �ײ����  8�ֽ�
UDP_Head_Copy(len);

tem=UDP_Check_Code(len);//�õ������

UDP_Head[6]=tem/256;
UDP_Head[7]=tem%256;//������Ժ󲹳�
}


