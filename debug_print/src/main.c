#include <psp2/kernel/processmgr.h>
#include <stdio.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

int main(int argc, char *argv[]) {
	int remain = 5;
	psvDebugScreenInit();

	printf("Welcome to the psvDebugScreen showcase !\n");
	
	/* print some bg/fg colors */
	const char* message = "Let's have some foreground/background colors !\n";
	int modes[]={3,9,4,10}, c;
	for(c = 0; message[c] != '\0'; c++)
		printf("\e[%i%im%c", modes[(c/8)%4], c%8, message[c]);

	/* reset fg/bg color back to default */
	printf("\e[m");

	/* \r demo using a countdown */
	while(remain-->0){
		printf("This sample will close in %i s...\r", remain);
		sceKernelDelayThread(1000*1000);
	}

	/* print at specific col;row */
	printf("\e[10;20HBye Bye");
	sceKernelDelayThread(2*1000*1000);

	sceKernelExitProcess(0);
	return 0;
}
