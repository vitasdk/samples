#include <psp2/kernel/processmgr.h>
#include <stdio.h>

#include "debugScreen.h"

int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	psvDebugScreenPrintf("Hello, world!\nThis sample will close in 5s...");
	sceKernelDelayThread(5*1000*1000);
	sceKernelExitProcess(0);
	return 0;
}
