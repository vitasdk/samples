/* 
	ICMP with Raw Sockets
	Use this sample to ping Google DNS (8.8.8.8) from your PSVita
	
	Enjoy,
		- pyroesp
*/

#include <psp2/sysmodule.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/net/net.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

#define NET_PARAM_MEM_SIZE (1*1024*1024)

/* IP */
#define IP_GOOGLE_DNS "8.8.8.8"

/* Arbitrary payload size for ICMP */
#define ICMP_MIN_PAYLOAD (32) 


/* TCP Flag bits struct */
typedef struct{
	struct{
		unsigned short : 7; /* data offset + reserved, not used in this struct */
		unsigned short ns : 1;
		unsigned short cwr : 1;
		unsigned short ece : 1;
		unsigned short urg : 1;
		unsigned short ack : 1;
		unsigned short psh : 1;
		unsigned short rst : 1;
		unsigned short syn : 1;
		unsigned short fin : 1;
	};
}TcpFlagBits;

/* From wikipedia TCP Header table */
/* TCP header struct */
typedef struct{
	uint16_t src;
	uint16_t dst;
	uint32_t sequence;
	uint32_t ack_num;
	union{
		uint16_t data_flags;
		struct{
			uint16_t data_offset : 4;
			uint16_t reserved : 3;
			uint16_t flags : 9;
		};
		TcpFlagBits bit;
	};
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_ptr;
}TcpHdr;

/* ICMP Packet structure (= icmp header + payload) */
typedef struct{
	SceNetIcmpHeader hdr;
	char payload[ICMP_MIN_PAYLOAD];
}IcmpPacket;

/* TCP Packet structure (= tcp header + icmp packet) */
typedef struct{
	TcpHdr hdr;
	IcmpPacket icmp;
}TcpPacket;

/* 
	Union needed to correctly access the network packets
	- IcmpPacket for the structure of the packet 
	- uin16_t[] for the checksum
	- char[] for the sendto and recvfrom functions
*/
typedef union{
	IcmpPacket icmp_struct;
	uint16_t icmp_u16buff[sizeof(IcmpPacket)/sizeof(uint16_t)];
	char icmp_packet[sizeof(IcmpPacket)];
}IcmpUnion;

typedef union{
	TcpPacket tcp_struct;
	uint16_t tcp_u16buff[sizeof(TcpPacket)/sizeof(uint16_t)];
	char tcp_packet[sizeof(TcpPacket)];
}TcpUnion;


/* ICMP Checksum */
uint16_t in_cksum(uint16_t *ptr, int32_t nbytes); /* Algorithm from RFC1071 */
/* Display data on screen */
void displayRecvPacket(char *recv_packet, uint32_t received_data, TcpHdr *tcphdr, SceNetIcmpHeader *recv_icmphdr, char *recv_payload);
void displaySentPacket(char* packet, uint32_t packet_size, SceNetIcmpHeader *icmphdr, char *payload);


int main (int argc, char *argv[]){
	int32_t retval; /* return value */
	int32_t sfd; /* Socket file descriptor */
	int32_t on; /* used in Setsockopt function */
	int32_t sent_data; /* return value for sendto function */
	int32_t received_data; /* return value for recvfrom function */
	SceNetInAddr dst_addr; /* destination address */
	SceNetSockaddrIn serv_addr; /* server address to send data to */
	SceNetInitParam net_init_param; /* Net init param structure */

	IcmpUnion icmp; /* icmp union */
	TcpUnion tcp; /* tcp union */

	psvDebugScreenInit(); /* start psvDebugScreen */
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET); /* load NET module */

	/* Initialize Net */
	net_init_param.memory = malloc(NET_PARAM_MEM_SIZE);
	memset(net_init_param.memory, 0, NET_PARAM_MEM_SIZE);
	net_init_param.size = NET_PARAM_MEM_SIZE;
	net_init_param.flags = 0;
	sceNetInit(&net_init_param);

	/* Change IP string to IP uint */
	printf("Converting IP address.\n");
	sceNetInetPton(SCE_NET_AF_INET, IP_GOOGLE_DNS, (void*)&dst_addr);
	
	/* Create raw socket type with icmp net protocol */
	sfd = sceNetSocket("ping_test", SCE_NET_AF_INET, SCE_NET_SOCK_RAW, SCE_NET_IPPROTO_ICMP);
	if (sfd < 0)
		goto exit;
	printf("Raw socket created.\n");
	
	/* Allow socket to send datagrams to broadcast addresses */
	retval = sceNetSetsockopt(sfd, SCE_NET_SOL_SOCKET, SCE_NET_SO_BROADCAST, (const void*)&on, sizeof(on)); 
	if (retval == -1)
		goto exit;
	printf("Allow socket to broadcast.\n");
	
	icmp.icmp_struct.hdr.type = SCE_NET_ICMP_TYPE_ECHO_REQUEST; /* set icmp type to echo request */
	icmp.icmp_struct.hdr.code = SCE_NET_ICMP_CODE_DEST_UNREACH_NET_UNREACH;
	icmp.icmp_struct.hdr.un.echo.id = 0x1; /* arbitrary id */
	icmp.icmp_struct.hdr.un.echo.sequence = 0x1234; /* arbitrary sequence */
	
	/* fill payload with random text, this will get sent back */
	strncpy(icmp.icmp_struct.payload, "Random Payload in ping", ICMP_MIN_PAYLOAD); /* NB: strncpy fills the remainder of the buffer with zeroes */
	icmp.icmp_struct.hdr.checksum = in_cksum(icmp.icmp_u16buff, sizeof(IcmpPacket)); /* compute checksum */
	
	serv_addr.sin_family = SCE_NET_AF_INET; /* set packet to IPv4 */
	serv_addr.sin_addr = dst_addr; /* set destination address */
	memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero)); /* fill sin_zero with zeroes */

	/* Send data */
	sent_data = sceNetSendto(sfd, icmp.icmp_packet, sizeof(IcmpPacket), 0, (SceNetSockaddr*)&serv_addr, sizeof(SceNetSockaddr));
	if (sent_data < 1)
		goto exit; /* send failed */
	
	printf("Data sent:\n");
	printf("----------\n\n");
	displaySentPacket(icmp.icmp_packet, sizeof(IcmpPacket), &icmp.icmp_struct.hdr, icmp.icmp_struct.payload); /* Display colored data */
	
	/* Receive data */
	SceNetSockaddr from_addr;
	uint32_t from_len = sizeof(from_addr);

	printf("\n\nReceiving data.\n");
	received_data = sceNetRecvfrom(sfd, tcp.tcp_packet, sizeof(TcpPacket), SCE_NET_MSG_WAITALL, &from_addr, (unsigned int*)&from_len);
	if (received_data < 1)
		goto exit;
	
	printf("Data received:\n");
	printf("--------------\n\n");
	displayRecvPacket(tcp.tcp_packet, sizeof(TcpPacket), &tcp.tcp_struct.hdr, &tcp.tcp_struct.icmp.hdr, tcp.tcp_struct.icmp.payload); /* display colored data */
	
	/* Select to exit */
	printf("\n\n\n\n\n\n                Press select to exit.\n");
	SceCtrlData ctrl;
	while(1){
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		if (ctrl.buttons & SCE_CTRL_SELECT)
			break;

		sceKernelDelayThread(100*1000);
	}
	
	sceNetSocketClose(sfd); /* Close socket */
exit:
	sceNetTerm(); /* Close net */
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET); /* Unload net module */
	sceKernelExitProcess(0); /* Exit */
	return 0;
}


uint16_t in_cksum(uint16_t *ptr, int32_t nbytes){
	uint32_t sum;
	sum = 0;
	while (nbytes > 1){
		sum += *(ptr++);
		nbytes -= 2;
	}

	if (nbytes > 0)
		sum += *(char*)ptr;

	while (sum>>16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (~sum);
}

void displayRecvPacket(char *recv_packet, uint32_t received_data, TcpHdr *tcphdr, SceNetIcmpHeader *recv_icmphdr, char *recv_payload){
	uint32_t i;
	printf("Total Reply packet size = %d\n\n", received_data);
	
	printf("TCP Header:\n");
	printf("-----------\n");
	psvDebugScreenSetBgColor(0xFF808080);
	printf("TCP src = 0x%04X\n", tcphdr->src);
	psvDebugScreenSetBgColor(0xFF0000FF);
	psvDebugScreenSetFgColor(COLOR_BLACK);
	printf("TCP dst = 0x%04X\n", tcphdr->dst);
	psvDebugScreenSetBgColor(0xFFC8FAFF);
	printf("TCP sequence = 0x%08X\n", tcphdr->sequence);
	psvDebugScreenSetFgColor(COLOR_WHITE);
	psvDebugScreenSetBgColor(0xFF286EAA);
	printf("TCP acknowledge number = 0x%08X\n", tcphdr->ack_num);
	psvDebugScreenSetBgColor(0xFFF0F046);
	psvDebugScreenSetFgColor(COLOR_BLACK);
	printf("TCP data flags = 0x%04X\n", tcphdr->data_flags);
	psvDebugScreenSetBgColor(0x3CF5D2);
	printf("TCP window size = 0x%04X\n", tcphdr->window_size);
	psvDebugScreenSetFgColor(COLOR_WHITE);
	psvDebugScreenSetBgColor(0xFFE632F0);
	printf("TCP checksum = 0x%04X\n", tcphdr->checksum);
	psvDebugScreenSetBgColor(0xFF3182F5);
	printf("TCP urgent pointer = 0x%04X\n\n", tcphdr->urgent_ptr);
	psvDebugScreenSetBgColor(0xFF000000);
	printf("ICMP header:\n");
	printf("------------\n");
	psvDebugScreenSetBgColor(0xFF7F7F00);
	printf("ICMP echo reply = 0x%02X\n", recv_icmphdr->type);
	psvDebugScreenSetBgColor(0xFF7F007F);
	printf("ICMP echo code = 0x%02X\n", recv_icmphdr->code);
	psvDebugScreenSetBgColor(0xFF7F0000);
	printf("ICMP id = 0x%04X\n", recv_icmphdr->un.echo.id);
	psvDebugScreenSetBgColor(0xFF007F00);
	printf("ICMP sequence = 0x%04X\n", recv_icmphdr->un.echo.sequence);
	psvDebugScreenSetBgColor(0xFF007F7F);
	printf("ICMP checksum = 0x%04X, is %s\n\n", recv_icmphdr->checksum, (in_cksum((uint16_t*)(recv_packet+20), received_data-20))?"invalid":"valid");
	psvDebugScreenSetBgColor(0xFF00007F);
	printf("Payload : %s\n\n", recv_payload);
	psvDebugScreenSetBgColor(0xFF000000);
	
	printf("Raw TCP dump:\n");
	printf("-------------\n");
	for (i = 0; i < received_data; i++){
		if (i){
			psvDebugScreenSetBgColor(0xFF000000);
			if (i % 16 == 0)
				printf("\n");
			else if (i % 8 == 0)
				printf("    ");
		}
	
		psvDebugScreenSetFgColor(COLOR_WHITE);
		if (recv_packet+i >= recv_payload)
			psvDebugScreenSetBgColor(0xFF00007F);
		else if (recv_packet+i >= (char*)&recv_icmphdr->un.echo.sequence)
			psvDebugScreenSetBgColor(0xFF007F00);
		else if (recv_packet+i >= (char*)&recv_icmphdr->un.echo.id)
			psvDebugScreenSetBgColor(0xFF7F0000);
		else if (recv_packet+i >= (char*)&recv_icmphdr->checksum)
			psvDebugScreenSetBgColor(0xFF007F7F);
		else if (recv_packet+i >= (char*)&recv_icmphdr->code)
			psvDebugScreenSetBgColor(0xFF7F007F);
		else if (recv_packet+i >= (char*)&recv_icmphdr->type)
			psvDebugScreenSetBgColor(0xFF7F7F00);
		else if (recv_packet+i >= (char*)&tcphdr->urgent_ptr)
				psvDebugScreenSetBgColor(0xFF3182F5);
		else if (recv_packet+i >= (char*)&tcphdr->checksum)
			psvDebugScreenSetBgColor(0xFFE632F0);
		else if (recv_packet+i >= (char*)&tcphdr->window_size){
			psvDebugScreenSetBgColor(0x3CF5D2);
			psvDebugScreenSetFgColor(COLOR_BLACK);
		}
		else if (recv_packet+i >= (char*)&tcphdr->data_flags){
			psvDebugScreenSetBgColor(0xFFF0F046);
			psvDebugScreenSetFgColor(COLOR_BLACK);
		}
		else if (recv_packet+i >= (char*)&tcphdr->ack_num)
			psvDebugScreenSetBgColor(0xFF286EAA);
		else if (recv_packet+i >= (char*)&tcphdr->sequence){
			psvDebugScreenSetBgColor(0xFFC8FAFF);
			psvDebugScreenSetFgColor(COLOR_BLACK);
		}
		else if (recv_packet+i >= (char*)&tcphdr->dst){
			psvDebugScreenSetBgColor(0xFF0000FF);
			psvDebugScreenSetFgColor(COLOR_BLACK);
		}
		else if (recv_packet+i >= (char*)&tcphdr->src)
			psvDebugScreenSetBgColor(0xFF808080);
		else
			psvDebugScreenSetBgColor(0xFF000000);
		
		printf("%02X ", recv_packet[i]);
	}		
	psvDebugScreenSetBgColor(0xFF000000);
}

void displaySentPacket(char* packet, uint32_t packet_size, SceNetIcmpHeader *icmphdr, char *payload){
	uint32_t i;
	printf("Total ICMP request packet size = %d\n\n", packet_size);
	printf("Destination : %s\n", IP_GOOGLE_DNS);
	psvDebugScreenSetBgColor(0xFF7F7F00);
	printf("ICMP echo request = 0x%02X\n", icmphdr->type);
	psvDebugScreenSetBgColor(0xFF7F007F);
	printf("ICMP echo code = 0x%02X\n", icmphdr->code);
	psvDebugScreenSetBgColor(0xFF7F0000);
	printf("ICMP id = 0x%04X\n", icmphdr->un.echo.id);
	psvDebugScreenSetBgColor(0xFF007F00);
	printf("ICMP sequence = 0x%04X\n", icmphdr->un.echo.sequence);
	psvDebugScreenSetBgColor(0xFF007F7F);
	printf("ICMP checksum = 0x%04X\n\n", icmphdr->checksum);
	psvDebugScreenSetBgColor(0xFF00007F);
	printf("Payload : %s\n\n", payload);
	psvDebugScreenSetBgColor(0xFF000000);
	printf("Raw ICMP dump:\n");
	printf("--------------\n\n");
	for (i = 0; i < packet_size; i++){
		if (i){
			psvDebugScreenSetBgColor(0xFF000000);
			if (i % 16 == 0)
				printf("\n");
			else if (i % 8 == 0)
				printf("    ");
		}
		
		if (packet+i >= payload)
			psvDebugScreenSetBgColor(0xFF00007F);
		else if (packet+i >= (char*)&icmphdr->un.echo.sequence)
			psvDebugScreenSetBgColor(0xFF007F00);
		else if (packet+i >= (char*)&icmphdr->un.echo.id)
			psvDebugScreenSetBgColor(0xFF7F0000);
		else if (packet+i >= (char*)&icmphdr->checksum)
			psvDebugScreenSetBgColor(0xFF007F7F);
		else if (packet+i >= (char*)&icmphdr->code)
			psvDebugScreenSetBgColor(0xFF7F007F);
		else if (packet+i >= (char*)&icmphdr->type)
			psvDebugScreenSetBgColor(0xFF7F7F00);
		else
			psvDebugScreenSetBgColor(0xFF000000);
		
		printf("%02X ", packet[i]);
	}
	
	psvDebugScreenSetBgColor(0xFF000000);
}
