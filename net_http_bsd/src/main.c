#ifdef __vita__
#include <psp2/sysmodule.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/net/net.h>
#include "debugScreen.h"
#define printf psvDebugScreenPrintf
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

unsigned char readUTF(unsigned char c, int fd){
	int uni = 0;char u;
	if(c>=0b11000000){uni = uni?:c & 0b011111;read(fd,&u,sizeof(u));uni = (uni << 6) | (u & 0x3f);}
	if(c>=0b11100000){uni = uni?:c & 0b001111;read(fd,&u,sizeof(u));uni = (uni << 6) | (u & 0x3f);}
	if(c>=0b11110000){uni = uni?:c & 0b000111;read(fd,&u,sizeof(u));uni = (uni << 6) | (u & 0x3f);}
	switch(uni) {
	case  176:return 0xBC;
	case 8213:return 0x17;
	case 8216:return 0x60;
	case 8217:return 0x27;
	case 8230:return 0x2E;
	case 8592:return '<';
	case 8593:return '^';
	case 8594:return '>';
	case 8595:return 'v';
	case 8598:return '\\';
	case 8599:return '/';
	case 8601:return '\\';
	case 8602:return '/';
	case 9472:return 0x17;
	case 9474:return 0x16;
	case 9484:return 0x18;
	case 9488:return 0x19;
	case 9492:return 0x1A;
	case 9496:return 0x1B;
	case 9500:return 0x14;
	case 9508:return 0x13;
	case 9516:return 0x12;
	case 9524:return 0x11;
	case 9532:return 0x15;
	case 9600:return 0xDF;
	case 9604:return 0xDC;
	}
	return '?';
}

int main (int argc, char *argv[]){
	static char net_mem[1*1024*1024];

	#ifdef __vita__
	psvDebugScreenInit();
	psvDebugScreenFont.size_w-=1;//narrow character printing
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	sceNetInit(&(SceNetInitParam){net_mem, sizeof(net_mem)});
	#endif

	char* host = "wttr.in", *path="/Paris";
	int pos=0, fd = socket(PF_INET, SOCK_STREAM, 0);
	connect(fd, (const struct sockaddr *)&((struct sockaddr_in){.sin_family = AF_INET, .sin_port = htons(80), .sin_addr.s_addr = *(long*)(gethostbyname(host)->h_addr)}), sizeof(struct sockaddr_in));
	
	char*header[] = {"GET ",path," HTTP/1.1\r\n", "User-Agent: curl/7.52.1\r\n", "Host: ",host,"\r\n", "\r\n", 0};
	for(int i=0;header[i];i++)write(fd, header[i], strlen(header[i]));

	for(unsigned char c, line[4096];read(fd,&c,sizeof(c))>0 && pos<sizeof(line);pos = (c=='\n')? 0 : (pos + 1)){
		#ifdef __vita__
		if (c>>6==3)c = readUTF(c,fd);
		#endif
		if (c == '\n') printf("%.*s\n",pos,line);
		else line[pos] = c;
	}

	close(fd);
	#ifdef __vita__
	sceNetTerm();
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
	sceKernelDelayThread(~0);
	#endif
	return 0;
}
