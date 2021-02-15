#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>

#include <psp2/io/fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "debugScreen.h"

#define debugPrintf(...) psvDebugScreenPrintf(__VA_ARGS__); \
                         printf(__VA_ARGS__)

void netInit() {
	debugPrintf("Running sceNetInit\n");
	SceNetInitParam netInitParam;
	int size = 16*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	int ret = sceNetInit(&netInitParam);
	debugPrintf("0x%08X sceNetInit\n", ret);

	debugPrintf("Running sceNetCtlInit\n");
	ret = sceNetCtlInit();
	debugPrintf("0x%08X sceNetCtlInit\n", ret);
}

void netTerm() {
	debugPrintf("Running sceNetCtlTerm\n");
	sceNetCtlTerm();

	debugPrintf("Running sceNetTerm\n");
	sceNetTerm();
}

void httpLoad() {
	debugPrintf("Loading module SCE_SYSMODULE_HTTPS\n");
	int ret = sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);
	debugPrintf("0x%08X sceSysmoduleLoadModule\n", ret);
}

void httpInit() {
	debugPrintf("Running sceHttpInit\n");
	int ret = sceHttpInit(64*1024);
	debugPrintf("0x%08X sceHttpInit\n", ret);
}

void sslInit() {
	debugPrintf("Running sceSslInit\n");
	int ret = sceSslInit(512*1024);
	debugPrintf("0x%08X sceSslInit\n", ret);
}

void sslTerm() {
	debugPrintf("Running sceSslEnd\n");
	sceSslEnd();
}


void download(const char *url, const char *dest) {
	SceInt32 result = 0;
	SceULong64 contentLength;
	SceInt32 statusCode;

	debugPrintf("\n\nDownloading %s to %s\n", url, dest);

	// Create template with user agend "PS Vita Sample App"
	int tpl = sceHttpCreateTemplate("PS Vita Sample App", SCE_HTTP_VERSION_1_1, 1);
	debugPrintf("0x%08X sceHttpCreateTemplate\n", tpl);

	// set url on the template
	int conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
	debugPrintf("0x%08X sceHttpCreateConnectionWithURL\n", conn);

	// create the request with the correct method
	int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);
	debugPrintf("0x%08X sceHttpCreateRequestWithURL\n", request);

	// Add request header. Last param is 
	result = sceHttpAddRequestHeader(request, "X-Test-Header", "some-value", SCE_HTTP_HEADER_OVERWRITE);
	debugPrintf("0x%08X sceHttpAddRequestHeader\n", result);

	// send the actual request. Second parameter would be POST data, third would be length of it.
	result = sceHttpSendRequest(request, NULL, 0);
	debugPrintf("0x%08X sceHttpSendRequest\n", result);

	// check result and status code
	if (result == 0) {
		result = sceHttpGetStatusCode(request, &statusCode);
		debugPrintf("response code = %d\n", statusCode);

		if(statusCode == 200) {
			// this can return SCE_HTTP_ERROR_CHUNK_ENC for chunked encoding, or no Content-Length at all, which is totally valid
			result = sceHttpGetResponseContentLength(request, &contentLength);
			if(result < 0) {
				debugPrintf("sceHttpGetContentLength() error: 0x%08X\n", result);
			} else {
				debugPrintf("Content-Length = %lld\n", contentLength);
			}

			// open destination file
			int fh = sceIoOpen(dest, SCE_O_WRONLY | SCE_O_CREAT, 0777);
			debugPrintf("0x%08X sceIoOpen\n", fh);

			// create buffer and counter for read bytes.
			unsigned char data[16*1024];
			int read = 0;

			// read data until finished
			while ((read = sceHttpReadData(request, &data, sizeof(data))) > 0) {
				debugPrintf("read %d bytes\n", read);

				// writing the count of read bytes from the data buffer to the file
				int write = sceIoWrite(fh, data, read);
				debugPrintf("wrote %d bytes\n", write);
			}

			// close file
			sceIoClose(fh);
			debugPrintf("sceIoClose\n");
		}
	}

	debugPrintf("\n\n");
}

int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	psvDebugScreenPrintf("HTTP Sample v.1.1 by barooney\n\n");

	httpLoad();
	netInit();
	sslInit();
	httpInit();

	download("https://google.com/", "ux0:data/index.html");

	sslTerm();
	netTerm();

	debugPrintf("This app will close in 10 seconds!\n");
	sceKernelDelayThread(10*1000*1000);

	sceKernelExitProcess(0);
	return 0;
}
