#include <STC12C5A.H>
#include "string.h"
#include "mcuinit.h"
#include "fu_receive.h"
#include "enc28j60.h"
#include "fu_config.h"
#include "fu_arp.h"
#include "fu_send.h"
#include "fu_icmp.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_app.h"
#include "fu_http.h"
#include "fu_dhcp.h"
/********************************************************************/
/* ���������ѧϰ�ο���δ�������������������κ���ҵ��;  			*/
/* FuIPרΪ��Ƭ�������Ƶ�TCP/IPЭ��ջ���������κ�8,16,32λ��Ƭ��	*/
/* �����򣬿����ڿͻ��ˣ������    					            	*/
/* ������̳��www.fuhome.net/bbs/						        	*/
/* ������̫�����ݱ�����ʶ�����ARP����TCP����TCP,UDP����	    */
/* FuIP�����ڣ��ͻ��ˣ�����ˣ���ɽ���fuhome.net ƽ̨����Զ�̿���  */ 
/* �汾��v2.3 2015��1��12��						       			 	*/
/* �����ܶ�										       			 	*/
/* ���ڸ��£������ע�������ţ����������룬����	     			 	*/
/* Copyright fuhome.net δ��֮�� ʵ���ң��ÿƼ���������				*/
/********************************************************************/

/********************************************/
/* ���ܣ��������ݣ����뻺����           	*/
/* ���룺��								    */
/* �������־  								*/
/********************************************/
U8 Data_Receive(void)
{  U16 len;
   U8 temp=0xFF;
   len=enc28j60PacketReceive(FU_LEN, DATA);	 //��������
   //Uart_Send(len);//���ڷ����յ����������ݣ����ڵ���ʹ��
   // �ж����ݰ�����Ȼ���ɴ���
   if(len>18)//ȷ�����°��˲Ž������д���18����С���ݰ� 14 ��̫���ײ�+4 CRC
   { 
   if(DATA[13]==0x06)//ARP��
   		temp=ARP_Answer();//���ʣ��Է��ش�
   if(DATA[13]==0x00)//IP��
        temp=IP_Answer(len);//3 ICMP  4 TCP  5UDP 
	
  Uart_Send_Byte(temp);//���ڷ���״̬�������ڵ���

   }
   
   return temp;  
}

/*******************************************/
/* ���ܣ�����ARP���ݱ�      		       */
/* ���룺��					               */
/* �����0 ����ARP����ɹ��� 1Ӧ�����     */
/*******************************************/
U8 ARP_Answer(void)
{
  U8 flag=0,i;
	 //����
	 if(DATA[21]==1)
	 {
			
	   for(i=0;i<4;i++)
	 	{
		if(FU_LoaclIP[i]!=DATA[38+i])
		flag=1;//�����ʱ���IP
	 	}

		//������ʱ�������Ӧ��
		if(flag==0)
		{   

		for(i=0;i<6;i++)
		 	{
			Re_MAC[i]=DATA[6+i];
			}
		for(i=0;i<4;i++)
		 	{
			Re_IP[i]=DATA[28+i];//Զ������IP
			}

		Answer_ARP_Bag();//Ӧ���
		
		return 0x20;//�ش����
		}
	  	
	 }
	 
	 //�õ��Է��ش�
	 if(DATA[21]==2)
	  {
	  
			
	   for(i=0;i<4;i++)
	 	{
		if(FU_LoaclIP[i]!=DATA[38+i])
		flag=1;//�����ʱ���IP
	 	}

		//����ǻش�������ģ�������MAC
		if(flag==0)
		{   

			for(i=0;i<6;i++)
		 	{
			Re_MAC[i]=DATA[6+i];
			}
			ARP_Flag=0x21;
			return 0x21;//�õ��ش�
		
		}
	  
	  }
	  return 2;
}

/*******************************************/
/* ���ܣ�����IP���ݱ�      		           */
/* ���룺DATA>54���������ݵ� 			   */
/* �����3 ICMP 4 TCP 5 UDP	����6          */
/*******************************************/
U8 IP_Answer(U16 len)
{
	U8 flag=0,tcpflag=0,i;
	U32 temp=0;
	U16 Port_tem;
		
		
		/* �ж��ǲ���DHCP */
		if(0x11==DATA[23]&&DHCP_OK==0)
		{
		   //UDP_Port
		    Port_tem=DATA[34]*256+DATA[35];//Զ�̶˿�
			if(Port_tem==67&&DHCP_Flag!=0x03)//ΪDHCP��
			{
			     flag=0;
				 for(i=0;i<4;i++)
				 {
					if(DHCP_Data[4]!=DATA[46])
					{
					flag=1;//�����ʱ���
					return 0x71;
					}
				 } 
				 	 
				//�յ�OFFER
				if(DHCP_Flag==0&&DATA[284]==2)
					{ 
					   DHCP_Flag=1;
					   for(i=0;i<4;i++)
					 	{
						DHCP_Server[i]=DATA[26+i];//Զ������(����)IP
						}
	
						//��Ч���ݴ�DATA[42]��ʼ
						for(i=0;i<4;i++)
					 	{
						DHCP_Client[i]=DATA[58+i];//���������IP
						}
					  return 0x72;	
					}
				 //�յ�ACK
				 if(DHCP_Flag==2&&DATA[284]==5)
					{ 
					    DHCP_Flag=3;  
					    for(i=0;i<4;i++)
					 	{
						FU_GateWay[i]=DATA[26+i];//Զ������(����)IP
						}
	
						//��Ч���ݴ�DATA[42]��ʼ
						for(i=0;i<4;i++)
					 	{
						FU_LoaclIP[i]=DATA[58+i];//���������IP
						}
                        
                        DHCP_OK=1;
						return 0x73;		
					}
				  //�յ�NSK
				  if(DHCP_Flag==2&&DATA[284]==6)
					 {
					 DHCP_Flag=0x00;
					 return 0x74;
					 }

			  return 0x75;
			}
		 	  
		   return 0x70;
		}//DHCP����


	   flag=0;
	   for(i=0;i<6;i++)
	 	{
		if(FU_MAC[i]!=DATA[i])//����̫���ײ�����
		flag=1;//�����ʱ���MAC
	 	}
			
	   for(i=0;i<4;i++)
	 	{
		if(FU_LoaclIP[i]!=DATA[30+i])
		flag=1;//�����ʱ���IP
	 	}

		//IP������ݲ�����
		
		

		//����ǻش�������ģ�������
		if(flag==0)
		{
			 //TCP����ͣ�TCP_Port�ݲ�����
		     
		 for(i=0;i<6;i++)
		 	{
			Re_MAC[i]=DATA[6+i];//Զ������MAC
			}
		 for(i=0;i<4;i++)
		 	{
			Re_IP[i]=DATA[26+i];//Զ������IP
			}
		

			//3 ICMP(��ʶ��ping����) 
			if(0x01==DATA[23]&&0x08==DATA[34])
			{
			 //��Ӧping��Ӧ
			 Send_ICMP_Ping_Back_Bag();
			 return 3;

			}
			//4 TCP 
			if(0x06==DATA[23])
			{

			  
			  /* �ж�TCP���ְ� */
			   	TCP_Re_Port=DATA[34]*256+DATA[35];//Զ�̶˿�
				
				TCP_Port=DATA[36]*256+DATA[37];//���ض˿�

				if(TCP_Port==80)
				   TCP_Wflag=1;//��ҳ
				else
				   TCP_Wflag=0;//��ͨ
			  /* ����TCP��������Ĺ��� */

			    //���������ְ�(1)
				if(DATA[47]==TCP_SYN)
				 {
				  TCP_Mark=0x1200;//���

				  temp+=(U32)(DATA[38]*16777216);
				  temp+=(U32)(DATA[39]*65536);
				  temp+=(U32)(DATA[40]*256);
				  temp+=(U32)(DATA[41]);
				  TCP_CMark=temp+1;//ȷ�����+1

				  TCP_Connected=0x02;//�����ֵڶ���
				  
				  //���͵�2�λ�Ӧ�˼ҵ����ְ�
				  TCP_Flag=(TCP_ACK|TCP_SYN);
				  Send_TCP_Bag(0);

				  return 0x41;
				 }

 				//�յ����ݰ�(4)
				if((DATA[47]&TCP_PSH)||(len>100))
				 {
				  temp=0;
				  temp+=(U32)(DATA[42]*16777216);
				  temp+=(U32)(DATA[43]*65536);
				  temp+=(U32)(DATA[44]*256);
				  temp+=(U32)(DATA[45]);
				  TCP_Mark=temp;//��� 

				  temp=0;
				  temp+=(U32)(DATA[38]*16777216);
				  temp+=(U32)(DATA[39]*65536);
				  temp+=(U32)(DATA[40]*256);
				  temp+=(U32)(DATA[41]);
				  TCP_CMark=temp+(DATA[16]*256+DATA[17]-40);//ȷ�����=���ֳ�				  

				   TCP_Flag=TCP_ACK;
				   Send_TCP_Bag(0);
                 
				   App_TCP_Bag();//����������������


				 // Uart_Send_Byte(0xE4);//���ڷ���״̬�������ڵ���
				 return 0x44;
				 }

				   //�յ����������ݵĻ�Ӧ(5)
				 if((TCP_Sended==1)&&(DATA[47]&TCP_ACK))
				 {
				  
				  TCP_Sended=2;//���ͳɹ�
				  temp=0;
				  temp+=(U32)(DATA[42]*16777216);
				  temp+=(U32)(DATA[43]*65536);
				  temp+=(U32)(DATA[44]*256);
				  temp+=(U32)(DATA[45]);
				  TCP_Mark=temp;//��� 

				  temp=0;
				  temp+=(U32)(DATA[38]*16777216);
				  temp+=(U32)(DATA[39]*65536);
				  temp+=(U32)(DATA[40]*256);
				  temp+=(U32)(DATA[41]);
				  TCP_CMark=temp;//ȷ������ճ�

				  if(TCP_Buf==0&&TCP_Wflag==1)//�����ҳ���ӣ��ҶԷ��յ����һ������
				   	{
					TCP_Flag=(TCP_ACK|TCP_FIN);//���һ��
					TCP_Connected=0x01;//�Ͽ�
					Send_TCP_Bag(0);

			    	//	Uart_Send_Byte(0xF5);//���ڷ���״̬�������ڵ���

					TCP_Clear=20;//200ms֮�����

					}

				   // Uart_Send_Byte(0xE5);//���ڷ���״̬�������ڵ���
				   
					TCP_Send_Over(pSend,0);


				 
				  return 0x45;
				 }

				 //�յ��Ͽ�����(6)
				 if((DATA[47]&TCP_FIN)==TCP_FIN) 
				   
				   {
					  temp=0;
					  temp+=(U32)(DATA[42]*16777216);
					  temp+=(U32)(DATA[43]*65536);
					  temp+=(U32)(DATA[44]*256);
					  temp+=(U32)(DATA[45]);
					  TCP_Mark=temp;//��� 
	
					  temp=0;
					  temp+=(U32)(DATA[38]*16777216);
					  temp+=(U32)(DATA[39]*65536);
					  temp+=(U32)(DATA[40]*256);
					  temp+=(U32)(DATA[41]);
					  TCP_CMark=temp+1;//ȷ�����
					  
					  if(TCP_Connected==0x01)//���������Ͽ���
					  {
					  TCP_Connected=0x00;//�Ͽ�
					  //���ͶϿ����ֵİ�
	 				  TCP_Flag=TCP_ACK;
					  Send_TCP_Bag(0);
					  //Uart_Send_Byte(0xF6);
					  }
					  else//�Է��Ͽ�
					  {
					   TCP_Connected=0x01;//�Ͽ�
					   TCP_Flag=TCP_ACK;  
					   Send_TCP_Bag(0);

					   TCP_Flag=(TCP_ACK|TCP_FIN);
					   Send_TCP_Bag(0);
					   //Uart_Send_Byte(0xE6);
					  }
	
					 
				   	 return 0x46;
				   }
				  
				  //��������(0)
				if((DATA[47]&TCP_RST)==TCP_RST)
				{
				  temp=0;
				  temp+=(U32)(DATA[42]*16777216);
				  temp+=(U32)(DATA[43]*65536);
				  temp+=(U32)(DATA[44]*256);
				  temp+=(U32)(DATA[45]);
				  TCP_Mark=temp;//��� 

				  TCP_CMark=0;//ȷ�����
				  TCP_Connected=0x00;
				  TCP_Sended=0x00;
				 return 0x40;
				}  


			   }//TCP����
 
			 //5 UDP 
			if(0x11==DATA[23])
			{
			   //UDP����ͣ�UDP_Port�ݲ�����
			    UDP_Re_Port=DATA[34]*256+DATA[35];//Զ��IP
				UDP_Port=DATA[36]*256+DATA[37];//����IP
			  	  App_UDP_Bag();//����������������
			 	  return 0x05;

			}//UDP����


			 return 0x04;//���� TCP UDP ICMP_Ping
		  }
	return 0x06;//���Ǹ��Լ��İ�
}


/*******************************************/
/* ����:�̶�������:��ҳ���ݷ�����������*/
/* ���룺0 �˴�����Ҫ������N<255����       */
/* �����                                  */
/*******************************************/

void TCP_Send_Over(char* p,U16 len)
{	 

	 if(len==0)//����ʣ������
	 {
	      if(TCP_Buf==0)
		  return ;//ֱ���˳�
	      
		  //Uart_Send_Byte(0xEE);
		   
	      TCP_Flag=(TCP_ACK|TCP_PSH);

		   //������Ȳ��㣬ֱ�ӷ�
		   if(TCP_Buf<=254)
		   {
		    
			memcpy(DATA,p+TCP_Haved,TCP_Buf);//�������ݵ���������
		   	Send_TCP_Bag(TCP_Buf);

			 
			 TCP_Haved+=TCP_Buf;
			 TCP_Buf=0;
			 TCP_Sended=1;
		   }
		   else	//���ȴ���
		   {
		    memcpy(DATA,p+TCP_Haved,254);//�������ݵ���������
			Send_TCP_Bag(254);

			TCP_Haved+=254;
			TCP_Buf-=254;
			TCP_Sended=1;
			}
	 }
	else
	 { 
	   TCP_Buf=len;
	   TCP_Haved=0;

	  // Uart_Send_Byte(0xEE);

       TCP_Flag=(TCP_ACK|TCP_PSH);
		 

		   //������Ȳ��㣬ֱ�ӷ�
		   if(len<=254)
		   {

		    memcpy(DATA,p,len);//�������ݵ���������
		   	Send_TCP_Bag(len);

			 TCP_Haved=len;
			 TCP_Buf=0;			 
			 TCP_Sended=1;
		    }
		   else	//���ȴ���
		   {
		    memcpy(DATA,p,254);//�������ݵ���������
			Send_TCP_Bag(254);	
			TCP_Haved+=254;
			TCP_Buf=len-254;
			TCP_Sended=1;
		   }

	 }

}
