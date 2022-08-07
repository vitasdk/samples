#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>

#include "debugScreen.h"
#define printf psvDebugScreenPrintf
#define POS(C,L) "\033[" C ";" L "H"
#define ONPRESS(flag) ((ctrl.buttons & (flag)) && !(ctrl_old.buttons & (flag)))

#include "plugin_interface.h"

/**
 * If some plugin need a function we don't use in main binary,
 * reference it here
 **/
#include <stdio.h>
void *forceLinkFunctions[] = {
	(void*)sprintf
};

static int somePrivateValueForPlugin = 0;
int somePublicValueForPlugin = 1;

int getSomeMainInt() {
	return somePrivateValueForPlugin;
}

int main(int argc, char *argv[]) {
	psvDebugScreenInit();

	SceUID modid=0;
	int status=0, ret=0;
	char*suprx_path = "app0:/plugin.suprx";
	SceCtrlData ctrl,ctrl_old={};
	const char *msg = "";

        struct PluginPointers *functions = NULL;

	for(;;){
		printf(POS("0","0")"Plugin sample (Dpad)\n");
		printf("Up   : Boot Up '%s'\n", suprx_path);
		printf("Down : Shut Down\n");
		printf("Left : Modify values used by plugin\n");
		printf("Right: Call plugin job function\n\n");
		printf("ModuleId  : %08X\n", modid);
		printf("Functions : %08X\n", (unsigned int)functions);
		printf("start/stop: %08X\n", status);
		printf("last call : %08X\n", ret);
		printf("last msg  : %s\n", msg);
		ctrl_old = ctrl;
		sceCtrlReadBufferPositive(0, &ctrl, 1);
		if(ONPRESS(SCE_CTRL_UP) && !functions){
			struct PluginPointers **arg = &functions;
			modid = sceKernelLoadStartModule(suprx_path, sizeof(arg), &arg, 0, NULL, &status);
			if (!modid) {
				msg = "No module";
				continue;
			}
			if(functions == NULL){
				msg = "No functions";
				continue;
			}
			if(functions->version != PluginPointers_VERSION){
				msg = "Interface mismatch";
				continue;
			}
			msg = "Plugin loaded";
		}
		if(ONPRESS(SCE_CTRL_DOWN)){
			functions = NULL;
			ret = sceKernelStopUnloadModule(modid, 0, NULL, 0, NULL, &status);
			modid = 0;
		}
		if(ONPRESS(SCE_CTRL_LEFT)){
			somePrivateValueForPlugin++;
			somePublicValueForPlugin <<= 1;
			if (somePublicValueForPlugin == 0) {
				somePublicValueForPlugin = 1;
			}

		}
		if(ONPRESS(SCE_CTRL_RIGHT) && functions){
			ret = functions->doTheJob();
		}
	}
	sceKernelExitProcess(0);
	return 0;
}
