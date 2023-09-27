
#include <psp2/kernel/clib.h>
#include <malloc.h>


/*
 * By default it is 128MiB, but you can reduce the size if you want to use memory other than newlib malloc/free.
 */

unsigned int _newlib_heap_size_user = SCE_KERNEL_8MiB;


/* main routine */
int main(int argc, char *argv[])
{
	void *ptr;

	// 7-MiB (should be return not NULL)
	ptr = malloc(SCE_KERNEL_4MiB + SCE_KERNEL_2MiB + SCE_KERNEL_1MiB);

	sceClibPrintf("ptr : %p\n", ptr);

	free(ptr);

	// 9-MiB (should be return NULL)
	ptr = malloc(SCE_KERNEL_8MiB + SCE_KERNEL_1MiB);

	sceClibPrintf("ptr : %p\n", ptr);

	free(ptr);

	return 0;
}
