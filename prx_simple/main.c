#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include <string.h>

#define POS(C,L) "\033[" C ";" L "H"

// plugin parameters passed by main executable
typedef int (*print_func)(const char *format, ...);
typedef struct {
    print_func print;
} plugin_param;

static plugin_param params;

void simple_greeting(){
    params.print(POS("61","1")"Sup!\n");
}

int module_stop(SceSize argc, void *args) {
    params.print(POS("60","1")"I, PRX, have been stopped (%s)\n",(char*)args);
    return SCE_KERNEL_STOP_SUCCESS;
}

int module_exit(){
    params.print(POS("61","1")"Blarg\n");
    return SCE_KERNEL_STOP_SUCCESS;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, void *args) {
    sceClibPrintf("Sup!\n");

    params.print = ((plugin_param*)args)->print; // get pointer to print func

    params.print(POS("63","1")"Hello world from PRX!\n");
    params.print(POS("64","1")"start:%p stop:%p exit:%p func:%p\n", module_start, module_stop, module_exit, simple_greeting);

    return SCE_KERNEL_START_SUCCESS;
}
