#include <string.h>
#include <stdbool.h>
#include <psp2/types.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/message_dialog.h>
#include <psp2/ime_dialog.h>
#include <psp2/display.h>
#include <psp2/apputil.h>
#include <psp2/gxm.h>
#include <psp2/kernel/sysmem.h>

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))
#define DISPLAY_WIDTH			960
#define DISPLAY_HEIGHT			544
#define DISPLAY_STRIDE_IN_PIXELS	1024
#define DISPLAY_BUFFER_COUNT		2
#define DISPLAY_MAX_PENDING_SWAPS	1

typedef struct{
	void*data;
	SceGxmSyncObject*sync;
	SceGxmColorSurface surf;
	SceUID uid;
}displayBuffer;

unsigned int backBufferIndex = 0;
unsigned int frontBufferIndex = 0;
/* could be converted as struct displayBuffer[] */
displayBuffer dbuf[DISPLAY_BUFFER_COUNT];

void *dram_alloc(unsigned int size, SceUID *uid){
	void *mem;
	*uid = sceKernelAllocMemBlock("gpu_mem", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, ALIGN(size,256*1024), NULL);
	sceKernelGetMemBlockBase(*uid, &mem);
	sceGxmMapMemory(mem, ALIGN(size,256*1024), SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE);
	return mem;
}
void gxm_vsync_cb(const void *callback_data){
	sceDisplaySetFrameBuf(&(SceDisplayFrameBuf){sizeof(SceDisplayFrameBuf),
		*((void **)callback_data),DISPLAY_STRIDE_IN_PIXELS, 0,
		DISPLAY_WIDTH,DISPLAY_HEIGHT}, SCE_DISPLAY_SETBUF_NEXTFRAME);
}
void gxm_init(){
	sceGxmInitialize(&(SceGxmInitializeParams){0,DISPLAY_MAX_PENDING_SWAPS,gxm_vsync_cb,sizeof(void *),SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE});
	unsigned int i;
	for (i = 0; i < DISPLAY_BUFFER_COUNT; i++) {
		dbuf[i].data = dram_alloc(4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT, &dbuf[i].uid);
		sceGxmColorSurfaceInit(&dbuf[i].surf,SCE_GXM_COLOR_FORMAT_A8B8G8R8,SCE_GXM_COLOR_SURFACE_LINEAR,SCE_GXM_COLOR_SURFACE_SCALE_NONE,SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,DISPLAY_WIDTH,DISPLAY_HEIGHT,DISPLAY_STRIDE_IN_PIXELS,dbuf[i].data);
		sceGxmSyncObjectCreate(&dbuf[i].sync);
	}
}
void gxm_swap(){
	sceGxmPadHeartbeat(&dbuf[backBufferIndex].surf, dbuf[backBufferIndex].sync);
	sceGxmDisplayQueueAddEntry(dbuf[frontBufferIndex].sync, dbuf[backBufferIndex].sync, &dbuf[backBufferIndex].data);
	frontBufferIndex = backBufferIndex;
	backBufferIndex = (backBufferIndex + 1) % DISPLAY_BUFFER_COUNT;
}
void gxm_term(){
	sceGxmTerminate();
	/*TODO*/
}

#define LEN SCE_IME_DIALOG_MAX_TEXT_LENGTH

int main(int argc, const char *argv[]) {
	sceAppUtilInit(&(SceAppUtilInitParam){}, &(SceAppUtilBootParam){});
	sceCommonDialogSetConfigParam(&(SceCommonDialogConfigParam){});

	gxm_init();
	
	int shown_dial = 0;
	bool said_yes = false;
	while (!said_yes) {
		//clear current screen buffer
		memset(dbuf[backBufferIndex].data,0xff000000,DISPLAY_HEIGHT*DISPLAY_STRIDE_IN_PIXELS*4);

		uint16_t input[LEN + 1];
		if(!shown_dial)
			shown_dial = sceImeDialogInit(&(SceImeDialogParam){.title=u"say yes!", LEN, u"nah", input}) > 0;
		
		if (sceImeDialogGetStatus() == SCE_COMMON_DIALOG_STATUS_FINISHED) {
			SceImeDialogResult result={};
			sceImeDialogGetResult(&result);
			uint16_t*last_input = (result.button == SCE_IME_DIALOG_BUTTON_CLOSE) ? u"" : input;
			said_yes=!memcmp(last_input,u"yes",4*sizeof(u' '));
			sceImeDialogTerm();
			shown_dial = 0;/*< to respawn sceImeDialogInit on next loop */
		}
		
		sceCommonDialogUpdate(&(SceCommonDialogUpdateParam){{
			NULL,dbuf[backBufferIndex].data,0,0,
			DISPLAY_WIDTH,DISPLAY_HEIGHT,DISPLAY_STRIDE_IN_PIXELS},
			dbuf[backBufferIndex].sync});

		gxm_swap();
		sceDisplayWaitVblankStart();
	}
	gxm_term();
	sceKernelExitProcess(0);
	return 0;
}
