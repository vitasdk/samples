#include <psp2/kernel/processmgr.h>
#include <stdio.h>
#include <khash.h>
#include <frozen.c>

#define ASSERT(expr) if (!(expr))return sceKernelExitProcess(0);


KHASH_MAP_INIT_INT(32, char)

int main(int argc, char *argv[]) {
	int ret, is_missing;
	khiter_t k;
	khash_t(32) *h = kh_init(32);
	k = kh_put(32, h, 5, &ret);
	kh_value(h, k) = 10;
	k = kh_get(32, h, 10);
	is_missing = (k == kh_end(h));
	k = kh_get(32, h, 5);
	kh_del(32, h, k);
	for (k = kh_begin(h); k != kh_end(h); ++k)
		if (kh_exist(h, k)) kh_value(h, k) = 1;
	kh_destroy(32, h);
	
	int a = 0, b = 0;
	char *d = NULL;
	const char *str ="{ a: 1234, b : true, d: \"hi%20there\" }";

	ASSERT(json_scanf(str, strlen(str), "{a: %d, b: %B, d: %Q}", &a, &b, &d) == 4);
	ASSERT(a == 1234);
	ASSERT(b == 1);
	ASSERT(d != NULL);
	ASSERT(strcmp(d, "hi%20there") == 0);
	free(d);
	
	sceKernelExitProcess(0);
	return 0;
}
