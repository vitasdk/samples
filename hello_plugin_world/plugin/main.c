#ifdef __cplusplus
#include <cxxabi.h>
#endif

#include <psp2/kernel/modulemgr.h>

// We use these functions which are located in the main executable
#include <stdio.h>
#include "debugScreen.h"
#define printf psvDebugScreenPrintf
#define puts psvDebugScreenPuts
#define POS(C,L) "\033[" C ";" L "H"

#include "plugin_interface.h"

static int PLUGIN_doTheJob();

static struct PluginPointers functions = {
	PluginPointers_VERSION,

	PLUGIN_doTheJob
};

// hacks to make libc work
#ifdef __cplusplus
extern void *__dso_handle __attribute__((weak));
#endif

/* These magic symbols are provided by the linker.  */
extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start [])(void) __attribute__((weak));
extern void (*__fini_array_end [])(void) __attribute__((weak));

static void __libc_init_array(void) {
	size_t count, i;

	count = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < count; i++) {
		__preinit_array_start[i]();
	}

	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++) {
		__init_array_start[i]();
	}
}

static void __libc_fini_array(void) {
	size_t count, i;

	count = __fini_array_end - __fini_array_start;
	for (i = count; i > 0; i--) {
		__fini_array_start[i-1]();
	}
}

int module_stop(SceSize argc, const void *args) {
#ifdef __cplusplus
	if (&__dso_handle != nullptr) {
		__cxxabiv1::__cxa_finalize(&__dso_handle);
	}
#endif
	__libc_fini_array();
	return SCE_KERNEL_STOP_SUCCESS;
}

int module_exit() {
#ifdef __cplusplus
	if (&__dso_handle != nullptr) {
		__cxxabiv1::__cxa_finalize(&__dso_handle);
	}
#endif
	__libc_fini_array();
	return SCE_KERNEL_STOP_SUCCESS;
}

int _start(SceSize argc, void *args) __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, void *args) {
	struct PluginPointers **arg = *(struct PluginPointers ***)args;

	__libc_init_array();

	*arg = &functions;

	return SCE_KERNEL_START_SUCCESS;
}

int PLUGIN_doTheJob(){
	char buf[256];
	printf(POS("64","0")"Plugin is running with values: %d and %d\n",somePublicValueForPlugin, getSomeMainInt());
	// This function is never used in main executable so we must force it to be linked as nothing is in the plugin
	sprintf(buf, "Sample format: %d", somePublicValueForPlugin);
	puts(buf);
	return 0x80000000 | somePublicValueForPlugin;
}
