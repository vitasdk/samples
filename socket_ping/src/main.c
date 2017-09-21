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
	};
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_ptr;
}TcpHdr;

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
	}bit;
}TcpFlag;


/* ICMP Checksum */
uint16_t in_cksum(uint16_t *ptr, int32_t nbytes); /* Algorithm from RFC1071 */
/* Display data on screen */
void displayRecvPacket(char *recv_packet, uint32_t received_data, TcpHdr *tcphdr, SceNetIcmpHeader *recv_icmphdr, char *recv_payload);
void displaySentPacket(char* packet, uint32_t packet_size, SceNetIcmpHeader *icmphdr, char *payload);


int main (int argc, char *argv[]){
	char *packet; /* Packet to send */
	char *payload; /* Payload inside packet */
	int32_t retval; /* return value */
	int32_t sfd; /* Socket file descriptor */
	int32_t on; /* used in Setsockopt function */
	int32_t sent_data; /* return value for sendto function */
	int32_t received_data; /* return value for recvfrom function */
	uint32_t packet_size; /* Packet to send size */
	SceNetInAddr dst_addr; /* destination address */
	SceNetSockaddrIn serv_addr; /* server address to send data to */
	SceNetIcmpHeader *icmphdr; /* ICMP header structure */
	SceNetInitParam net_init_param; /* Net init param structure */

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
	
	/* Malloc packet to send */
	packet_size = sizeof(SceNetIcmpHeader) + ICMP_MIN_PAYLOAD; /* packet size is sizeof ICMP header + payload buffer size */
	packet = (char*)malloc(packet_size);
	memset(packet, 0, packet_size); /* set packet to 0 */
	icmphdr = (SceNetIcmpHeader*)packet; /* get icmp header pointer */
	payload = packet + sizeof(SceNetIcmpHeader); /* get payload pointer */
	printf("Malloc packet and set pointers for ICMP header and payload.\n\n");
	
	icmphdr->type = SCE_NET_ICMP_TYPE_ECHO_REQUEST; /* set icmp type to echo request */
	icmphdr->code = SCE_NET_ICMP_CODE_DEST_UNREACH_NET_UNREACH;
	icmphdr->un.echo.id = 0x0100; /* arbitrary id */
	icmphdr->un.echo.sequence = 0x3412; /* arbitrary sequence */
	
	strncpy(payload, "Random Payload in ping", ICMP_MIN_PAYLOAD); /* fill payload with random text, this will get sent back */
	icmphdr->checksum = in_cksum((uint16_t*)packet, packet_size); /* compute checksum */
	
	serv_addr.sin_family = SCE_NET_AF_INET; /* set packet to IPv4 */
	serv_addr.sin_addr = dst_addr; /* set destination address */
	memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero)); /* fill sin_zero with zeroes */

	/* Send data */
	sent_data = sceNetSendto(sfd, packet, packet_size, 0, (SceNetSockaddr*)&serv_addr, sizeof(SceNetSockaddr));
	if (sent_data < 1)
		goto exit; /* send failed */
	
	printf("Data sent:\n");
	printf("----------\n\n");
	displaySentPacket(packet, packet_size, icmphdr, payload); /* Display colored data */
	
	/* Receive data */
	SceNetSockaddr from_addr;
	uint32_t from_len = sizeof(from_addr);
	uint32_t recv_len = 512; /* arbitrary receive buffer length */
	char *recv_packet = (char*)malloc(recv_len); /* malloc receive packet */
	memset(recv_packet, 0, recv_len); /* set packet to 0 */

	printf("\n\nReceiving data.\n");
	received_data = sceNetRecvfrom(sfd, recv_packet, recv_len, SCE_NET_MSG_WAITALL, &from_addr, (unsigned int*)&from_len);
	if (received_data < 1)
		goto exit;
	
	TcpHdr *tcphdr = (TcpHdr*)recv_packet; /* get tcp header pointer*/
	TcpFlag *tcpflag = (TcpFlag*)&tcphdr->data_flags; /* get tcp flag pointer if you want to be able to access individual bits, not used in this sample */
	SceNetIcmpHeader *recv_icmphdr = (SceNetIcmpHeader*)(recv_packet + sizeof(TcpHdr)); /* get icmp pointer of received packet */
	char *recv_payload = recv_packet + sizeof(TcpHdr) + sizeof(SceNetIcmpHeader); /* get payload pointer */

	printf("Data received:\n");
	printf("--------------\n\n");
	displayRecvPacket(recv_packet, received_data, tcphdr, recv_icmphdr, recv_payload); /* display colored data */
	
	/* Select to exit */
	printf("\n\n\n\n\n\n                Press select to exit.\n");
	SceCtrlData ctrl;
	while(1){
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		if (ctrl.buttons & SCE_CTRL_SELECT)
			break;

		sceKernelDelayThread(100*1000);
	}
	
	/* free data */
	free(packet);
	free(recv_packet);
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
