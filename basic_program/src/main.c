
#include <psp2/types.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>

// module_start param
const char sceUserMainThreadName[]       = "basic_program";
const int sceUserMainThreadPriority      = 0x60;
const SceSize sceUserMainThreadStackSize = 0x2000;

// process param
const int sceKernelPreloadModuleInhibit  = SCE_KERNEL_PRELOAD_INHIBIT_LIBDBG;

__attribute__((__noreturn__))
void basic_program_panic(void){
	while(1){
		*(int *)(0xAA) = 0x55; // trigger coredump
	}
}

SceKernelSysClock sys_clock_basic_program;

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp){

	sceClibPrintf("Starting basic_program\n");

	int res = sceKernelGetProcessTime(&sys_clock_basic_program);
	if(res < 0){
		sceClibPrintf("sceKernelGetProcessTime failed 0x%X\n", res);
		basic_program_panic();
	}

	return SCE_KERNEL_START_SUCCESS;
}
