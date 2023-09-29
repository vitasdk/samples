
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/clib.h>


// Make the function auto export by adding `__attribute__((__visibility__("default")))`
__attribute__((__visibility__("default")))
int vitasdk_sample_math_xor(int a, int b){
	return a ^ b;
}

__attribute__((__visibility__("default")))
int vitasdk_sample_math_orr(int a, int b){
	return a | b;
}

__attribute__((__visibility__("default")))
int vitasdk_sample_math_and(int a, int b){
	return a & b;
}

__attribute__((__visibility__("default")))
int vitasdk_sample_math_bic(int a, int b){
	return a & ~b;
}

int vitasdk_sample_never_exported(void){
	sceClibPrintf("You can not import this function via export link!\n");
	return 0;
}

// int _start(SceSize args, void *argp) __attribute__ ((weak, alias("module_start")));
int vitasdk_sample_module_start(SceSize args, void *argp){
	sceClibPrintf("module start!\n");
	return SCE_KERNEL_START_SUCCESS;
}

int vitasdk_sample_module_stop(SceSize args, void *argp){
	return SCE_KERNEL_STOP_SUCCESS;
}

int vitasdk_sample_module_exit(SceSize args, void *argp){
	return SCE_KERNEL_START_SUCCESS;
}
