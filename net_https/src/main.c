
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/clib.h>
#include <psp2/io/fcntl.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>
#include <psp2/paf.h>
#include <psp2/sysmodule.h>
#include <psp2/vshbridge.h>

// module_start param
const char sceUserMainThreadName[]       = "https_sample";
const int sceUserMainThreadPriority      = 0x60;
const SceSize sceUserMainThreadStackSize = 0x1000;

// process param
const int sceKernelPreloadModuleInhibit  = SCE_KERNEL_PRELOAD_INHIBIT_LIBDBG;

// #define GITHUB_TOKEN "token is here"

int https_test(void){

	int res, tpl, conn, req;
	SceUInt64 length = 0;

	SceUID fd;
	void *recv_buffer = NULL;

	SceNetInitParam net_init_param;
	net_init_param.size = 0x800000;
	net_init_param.flags = 0;

	SceUID memid = sceKernelAllocMemBlock("SceNetMemory", 0x0C20D060, net_init_param.size, NULL);
	if(memid < 0){
		sceClibPrintf("sceKernelAllocMemBlock failed (0x%X)\n", memid);
		return memid;
	}

	sceKernelGetMemBlockBase(memid, &net_init_param.memory);

	res = sceNetInit(&net_init_param);
	if(res < 0){
		sceClibPrintf("sceNetInit failed (0x%X)\n", res);
		goto free_memblk;
	}

	res = sceNetCtlInit();
	if(res < 0){
		sceClibPrintf("sceNetCtlInit failed (0x%X)\n", res);
		goto net_term;
	}

	res = sceHttpInit(0x800000);
	if(res < 0){
		sceClibPrintf("sceHttpInit failed (0x%X)\n", res);
		goto netctl_term;
	}

	res = sceSslInit(0x800000);
	if(res < 0){
		sceClibPrintf("sceSslInit failed (0x%X)\n", res);
		goto http_term;
	}

	tpl = sceHttpCreateTemplate("PSP2 GITHUB", 2, 1);
	if(tpl < 0){
		sceClibPrintf("sceHttpCreateTemplate failed (0x%X)\n", tpl);
		goto ssl_term;
	}

	res = sceHttpAddRequestHeader(tpl, "Accept", "application/vnd.github.v3+json", SCE_HTTP_HEADER_ADD);
	sceClibPrintf("sceHttpAddRequestHeader=0x%X\n", res);

#if defined(GITHUB_TOKEN)
	char token[0x80];
	sce_paf_memset(token, 0, sizeof(token));
	sce_paf_snprintf(token, sizeof(token) - 1, "token %s", GITHUB_TOKEN);

	res = sceHttpAddRequestHeader(tpl, "Authorization", token, SCE_HTTP_HEADER_ADD);
	sceClibPrintf("sceHttpAddRequestHeader=0x%X\n", res);
#endif

	const char *user = "vitasdk";
	const char *repo = "vita-headers";
	int page = 1;

	char url[0x80];

	sceClibSnprintf(url, sizeof(url) - 1, "https://api.github.com/repos/%s/%s/pulls?state=closed&sort=updated&direction=desc&page=%d", user, repo, page);

	conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
	if(conn < 0){
		sceClibPrintf("sceHttpCreateConnectionWithURL failed (0x%X)\n", conn);
		goto http_del_temp;
	}

	req = sceHttpCreateRequestWithURL(conn, 0, url, 0);
	if(req < 0){
		sceClibPrintf("sceHttpCreateRequestWithURL failed (0x%X)\n", req);
		goto http_del_conn;
	}

	res = sceHttpSendRequest(req, NULL, 0);
	if(res < 0){
		sceClibPrintf("sceHttpSendRequest failed (0x%X)\n", res);
		goto http_del_req;
	}

	res = sceHttpGetResponseContentLength(req, &length);
	sceClibPrintf("sceHttpGetResponseContentLength=0x%X\n", res);

	if(res < 0){
		recv_buffer = sce_paf_memalign(0x40, 0x40000);
		if(recv_buffer == NULL){
			sceClibPrintf("sce_paf_memalign return to NULL\n");
			goto http_abort_req;
		}

		fd = sceIoOpen("host0:net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
		if(fd < 0)
			fd = sceIoOpen("sd0:net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
		if(fd < 0)
			fd = sceIoOpen("uma0:net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
		if(fd < 0)
			fd = sceIoOpen("ux0:data/net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);

		if(fd < 0){
			sceClibPrintf("sceIoOpen failed (0x%X)\n", fd);
			goto http_abort_req;
		}

		do {
			res = sceHttpReadData(req, recv_buffer, 0x40000);
			if(res > 0){
				res = sceIoWrite(fd, recv_buffer, res);
			}
		} while(res > 0);

		sceIoClose(fd);
	}else{
		sceClibPrintf("length=0x%llX\n", length);

		recv_buffer = sce_paf_memalign(0x40, (SceSize)length);
		if(recv_buffer == NULL){
			sceClibPrintf("sce_paf_memalign return to NULL. length=0x%08X\n", (SceSize)length);
			goto http_abort_req;
		}

		res = sceHttpReadData(req, recv_buffer, (SceSize)length);
		if(res > 0){
			fd = sceIoOpen("host0:net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
			if(fd < 0)
				fd = sceIoOpen("sd0:net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
			if(fd < 0)
				fd = sceIoOpen("uma0:net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
			if(fd < 0)
				fd = sceIoOpen("ux0:data/net_https_git_pr.json", SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);

			if(fd < 0){
				sceClibPrintf("sceIoOpen failed (0x%X)\n", fd);
			}else{
				sceIoWrite(fd, recv_buffer, res);
				sceIoClose(fd);
			}
		}
	}

http_abort_req:
	sceHttpAbortRequest(req);

http_del_req:
	sceHttpDeleteRequest(req);
	req = -1;

http_del_conn:
	sceHttpDeleteConnection(conn);
	conn = -1;

http_del_temp:
	sceHttpDeleteTemplate(tpl);
	tpl = -1;

ssl_term:
	sceSslTerm();

http_term:
	sceHttpTerm();

netctl_term:
	sceNetCtlTerm();

net_term:
	sceNetTerm();

free_memblk:
	sceKernelFreeMemBlock(memid);
	memid = -1;

	sce_paf_free(recv_buffer);
	recv_buffer = NULL;

	return 0;
}

int main(int argc, char **argp){

	int res;
	SceUInt32 paf_init_param[6];
	SceSysmoduleOpt sysmodule_opt;

	paf_init_param[0] = 0x4000000;
	paf_init_param[1] = 0;
	paf_init_param[2] = 0;
	paf_init_param[3] = 0;
	paf_init_param[4] = 0x400;
	paf_init_param[5] = 1;

	res = ~0;
	sysmodule_opt.flags  = 0;
	sysmodule_opt.result = &res;

	sceSysmoduleLoadModuleInternalWithArg(SCE_SYSMODULE_INTERNAL_PAF, sizeof(paf_init_param), &paf_init_param, &sysmodule_opt);
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);

	https_test();

	sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTPS);
	sceSysmoduleUnloadModuleInternal(SCE_SYSMODULE_INTERNAL_PAF);

	sceKernelDelayThread(40000);
	return 0;
}
