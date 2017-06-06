#define VITASDK

#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>

#include <psp2/io/fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "debugScreen.h"

void netInit() {
	psvDebugScreenPrintf("Loading module SCE_SYSMODULE_NET\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	
	psvDebugScreenPrintf("Running sceNetInit\n");
	SceNetInitParam netInitParam;
	int size = 1*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	psvDebugScreenPrintf("Running sceNetCtlInit\n");
	sceNetCtlInit();
}

void netTerm() {
	psvDebugScreenPrintf("Running sceNetCtlTerm\n");
	sceNetCtlTerm();

	psvDebugScreenPrintf("Running sceNetTerm\n");
	sceNetTerm();

	psvDebugScreenPrintf("Unloading module SCE_SYSMODULE_NET\n");
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

void httpInit() {
	psvDebugScreenPrintf("Loading module SCE_SYSMODULE_HTTP\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);

	psvDebugScreenPrintf("Running sceHttpInit\n");
	sceHttpInit(1*1024*1024);
}

void httpTerm() {
	psvDebugScreenPrintf("Running sceHttpTerm\n");
	sceHttpTerm();

	psvDebugScreenPrintf("Unloading module SCE_SYSMODULE_HTTP\n");
	sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}

void download(const char *url, const char *dest) {
	psvDebugScreenPrintf("\n\nDownloading %s to %s\n", url, dest);

	// Create template with user agend "PS Vita Sample App"
	int tpl = sceHttpCreateTemplate("PS Vita Sample App", 1, 1);
	psvDebugScreenPrintf("0x%08X sceHttpCreateTemplate\n", tpl);

	// set url on the template
	int conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateConnectionWithURL\n", conn);

	// create the request with the correct method
	int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);
	psvDebugScreenPrintf("0x%08X sceHttpCreateRequestWithURL\n", request);

	// send the actual request. Second parameter would be POST data, third would be length of it.
	int handle = sceHttpSendRequest(request, NULL, 0);
	psvDebugScreenPrintf("0x%08X sceHttpSendRequest\n", handle);

	// open destination file
	int fh = sceIoOpen(dest, SCE_O_WRONLY | SCE_O_CREAT, 0777);
	psvDebugScreenPrintf("0x%08X sceIoOpen\n", fh);

	// create buffer and counter for read bytes.
	unsigned char data[16*1024];
	int read = 0;

	// read data until finished
	while ((read = sceHttpReadData(request, &data, sizeof(data))) > 0) {
		psvDebugScreenPrintf("read %d bytes\n", read);

		// writing the count of read bytes from the data buffer to the file
		int write = sceIoWrite(fh, data, read);
		psvDebugScreenPrintf("wrote %d bytes\n", write);
	}

	// close file
	sceIoClose(fh);
	psvDebugScreenPrintf("sceIoClose\n");

	psvDebugScreenPrintf("\n\n");
}

int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	psvDebugScreenPrintf("HTTP Sample v.1.0 by barooney\n\n");

	netInit();
	httpInit();

	download("http://barooney.com/", "ux0:data/index.html");

	httpTerm();
	netTerm();

	psvDebugScreenPrintf("This app will close in 10 seconds!\n");
	sceKernelDelayThread(10*1000*1000);

	sceKernelExitProcess(0);
	return 0;
}
