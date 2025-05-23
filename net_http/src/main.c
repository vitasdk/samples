#include <vitasdk.h>

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

#include "debugScreen.h"

#define COLOR_BLACK 0x000000
#define COLOR_RED   0xFF0000
#define COLOR_BLUE  0x0000FF
#define COLOR_GREEN 0x00FF00
#define COLOR_WHITE 0xFFFFFF

SceCtrlData pad, oldpad;

void download(const char *url, const char *dest) {
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenPrintf("Starting to download:\n\"%s\"\nto\n\"%s\"\n\n", url, dest);

	// Create template with an user agent
	int tpl = sceHttpCreateTemplate("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36", SCE_HTTP_VERSION_1_1, SCE_TRUE);
	psvDebugScreenPrintf("0x%08X sceHttpCreateTemplate\n", tpl);

	// set url on the template
	int conn = sceHttpCreateConnectionWithURL(tpl, url, SCE_TRUE);
	psvDebugScreenPrintf("0x%08X sceHttpCreateConnectionWithURL\n", conn);

	// create the request with the correct method
	int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateRequestWithURL\n", request);

	// send the actual request. Second parameter would be POST data, third would be length of it.
	int handle = sceHttpSendRequest(request, NULL, 0);
	psvDebugScreenPrintf("0x%08X sceHttpSendRequest\n", handle);

	// open destination file
	int fh = sceIoOpen(dest, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	psvDebugScreenPrintf("0x%08X sceIoOpen\n", fh);

	// create buffer and counter for read bytes.
	unsigned char data[16*1024];
	int read = 0;
	int wrote = 0;

	// Get the currently downloading file's size
	uint64_t size = 0;
	sceHttpGetResponseContentLength(request, &size);

	sceKernelDelayThread(3*1000*1000); // delay for 3 seconds
	// read data until finished
	while ((read = sceHttpReadData(request, &data, sizeof(data))) > 0) {
		psvDebugScreenClear(COLOR_BLACK);
		// writing the count of read bytes from the data buffer to the file
		int write = sceIoWrite(fh, data, read);
		wrote += write;
		int prog = (wrote*100)/size;
		psvDebugScreenPrintf("%d%% completed\nFile size: %d B\n", prog, size); // grouped together to avoid the size being a negative integer
		psvDebugScreenPrintf("Speed: %d B\n", read);
		psvDebugScreenPrintf("Wrote: %d B\n", wrote);
	}

	// close file
	sceIoClose(fh);

	psvDebugScreenSetFgColor(COLOR_GREEN);
	psvDebugScreenPrintf("\nDownload completed\n");
	psvDebugScreenPrintf("Find it in \"ux0:rbmt.vpk\"\n");
	psvDebugScreenPrintf("App will close in 5 seconds");
	psvDebugScreenSetFgColor(COLOR_WHITE);
	sceKernelDelayThread(5*1000*1000);
	sceKernelExitProcess(0);
}

int main() {
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	sceSysmoduleLoadModule(SCE_SYSMODULE_SSL); // you have to initialize ssl aswell else it'll crash the app
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
	SceNetInitParam netInitParam;
	netInitParam.memory = malloc(1*1024*1024);
	netInitParam.size = 1*1024*1024;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);
	sceNetCtlInit();
	sceHttpInit(1*1024*1024);
	sceSslInit(1*1024*1024);

	psvDebugScreenInit();
	psvDebugScreenSetBgColor(COLOR_BLUE);
	psvDebugScreenPrintf("HTTP(S) download sample");
	psvDebugScreenSetBgColor(COLOR_BLACK);
	psvDebugScreenPrintf("\n\n");
	psvDebugScreenSetFgColor(COLOR_BLUE);
	psvDebugScreenPrintf("Press Cross to download:\n\"https://github.com/HarommelRabbid/RabbidMultiToolVita/releases/download/0.12pre2/Rabbid.MultiTool.0.12pre2.vpk\"\nto\n\"ux0:rbmt.vpk\"\n\n");
	psvDebugScreenSetFgColor(COLOR_RED);
	psvDebugScreenPrintf("Press Circle to exit");
	psvDebugScreenSetFgColor(COLOR_WHITE);

	while(true){
		sceCtrlPeekBufferPositive(0, &pad, 1);

		if((pad.buttons == SCE_CTRL_CROSS) && !(oldpad.buttons == SCE_CTRL_CROSS)){
			//psvDebugScreenClear(COLOR_BLACK);
			download("https://github.com/HarommelRabbid/RabbidMultiToolVita/releases/download/0.12pre2/Rabbid.MultiTool.0.12pre2.vpk", "ux0:rbmt.vpk");
		}else if((pad.buttons == SCE_CTRL_CIRCLE) && !(oldpad.buttons == SCE_CTRL_CIRCLE)) break;

		oldpad = pad;
	}

	sceSslTerm();
	sceHttpTerm();
	sceNetCtlTerm();
	sceNetTerm();

	sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
	sceSysmoduleUnloadModule(SCE_SYSMODULE_SSL);
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);

	return 0;
}
