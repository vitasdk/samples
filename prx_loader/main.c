#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h> 
#include <psp2/io/fcntl.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>

#include "debugScreen.h"

void simple_greeting(); // plugin function

// plugin parameters
typedef int (*print_func)(const char *format, ...);
typedef struct {
    print_func print;
} plugin_param;

#define printf psvDebugScreenPrintf

#define POS(C,L) "\033[" C ";" L "H"

#define ONPRESS(flag) ((ctrl.buttons & (flag)) && !(ctrl_old.buttons & (flag)))

#define prel31_to_addr(ptr) ({ \
    long offset = (((long)*(ptr)) << 1) >> 1; \
    (unsigned long)(ptr) + offset; \
})

int main(int argc, char *argv[]) {
    psvDebugScreenInit();

    SceUID modid=0;
    int status=0, ret=0;

    // pass printf function pointer to plugin
    plugin_param args;
    args.print = printf;

    char arg_fini[] = "see you!";
    char* suprx_path = "ux0:/data/prx_simple.suprx";
    SceKernelModuleInfo mi;
    SceCtrlData ctrl, ctrl_old={};

    for(;;){
        printf(POS("0","0")"PRXloader (Dpad)\n");
        printf("Up   : Boot Up '%s'\n", suprx_path);
        printf("Down : Shut Down with '%s' argument\n", arg_fini);
        printf("Left : List All Modules\n");
        printf("Right: Call PRX Greeting function\n\n");
        printf("ModuleId  :%08X\n", modid);
        printf("start/stop:%08X\n", status);
        printf("last call :%08X\n", ret);
        ctrl_old = ctrl;
        sceCtrlReadBufferPositive(0, &ctrl, 1);
        if(ONPRESS(SCE_CTRL_UP)){
            modid = sceKernelLoadStartModule(suprx_path, sizeof(args), &args, 0, NULL, &status);
        }
        if(ONPRESS(SCE_CTRL_DOWN)){
            ret = sceKernelStopUnloadModule(modid, sizeof(arg_fini), arg_fini, 0, NULL, &status);
            modid = 0;
        }
        if(ONPRESS(SCE_CTRL_LEFT)){
            SceUID modids[32];//typycally ~13 are returned
            SceSize num = sizeof(modids)/sizeof(*modids);
            ret = sceKernelGetModuleList(0x80|0x1, modids, &num);// 0/1 = user 0x80=system
            psvDebugScreenClear(0);
            printf(POS("10","0")"ModuleList:\n");
            printf("---ID--- ------name-----\n");
            for(int n = 0; n < num; n++){
                ret = sceKernelGetModuleInfo(modids[n],&mi);/* TODO: segments[4] ? */
                printf("%08lX <%X-%lX> [%s%s%s%s] %-15s %-40s %4i %4i %8p\n", mi.modid, sizeof(mi), mi.size,
                       mi.unk28?"i":" ", mi.start_entry?"s":" ", mi.stop_entry?"s":" ", mi.exit_entry?"e":" ",
                       mi.module_name, mi.path,  mi.extab_btm - mi.extab_top, mi.exidx_btm - mi.exidx_top, mi.tlsInitSize);

                //Advanced informations: section/exception unwinding
                //for(SceKernelModuleExtab*i = mi.importTop; i < mi.importBtm; i++)
                //    printf("%p ", i->insn0, i->insn1);
                //printf("\n");
                //for(SceKernelModuleExidx*i = mi.exidxTop; i < mi.exidxBtm; i++)
                //    printf("%08lX ", prel31_to_addr(&i->ptr));
                //printf("\n");
            }
        }
        if(ONPRESS(SCE_CTRL_RIGHT) && modid){
            simple_greeting();
        }
    }
    sceKernelExitProcess(0);
    return 0;
}
