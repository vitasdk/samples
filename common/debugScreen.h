#ifndef DEBUG_SCREEN_H
#define DEBUG_SCREEN_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

#define SCE_DISPLAY_UPDATETIMING_NEXTVSYNC SCE_DISPLAY_SETBUF_NEXTFRAME
#include <psp2/display.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>

#include <debugScreenFont.c>

#define SCREEN_WIDTH    (960)
#define SCREEN_HEIGHT   (544)
#define SCREEN_FB_WIDTH (960)
#define SCREEN_FB_SIZE  (2 * 1024 * 1024)
#define SCREEN_FB_ALIGN (256 * 1024)

#define COLOR_BLACK   0xFF000000
#define COLOR_RED     0xFF0000FF
#define COLOR_BLUE    0xFF00FF00
#define COLOR_YELLOW  0xFF00FFFF
#define COLOR_GREEN   0xFFFF0000
#define COLOR_MAGENTA 0xFFFF00FF
#define COLOR_CYAN    0xFFFFFF00
#define COLOR_WHITE   0xFFFFFFFF
#define COLOR_GREY    0xFF808080

static uint32_t* psvDebugScreenBase;
static uint32_t psvDebugScreenCoordX = 0;
static uint32_t psvDebugScreenCoordY = 0;
static uint32_t psvDebugScreenColorFg;
static uint32_t psvDebugScreenColorBg;
static int psvDebugScreenMutex; /*< avoid race condition when outputing strings */

void psvDebugScreenInit() {
	psvDebugScreenMutex = sceKernelCreateMutex("log_mutex", 0, 0, NULL);

	SceKernelAllocMemBlockOpt opt = { 0 };
	#ifdef VITASDK
	opt.size = 4 * 5;
	#else
	opt.size = sizeof(opt);
	#endif
	opt.attr = 0x00000004;
	opt.alignment = SCREEN_FB_ALIGN;
	SceUID displayblock = sceKernelAllocMemBlock("display", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, SCREEN_FB_SIZE, &opt);
	sceKernelGetMemBlockBase(displayblock, (void**)&psvDebugScreenBase);

	SceDisplayFrameBuf framebuf = {
		.size = sizeof(framebuf),
		.base = psvDebugScreenBase,
		.pitch = SCREEN_WIDTH,
		.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8,
		.width = SCREEN_WIDTH,
		.height = SCREEN_HEIGHT,
	};

	int ret = sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);

	psvDebugScreenColorFg = 0xFFFFFFFF;
	psvDebugScreenColorBg = 0x00000000;
}

void psvDebugScreenClear(int bg_color){
	psvDebugScreenCoordX = psvDebugScreenCoordY = 0;
	int i;
	for(i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
		psvDebugScreenBase[i] = bg_color;
	}
}

uint32_t psvDebugScreenSetFgColor(uint32_t color) {
	uint32_t prev_color = psvDebugScreenColorFg;
	psvDebugScreenColorFg = color;
	return prev_color;
}

uint32_t psvDebugScreenSetBgColor(uint32_t color) {
	uint32_t prev_color = psvDebugScreenColorBg;
	psvDebugScreenColorBg = color;
	return prev_color;
}

void psvDebugScreenSetCoordX(uint32_t x) {
	psvDebugScreenCoordX = x;
}

void psvDebugScreenSetCoordY(uint32_t y) {
	psvDebugScreenCoordY = y;
}

int psvDebugScreenPuts(const char * text){
	int c, i, j, l;
	uint8_t *font;
	uint32_t *vram_ptr;
	uint32_t *vram;

	sceKernelLockMutex(psvDebugScreenMutex, 1, NULL);

	for (c = 0; text[c] != '\0' ; c++) {
		if (psvDebugScreenCoordX + 8 > SCREEN_WIDTH) {
			psvDebugScreenCoordY += 8;
			psvDebugScreenCoordX = 0;
		}
		if (psvDebugScreenCoordY + 8 > SCREEN_HEIGHT) {
			psvDebugScreenClear(psvDebugScreenColorBg);
		}
		char ch = text[c];
		if (ch == '\n') {
			psvDebugScreenCoordX = 0;
			psvDebugScreenCoordY += 8;
			continue;
		} else if (ch == '\r') {
			psvDebugScreenCoordX = 0;
			continue;
		}

		vram = psvDebugScreenBase + psvDebugScreenCoordX + psvDebugScreenCoordY * SCREEN_FB_WIDTH;

		font = &psvDebugScreenFont[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) {
			vram_ptr  = vram;
			for (j = 0; j < 8; j++) {
				if ((*font & (128 >> j))) *vram_ptr = psvDebugScreenColorFg;
				else *vram_ptr = psvDebugScreenColorBg;
				vram_ptr++;
			}
			vram += SCREEN_FB_WIDTH;
		}
		psvDebugScreenCoordX += 8;
	}

	sceKernelUnlockMutex(psvDebugScreenMutex, 1);
	return c;
}

int psvDebugScreenPrintf(const char *format, ...) {
	char buf[512];

	va_list opt;
	va_start(opt, format);
	int ret = vsnprintf(buf, sizeof(buf), format, opt);
	psvDebugScreenPuts(buf);
	va_end(opt);

	return ret;
}


#endif