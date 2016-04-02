#include "fu_ip.h"
#include "fu_config.h"

/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* IP�ײ�����������TCP UDP ICMP����             				    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v1.2 2014��4��5��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

U8 xdata IP_Head[20]={0};//IP�ײ�20�ֽ�
U16 xdata IP_Mark=0x1200;//IP�ײ��ı�ʾ�ֶ�

/*******************************************/
/* ���ܣ����IP�ײ�20�ֽ�      		       */
/* ���룺IP���ݱ�����0 UDP,1 TCP,2 ICMP	   */
/* ���룺IP���ݱ�����UDP20+8+len ICMP20+len*/
/* �������							       */
/*******************************************/

void IP_Head_Copy(U8 leixing,U16 IP_len)
{

U8 i;
IP_Head[0]=0x45;//�汾 �ײ�����
IP_Head[1]=0;//TOS һ�����
IP_Head[2]=IP_len/256;
IP_Head[3]=IP_len%256;//len=20+8+UDP_len,16λIP���ܳ���,20+40ICMP_Len ,20+20+TCP_Len
IP_Mark++;//��ʾ�ֶ�+1��Ψһ�ֶ�
if(IP_Mark==0xffff)
IP_Mark=0x1200;
IP_Head[4]=IP_Mark/256;
IP_Head[5]=IP_Mark%256;
IP_Head[6]=0;
IP_Head[7]=0;//��־��ƫ�ƣ���Ƭ
IP_Head[8]=FU_TTL;//��������

if(leixing==0)
IP_Head[9]=0x11;//17 UDP���ݰ�
if(leixing==1)
IP_Head[9]=0x06;//6 TCP���ݰ�
if(leixing==2)
IP_Head[9]=0x01;//01 ICMP

IP_Head[10]=0;
IP_Head[11]=0;//�����

//ԴIP
for(i=0;i<4;i++)
	IP_Head[12+i]=FU_LoaclIP[i];
//Ŀ��IP
for(i=0;i<4;i++)
	IP_Head[16+i]=Re_IP[i];


}

/********************************************/
/* ���ܣ�IP�ײ� ����ʹ�����           	*/
/* ���룺��									*/
/* �����16λUDP�����						*/
/********************************************/
U16 IP_Check_Code()
{
U8 i;
U16 tem;
U32 checkcode=0;

	for(i=0;i<20;i=i+2)
	{
	tem=IP_Head[i]*256;
	checkcode+=tem+IP_Head[i+1]; 
	}
	
/* �����λ����Ҫ�ӵ�β��*/ 
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
return tem;//ȡ��	 +

}

/********************************************/
/* ���ܣ�IP�ײ����                      	*/
/* ���룺IP������ 20+8+len					*/
/* �������					 				*/
/********************************************/
void IP_Send(U8 leixing,U16 IP_len)
{
U16 tem;
IP_Head_Copy(leixing,IP_len);
tem=IP_Check_Code();//IP�ײ������

IP_Head[10]=tem/256;
IP_Head[11]=tem%256;

}

/********************************************/
/* ���ܣ�IP�ײ��������Pingר�ã�          	*/
/* ���룺IP������ 20+8+len					*/
/* �������         						*/
/********************************************/
void IP_Send_Ping_Back(U8 leixing,U16 IP_len)
{
U16 tem;
IP_Head_Copy(leixing,IP_len);
IP_Head[4]=DATA[18];
IP_Head[5]=DATA[19];//��PingҪ����Pingһ��
tem=IP_Check_Code();//IP�ײ������

IP_Head[10]=tem/256;
IP_Head[11]=tem%256;

}								   

