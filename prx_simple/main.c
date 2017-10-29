#include <psp2/kernel/clib.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>

#include "debugScreen.h"
#define printf psvDebugScreenPrintf
#define POS(C,L) "\033[" C ";" L "H"

int simple_greeting(int in){
	return in*2;
}

int module_stop(SceSize argc, const void *args){
	printf(POS("64","2")"I, PRX, have been stopped (%s)\n",(char*)args);
	return SCE_KERNEL_STOP_SUCCESS;
}

int module_exit(){
	printf(POS("64","3")"Blarg\n");
	return SCE_KERNEL_STOP_SUCCESS;
}
void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, void *args){
	// The loader gave us SceDisplay info so we can print
	struct{int mutex;SceDisplayFrameBuf*fbuf;}*in = args;
	psvDebugScreenMutex = in->mutex;
	psvDebugScreenFrameBuf = *in->fbuf;
	printf(POS("64","1")"Hello world from PRX!\n");
	printf(POS("64","2")"start:%p stop:%p exit:%p func:%p\n",module_start, module_stop, module_exit, simple_greeting);
	return SCE_KERNEL_START_SUCCESS;
}

