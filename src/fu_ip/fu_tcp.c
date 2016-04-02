#include "fu_TCP.h"
#include "fu_config.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ��TCP�ײ�   					                 				    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.2 2014��12��5��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

U16 TCP_Port=8952;//����TCP �˿ں�
U16 TCP_Re_Port=0;//Ŀ��TCP �˿ں�
U8  xdata TCP_False[12]={0};//TCPα�ײ���12�ֽ�
U8  xdata TCP_Head[20]={0};//TCP�ײ�������20�ֽ�
U32 xdata TCP_Mark=0x1200;//TCP��ţ�Ψһ�ֶ�
U32 xdata TCP_CMark=0;//TCPȷ����ţ�Ψһ�ֶ�
U8  TCP_Flag=0;//6��λ��־
U8  TCP_Connected=0;//����״̬
U8  TCP_Sended=0;//���ͱ�ʶ
U8  TCP_Wflag=0;//�������� 0 ��ͨ 1��ҳ
U8  TCP_Clear=0;//tcp ״̬���
U16 TCP_Buf=0;//tcp���ͻ��峤��
U16 TCP_Haved=0;//tcp�Ѿ����ͳ���

/*******************************************/
/* ���ܣ�TCP α�ײ���� 12�ֽ�      	   */
/* ���룺TCP���ݰ�����				       */
/* �������							       */
/*******************************************/
void TCP_Flase_Copy(U16 len)//len Ϊż��
{
U8 i;
for(i=0;i<4;i++)
TCP_False[0+i]=FU_LoaclIP[i];//ԴIP

for(i=0;i<4;i++)
TCP_False[4+i]=Re_IP[i];//Ŀ��IP

TCP_False[8]=0;
TCP_False[9]=6;//6 TCP
TCP_False[10]=(20+len)/256;//TCP�ײ���+���ݳ�
TCP_False[11]=(20+len)%256;//TCP�ײ���+���ݳ�
}

/*******************************************/
/* ���ܣ�TCP �ײ����  20�ֽ�       	   */
/* ���룺TCP���ݰ�����                     */
/* �������							       */
/*******************************************/
void TCP_Head_Copy(void)
{
TCP_Head[0]=TCP_Port/256;
TCP_Head[1]=TCP_Port%256;//Դ�˿�
TCP_Head[2]=TCP_Re_Port/256;
TCP_Head[3]=TCP_Re_Port%256;//Ŀ�Ķ˿�
TCP_Head[4]=TCP_Mark>>24;
TCP_Head[5]=TCP_Mark>>16;
TCP_Head[6]=TCP_Mark>>8;
TCP_Head[7]=TCP_Mark;//TCP���
TCP_Head[8]=TCP_CMark>>24;
TCP_Head[9]=TCP_CMark>>16;
TCP_Head[10]=TCP_CMark>>8;
TCP_Head[11]=TCP_CMark;//TCPȷ�����
TCP_Head[12]=0x50;//TCP�ײ���
TCP_Head[13]=TCP_Flag;//TCP6��λ��־
TCP_Head[14]=(FU_LEN-58)/256;
TCP_Head[15]=(FU_LEN-58)%256;//TCP���ڴ�С FU_LEN-14-20-20-4=������յ�TCP����
TCP_Head[16]=0;
TCP_Head[17]=0;//�����
TCP_Head[18]=0;
TCP_Head[19]=0;//����ָ��
}

/********************************************/
/* ���ܣ�����ʹ�����           			*/
/* ���룺��									*/
/* �����16λTCP�����						*/
/********************************************/

U16 TCP_Check_Code(U16 len)
{
 U16 i;
 U16 tem;
 U32 checkcode=0;//�����

/* �������� */

	for(i=0;i<12;i=i+2)//TCPα�ײ�12
	{
	tem=TCP_False[i]*256;
	checkcode+=tem+TCP_False[i+1];
	}
 	for(i=0;i<20;i=i+2)//TCP�ײ�20
	{
	tem=TCP_Head[i]*256;
	checkcode+=tem+TCP_Head[i+1];
	}

	if(len%2==0)//TCP���ݳ�����ż��
	{
		for(i=0;i<len;i=i+2)
	    {   
		tem=DATA[i]*256;
		checkcode+=tem+DATA[i+1];
		}
	}
	else  //TCP���ݳ���������
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
/* ���ܣ���TCP���ݰ�����           			*/
/* ���룺TCP ���ݳ��� len 		 			*/
/* �������									*/
/********************************************/

void TCP_Send(U16 len)
{
U16 tem;
//���TCPα�ײ� 12�ֽ�
TCP_Flase_Copy(len);//len Ϊż��
//TCP �ײ����  20�ֽ�
TCP_Head_Copy();

tem=TCP_Check_Code(len);//�õ������

TCP_Head[16]=tem/256;
TCP_Head[17]=tem%256;//������Ժ󲹳�
}


