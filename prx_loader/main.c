#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h> 
#include <psp2/io/fcntl.h>
#include <psp2/ctrl.h>

#include "debugScreen.h"
#define printf psvDebugScreenPrintf
#define POS(C,L) "\033[" C ";" L "H"
#define ONPRESS(flag) ((ctrl.buttons & (flag)) && !(ctrl_old.buttons & (flag)))
#define prel31_to_addr(ptr) ({ \
    long offset = (((long)*(ptr)) << 1) >> 1; \
    (unsigned long)(ptr) + offset; \
})
int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	//store psvDebugScreen context to be sent to the module on start
	struct{int mutex;SceDisplayFrameBuf*fbuf;} arg = {psvDebugScreenMutex, &psvDebugScreenFrameBuf};

	SceUID modid=0;
	int status=0, ret=0;
	//char arg_init[] = "wake\0up!";
	char arg_fini[] = "see you!";
	char*suprx_path = "ux0:plugin.suprx";
	SceKernelModuleInfo mi;
	SceCtrlData ctrl,ctrl_old={};
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
			modid = sceKernelLoadStartModule(suprx_path, sizeof(arg), &arg, 0, NULL, &status);
		}
		if(ONPRESS(SCE_CTRL_DOWN)){
			ret = sceKernelStopUnloadModule(modid, sizeof(arg_fini), arg_fini, 0, NULL, &status);
			modid = 0;
		}
		if(ONPRESS(SCE_CTRL_LEFT)){
			SceUID modids[32];//typycally ~13 are returned
			int num = sizeof(modids)/sizeof(*modids);
			ret = sceKernelGetModuleList(0x80|0x1, modids, &num);// 0/1 = user 0x80=system
			psvDebugScreenClear(0);
			printf(POS("0","10")"ModuleList:\n");
			printf("---ID--- ------name-----\n");
			for(int n = 0; n < num; n++){
				ret = sceKernelGetModuleInfo(modids[n],&mi);/* TODO: segments[4] ? */
				printf("%08lX <%X-%lX> [%s%s%s%s] %-15s %-40s %4i %4i %8p\n", mi.modid, sizeof(mi), mi.size,
				       mi.unk28?"i":" ", mi.module_start?"s":" ", mi.module_stop?"s":" ", mi.module_exit?"e":" ",
				       mi.module_name, mi.path,  mi.extabBtm - mi.extabTop, mi.exidxBtm - mi.exidxTop, mi.tlsInit);
				//Advanced informations: section/exception unwinding
				//for(SceKernelModuleExtab*i = mi.importTop; i < mi.importBtm; i++)
				//	printf("%p ", i->insn0, i->insn1);
				//printf("\n");
				//for(SceKernelModuleExidx*i = mi.exidxTop; i < mi.exidxBtm; i++)
				//	printf("%08lX ", prel31_to_addr(&i->ptr));
				//printf("\n");
			}
		}
		if(ONPRESS(SCE_CTRL_RIGHT) && modid){
			ret = sceKernelGetModuleInfo(modid,&mi);
			if(!ret && mi.module_stop)
				status = mi.module_stop(0,"but not from ModuleMgr");
		}
	}
	sceKernelExitProcess(0);
	return 0;
}
