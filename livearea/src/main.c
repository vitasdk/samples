#include <psp2/kernel/processmgr.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	printf("Hello, pretty world!\n");
	sceKernelExitProcess(0);
	return 0;
}
