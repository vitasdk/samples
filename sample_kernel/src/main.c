
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/sysmem.h>
#include <psp2kern/kernel/sysclib.h>
#include <sample_lib/sample_kernel.h>

const SceKernelDebugMessageContext ctx = {
	.hex_value0_hi = 0x12345678,
	.hex_value0_lo = 0x9ABCDEF0,
	.hex_value1    = 0xA5A5A5A5,
	.func = NULL,
	.line = 0,
	.file = NULL
};

SceUInt64 time_s = 0LL;

SceUInt64 sample_kern_get_module_start_time(void){
	return time_s;
}

int sample_kern_get_module_start_time_for_user(SceUInt64 *time){

	int res, state;

	ENTER_SYSCALL(state);

	res = ksceKernelMemcpyKernelToUser((uintptr_t)time, &time_s, sizeof(time));

	EXIT_SYSCALL(state);

	return res;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp){

	const void *lr;
	char buff1[0x10];
	char buff2[0x10];

	asm volatile("mov %0, lr\n" :"=r"(lr));

	buff1[sizeof(buff1) - 1] = 0;
	strncpy(buff1, "Hello", sizeof(buff1) - 1);

	if(memcpy(buff2, buff1, sizeof(buff2)) != buff2)
		ksceDebugPrintKernelPanic(&ctx, lr);

	if(memcmp(buff1, buff2, sizeof(buff1)) != 0)
		ksceDebugPrintKernelPanic(&ctx, lr);

	if(strncmp(buff1, buff2, strlen(buff1)) != 0)
		ksceDebugPrintKernelPanic(&ctx, lr);

	time_s = ksceKernelGetSystemTimeWide();

	ksceDebugPrintf("All ok\n");

	return SCE_KERNEL_START_SUCCESS;
}
