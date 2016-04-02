#include "enc28j60.h"
#include "spi.h"
#include "delay.h"
#include "mcuinit.h"
#include "fu_arp.h"
#include "fu_enthernet.h"
#include "fu_ip.h"
#include "fu_config.h"
#include "fu_udp.h"
#include "fu_tcp.h"
#include "fu_icmp.h"
#include "fu_http.h"

/********************************************************************/
/* 本程序仅供学习参考，未经作者允许，不得用于任何商业用途  			*/
/* FuIP专为单片机而定制的TCP/IP协议栈，适用于任何8,16,32位单片机	*/
/* 主程序，可用于客户端，服务端    					            	*/
/* 技术论坛：www.fuhome.net/bbs/						        	*/
/* 移植请注意，此程序使用硬件SPI，模拟SPI会有区别，本程序默认是模拟 */
/* FuIP可用于，客户端，服务端，亦可接入fuhome.net 平台进行远程控制  */ 
/* 版本：v1.2 2014年4月5日						       			 	*/
/* 龙剑奋斗										       			 	*/
/* 定期更新，敬请关注——开放，交流，深入，进步	     			 	*/
/* Copyright fuhome.net 未来之家 实验室，让科技融入生活				*/
/********************************************************************/

#define MIN(a,b) (a) < (b) ? (a) : (b)
idata U8 Enc28j60Bank;
idata U16 NextPacketPtr;

/***************************** ENC28J60相关程序 *****************************/

/*******************************************/
/* 功能：ENC28J60读操作                    */
/* 输入：无                                */
/* 输出：无							       */
/*******************************************/
U8 enc28j60ReadOp(U8 op, U8 address)	  //读控制寄存器
{
	U8 dat1;
	//片选	
	CSN =0;
	// issue read command
	WriteByte(op | (address & ADDR_MASK));	
	dat1 = ReadByte();
	// do dummy read if needed (for mac and mii, see datasheet page 29)
	if(address & 0x80) 	dat1 = ReadByte();
	//释放片选
	CSN=1;
	return(dat1);
}
/*******************************************/
/* 功能：ENC28J60写操作                    */
/* 输入：无                                */
/* 输出：无							       */
/*******************************************/
void enc28j60WriteOp(U8 op, U8 address, U8 mydat)	 //写控制寄存器
{
	CSN=0;
	// issue write command
	WriteByte( op | (address & ADDR_MASK));	//写命令
	// write data
	WriteByte(mydat);
	CSN=1;
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
void enc28j60SetBank(U8 address)
{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
U8 enc28j60Read(U8 address)
{
	// select bank to read
	enc28j60SetBank(address);	
	// do the read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
void enc28j60Write(U8 address,U8 mydat)
{
	// select bank to write
	enc28j60SetBank(address);

	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, mydat);
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
U16 enc28j60Phyread(U8 address)
{
	U16 mydat;
	
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	enc28j60Write(MICMD, MICMD_MIIRD);
	
	// Loop to wait until the PHY register has been read through the MII
	// This requires 10.24us
	while( (enc28j60Read(MISTAT) & MISTAT_BUSY) );
	
	// Stop reading
	enc28j60Write(MICMD, MICMD_MIIRD);
	
	// Obtain results and return
	mydat = enc28j60Read ( MIRDL );
	mydat |= enc28j60Read ( MIRDH );

	return mydat;
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
void enc28j60PhyWrite(U8 address, U16 mydat)
{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	// write the PHY data
	enc28j60Write(MIWRL, mydat & 0x00ff);
	enc28j60Write(MIWRH, mydat >> 8);
	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY)
	{
		Delay_10us();
		Delay_10us();
	}
}

/*******************************************/
/* 功能：ENC28J60读缓冲区程序              */
/* 输入：无                                */
/* 输出：无							       */
/*******************************************/
void enc28j60ReadBuffer(U16 len, U8* dat)
{
// assert CS
//	ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_CONTROL_CS);
    CSN = 0;
    // issue read command
	//SPDR = ENC28J60_READ_BUF_MEM;
	WriteByte(ENC28J60_READ_BUF_MEM);
    //while(!(SPSR & (1<<SPIF)));
    while(len--)
    {
        // read data
		//SPDR = 0x00;
        //while(!(SPSR & (1<<SPIF)));
        //*dat++ = SPDR;
		*dat++ = ReadByte();
    }	
    // release CS
	//ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
	CSN = 1;
}

/*******************************************/
/* 功能：ENC28J60写缓冲区程序              */
/* 输入：无                                */
/* 输出：无							       */
/*******************************************/

void enc28j60WriteBuffer(U16 len, U8* dat)
{
    // assert CS
	//ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_CONTROL_CS);
	CSN = 0;//使能
    
    // issue write command
	//SPDR = ENC28J60_WRITE_BUF_MEM;
	WriteByte(ENC28J60_WRITE_BUF_MEM);
   // while(!(SPSR & (1<<SPIF)));
    while(len--)
    {
        // write data
		//SPDR = *dat++;
        //while(!(SPSR & (1<<SPIF)));
		WriteByte(*dat++);
    }	
    // release CS
//	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
	CSN = 1;
}



/*******************************************/
/* 功能：ENC28J60发送ARP数据包程序         */
/* 输入：无								   */
/* 输出：无							       */
/*******************************************/
void ARP_Packet_Send(void)
{
	// 设置写指针开始的传输缓冲区域
	enc28j60Write(EWRPTL, TXSTART_INIT);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // 设置TXND指向对应于给定的数据包大小
	enc28j60Write(ETXNDL, (TXSTART_INIT+42));//14为以太网头+28 ARP数据包长度
    enc28j60Write(ETXNDH, (TXSTART_INIT+42)>>8);

    // 写每个包的控制字
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // TODO, fix this up

    
        // 以太网头 到传输缓冲
		enc28j60WriteBuffer(14, EN_Head);
		// ARP数据包 到传输缓冲
		enc28j60WriteBuffer(28, ARP_Data);
    

	// 将以太网控制寄存器ECON1所有位 置1，以发送缓冲区数据
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}



/*******************************************/
/* 功能：ENC28J60发送UDP数据包程序         */
/* 输入：UDP数据长度len	     			   */
/* 输出：无							       */
/*******************************************/
void UDP_Packet_Send(U16 len)
{
	// 设置写指针开始的传输缓冲区域
	enc28j60Write(EWRPTL, TXSTART_INIT);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // 设置TXND指向对应于给定的数据包大小
	enc28j60Write(ETXNDL, (TXSTART_INIT+42+len));//14为以太网头+20IP头+8UDP头+len数据包长度
    enc28j60Write(ETXNDH, (TXSTART_INIT+42+len)>>8);

    // 写每个包的控制字
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // TODO, fix this up

    
        // 以太网头 到传输缓冲
		enc28j60WriteBuffer(14, EN_Head);
		// IP头 到传输缓冲
		enc28j60WriteBuffer(20, IP_Head);
		// UDP头 到传输缓冲
		enc28j60WriteBuffer(8, UDP_Head);
		// UDP数据 到传输缓冲
		enc28j60WriteBuffer(len, DATA);
    

	// 将以太网控制寄存器ECON1所有位 置1，以发送缓冲区数据
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

/*******************************************/
/* 功能：ENC28J60发送TCP数据包程序         */
/* 输入：TCP数据长度len      			   */
/* 输出：无							       */
/*******************************************/
void TCP_Packet_Send(U16 len)
{
	// 设置写指针开始的传输缓冲区域
	enc28j60Write(EWRPTL, TXSTART_INIT);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // 设置TXND指向对应于给定的数据包大小
	enc28j60Write(ETXNDL, (TXSTART_INIT+54+len));//14为以太网头+20IP头+20TCP头+len数据包长度
    enc28j60Write(ETXNDH, (TXSTART_INIT+54+len)>>8);

    // 写每个包的控制字
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // TODO, fix this up

    
        // 以太网头 到传输缓冲
		enc28j60WriteBuffer(14, EN_Head);
		// IP头 到传输缓冲
		enc28j60WriteBuffer(20, IP_Head);
		// UDP头 到传输缓冲
		enc28j60WriteBuffer(20, TCP_Head);
		// UDP数据 到传输缓冲

		enc28j60WriteBuffer(len,DATA);

    

	// 将以太网控制寄存器ECON1所有位 置1，以发送缓冲区数据
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

/*******************************************/
/* 功能：ENC28J60发送ICMP_Ping数据包程序   */
/* 输入：无			   					   */
/* 输出：无							       */
/*******************************************/
void ICMP_Ping_Packet_Send()//发送ICMP_Ping包
{
	// 设置写指针开始的传输缓冲区域
	enc28j60Write(EWRPTL, TXSTART_INIT);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // 设置TXND指向对应于给定的数据包大小
	enc28j60Write(ETXNDL, (TXSTART_INIT+74));//14为以太网头+20IP头+40ICMP_Ping数据包长度
    enc28j60Write(ETXNDH, (TXSTART_INIT+74)>>8);

    // 写每个包的控制字
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // TODO, fix this up

    
        // 以太网头 到传输缓冲
		enc28j60WriteBuffer(14, EN_Head);
		// IP头 到传输缓冲
		enc28j60WriteBuffer(20, IP_Head);
		// ICMP 到传输缓冲
		enc28j60WriteBuffer(40, ICMP);


	// 将以太网控制寄存器ECON1所有位 置1，以发送缓冲区数据
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}



//ETH包最小长度60个
#define ETHERNET_MIN_PACKET_LENGTH	0x3C
//ETH头长度	14
#define ETHERNET_HEADER_LENGTH		0x0E
//IP+TCP头长度 20+ TCP=20,UDP=8
#define IP_TCP_HEADER_LENGTH 40
#define IP_UDP_HEADER_LENGTH 28
//IP+TCP+ETH 头长度
#define TOTAL_TCP_HEADER_LENGTH (IP_TCP_HEADER_LENGTH+ETHERNET_HEADER_LENGTH)
#define TOTAL_UDP_HEADER_LENGTH (IP_UDP_HEADER_LENGTH+ETHERNET_HEADER_LENGTH)

/*******************************************/
/* 功能：ENC28J60发送数据包程序            */
/* 输入：leixing 0 TCP 1 UDP,len,数据包地址*/
/* 输出：无							       */
/*******************************************/
void enc28j60PacketSend(U8 leixing,U16 len,U8* packet)
{


    
	// 设置写指针开始的传输缓冲区域
	enc28j60Write(EWRPTL, TXSTART_INIT);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // 设置TXND指向对应于给定的数据包大小
	enc28j60Write(ETXNDL, (TXSTART_INIT+len));
    enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);

    // 写每个包的控制字
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // TODO, fix this up

    if( FU_LEN <= TOTAL_UDP_HEADER_LENGTH-42 )//如果长度小于这长度,不明觉厉
    {
        // 复制包到传输缓冲
		enc28j60WriteBuffer(len, packet);
    }
    else
    {
          
		// 写入头
		if(leixing==0)
        {
		enc28j60WriteBuffer(TOTAL_TCP_HEADER_LENGTH, packet);
		len -= TOTAL_TCP_HEADER_LENGTH;
		}
		if(leixing==1)
		{
		enc28j60WriteBuffer(TOTAL_UDP_HEADER_LENGTH, packet);
		len -= TOTAL_UDP_HEADER_LENGTH;
		}
		// 写入数据
        enc28j60WriteBuffer(len, (U8 *)DATA);
    }

	// 将以太网控制寄存器ECON1所有位 置1，以发送缓冲区数据
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}



/*******************************************/
/* 功能：ENC28J60接收数据包程序            */
/* 输入：最大maxlen,存储数据包地址         */
/* 输出：数据长度						   */
/*******************************************/
U16 enc28j60PacketReceive(U16 maxlen, U8* packet)	 //接收数据包函数，最大长度 数据
{
    U16 rxstat;
    U16 len;


	// 检查缓冲是否一个包已经收到
	if( !(enc28j60Read(EIR) & EIR_PKTIF) )
    {
        // 通过查看EPKTCNT寄存器再次检查是否收到包
		if (enc28j60Read(EPKTCNT) == 0)
            return 0;
    }
    
    // Make absolutely certain that any previous packet was discarded	
	//if( WasDiscarded == FALSE)
	//	MACDiscardRx();

    
	//设置接收到的数据包读指针开始
	enc28j60Write(ERDPTL, (NextPacketPtr));
    enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

    // 下一个封包读指针
	NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // 读取包的长度
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // 读取接收数据包的状态
	rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // 计算实际数据长度
	// 移除CRC字段的长度来减少MAC所报告长度
	len = MIN(len, maxlen);

    // 从缓冲复制数据包到packet中
	enc28j60ReadBuffer(len, packet);

    // ERXRDPT读缓冲器指针
	// ENC28J60将一直写到该指针之前的一单元为止
/*
    enc28j60Write(ERXRDPTL, (NextPacketPtr));
    enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);
*/
    // Errata workaround #13. Make sure ERXRDPT is odd
	//
	{
        U16 rs,re;
        rs = enc28j60Read(ERXSTH);
        rs <<= 8;
        rs |= enc28j60Read(ERXSTL);
        re = enc28j60Read(ERXNDH);
        re <<= 8;
        re |= enc28j60Read(ERXNDL);
        if (NextPacketPtr - 1 < rs || NextPacketPtr - 1 > re)
        {
            enc28j60Write(ERXRDPTL, (re));
            enc28j60Write(ERXRDPTH, (re)>>8);
        }
        else
        {
            enc28j60Write(ERXRDPTL, (NextPacketPtr-1));
            enc28j60Write(ERXRDPTH, (NextPacketPtr-1)>>8);
        }
    }

    // 数据包个数递减位EPKTCNT减1
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

    return len;
}



/*******************************************/
/* 功能：ENC28J60芯片初始化程序            */
/* 输入：无                                */
/* 输出：无							       */
/*******************************************/
void enc28j60_init(void)
{


	//*****Bank1区相关寄存器配置            SPI操作块         数据块
	//初始化程序一开始先进行软件复位，111<操作码>+11111<参数>， N/A
	// ENC28J60_SOFT_RESET=0xFF
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	Delay_5ms(1);


    // do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first

    // 初始化接收缓冲区，设置接收起始地址
	NextPacketPtr = RXSTART_INIT;	 // 读下一数据包指针
    enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXSTH, RXSTART_INIT>>8);

    // 设置接收读指针指向地址
	enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);

 	// 设置接收缓冲区的末尾地址
	// ERXND寄存器默认指向整个缓冲区的最后一个单元
	enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60Write(ERXNDH, RXSTOP_INIT>>8);

	// 设置发送缓冲区的起始地址
	// ETXST寄存器默认地址是整个缓冲区的第一个单元
	enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60Write(ETXSTH, TXSTART_INIT>>8);

 	// Bank2区相关寄存器配置
 	// MAC初始化配置
	// MAC接收使能，下行程序段表示使能MAC接收，使能IEEE流量控制
	enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    // MACON2清零，让MAC退出复位状态
	enc28j60Write(MACON2, 0x00);
    // 下行程序段表示使能自动填充和自动CRC添加
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
//	enc28j60Write(MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	// 配置非背对背包之间的间隔
	enc28j60Write(MAIPGL, 0x12);
    enc28j60Write(MAIPGH, 0x0C);
    // 配置背对背包之间的间隔
	enc28j60Write(MABBIPG, 0x12);
    // 设置允许接收或发送的最大帧长度编程
	enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
    enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);

    // Bank3区相关寄存器配置
	// 将MAC地址写入MAADR0-MAADR5寄存器中
	// NOTE: MAC address in ENC28J60 is byte-backward
	enc28j60Write(MAADR5, FU_MAC[0]);
	enc28j60Write(MAADR4, FU_MAC[1]);
	enc28j60Write(MAADR3, FU_MAC[2]);
	enc28j60Write(MAADR2, FU_MAC[3]);
	enc28j60Write(MAADR1, FU_MAC[4]);
	enc28j60Write(MAADR0, FU_MAC[5]);

    // 阻止发送回路的自动环回
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

    // Bank0区相关寄存器配置
	enc28j60SetBank(ECON1);

    
	//INT有数据包中断使能

	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);

	//WOL中断使能
	
	//enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EWOLIR, 0);//WOL各条件中断清0（ 单播，广播 ）
	//enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIR, 0);//WOL总中断清0
    //enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EWOLIE, 0x81);//WOL中断各条件使能
    //enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_WOLIE);//WOL总中断使能

	// 包接收允许使能
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

}


