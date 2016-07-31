#include <psp2/kernel/processmgr.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	printf("Hello, world!\n");
    sceKernelExitProcess(0);
	return 0;
}
