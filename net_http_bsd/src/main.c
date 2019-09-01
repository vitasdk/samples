#ifdef __vita__
#include <psp2/sysmodule.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/net/net.h>
#define printf psvDebugScreenPrintf
#else
#define NO_psvDebugScreenInit
#endif

#include "debugScreen.h"

#include <unistd.h>
#include <fcntl.h>
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
	case 8218:return ',';
	case 8230:return 0x2E;
	case 8592:return '<';
	case 8593:return 0xCE;
	case 8594:return '>';
	case 8595:return 0xCD;
	case 8598:return '\\';
	case 8599:return '/';
	case 8601:return '\\';
	case 8600:return '\\';
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
	case 9600:return 0xC0;
	case 9601:return 0xC0;
	case 9602:return 0xDC;
	case 9603:return 0xDC;
	case 9604:return 0xC2;
	case 9605:return 0xC2;
	case 9606:return 0xDB;
	case 9607:return 0xDB;
	case 9608:return 0xDB;
	}
	return '?';
}

int main (__attribute__((unused)) int argc,  __attribute__((unused)) char *argv[]){
	char* wttr[] = {"wttr.in","/London"};
	char* rate[] = {"rate.sx","/"};
	char**url = NULL;// will point to wttr or rate
	PsvDebugScreenFont *psvDebugScreenFont_current;

	psvDebugScreenInit();
	psvDebugScreenFont_current = psvDebugScreenGetFont();
	psvDebugScreenFont_current->size_w-=1;//narrow character printing
	printf("Press [L]=%s%s [R]=%s%s\n", wttr[0], wttr[1], rate[0], rate[1]);
	#ifdef __vita__
	static char net_mem[1*1024*1024];
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	sceNetInit(&(SceNetInitParam){net_mem, sizeof(net_mem)});
	#endif

	#ifdef __vita__
	for(SceCtrlData ctrl={}; !url; sceCtrlReadBufferPositive(0,&ctrl,1)){
		if(ctrl.buttons == SCE_CTRL_LTRIGGER)
			url=wttr;
		if(ctrl.buttons == SCE_CTRL_RTRIGGER)
			url=rate;
	}
	#else
	while(!url){
		int c = getchar();
		if(c == 'L' || c == 'l')
			url = wttr;
		if(c == 'R' || c == 'r')
			url = rate;
	}
	#endif

	printf("fetching %s%s...\n", url[0], url[1]);
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	connect(fd, (const struct sockaddr *)&((struct sockaddr_in){.sin_family = AF_INET, .sin_port = htons(80), .sin_addr.s_addr = *(long*)(gethostbyname(url[0])->h_addr)}), sizeof(struct sockaddr_in));
	
	char*header[] = {"GET ",url[1]," HTTP/1.1\r\n", "User-Agent: curl/PSVita\r\n", "Host: ",url[0],"\r\n", "\r\n", 0};
	for(int i = 0; header[i]; i++)//send all request header to the server
		write(fd, header[i], strlen(header[i]));

	unsigned pos = 0;
	unsigned char c, line[4096];
	while(read(fd,&c,sizeof(c)) > 0 && pos < sizeof(line)) {
		if (c>>6==3) // if fetched char is a UTF8 
			c = readUTF(c,fd); // convert it back to ASCII
		if (c == '\n') { // end of line reached
			psvDebugScreenPrintf("%.*s\n", pos, line); // printf the received line into the screen
			pos = 0; // reset the buffer pointer back to 0
		} else {
			line[pos] = c;
			pos++;
		}
	}
	close(fd);

	#ifndef __vita__ // generate a RGB screen dump (if built on PC)
	//convert the dump into PNG with: convert -depth 8 -size 960x544+0 RGB:screen.data out.png;
	int fdump = open("screen.data", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	for (unsigned i = 0; i < sizeof(base); i += 4)// for every RGBA bytes pointed by the screen "base" adress
		write(fdump, base+i, 3);//write the RGB part (3 bytes) into "screen.data"
	close(fdump);
	#endif

	#ifdef __vita__
	sceNetTerm();
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
	sceKernelDelayThread(~0);
	#endif
	return 0;
}
