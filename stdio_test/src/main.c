
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/io/fcntl.h>
#include <stdio.h>


extern FILE _Stderr;
extern FILE _Stdin;
extern FILE _Stdout;


int module_start(SceSize args, void *argp){

	sceClibPrintf("_Stderr : %p\n", &_Stderr);
	sceClibPrintf("_Stdin  : %p\n", &_Stdin);
	sceClibPrintf("_Stdout : %p\n", &_Stdout);

	sceClibPrintf("_Stderr: %p\n", _Stderr._blksize);

	fprintf(&_Stderr, "libc stderr test\n");
	fprintf(&_Stdout, "libc stdout test\n");

	sceIoWrite(sceKernelGetStderr(), "iofilemgr stderr test\n", sceClibStrnlen("iofilemgr stderr test\n", ~0));
	sceIoWrite(sceKernelGetStdout(), "iofilemgr stdout test\n", sceClibStrnlen("iofilemgr stdout test\n", ~0));

	sceKernelDelayThread(10000);
	sceKernelExitProcess(0);

	return SCE_KERNEL_START_SUCCESS;
}
