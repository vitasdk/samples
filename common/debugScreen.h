#ifndef DEBUG_SCREEN_H
#define DEBUG_SCREEN_H

#include <stdbool.h> // for bool

#include "debugScreen_custom.h"

typedef struct ColorState {
	bool fgTrueColorFlag; // flag if truecolors or ANSI/VTERM/GREYSCALE colors are used
	bool bgTrueColorFlag; // flag if truecolors or ANSI/VTERM/GREYSCALE colors are used
	// truecolors
	uint32_t fgTrueColor; // color in RGB (internal BGR)
	uint32_t bgTrueColor; // color in RGB (internal BGR)
	// ANSI/VTERM/GREYSCALE colors
	unsigned char fgIndex; // ANSI/VTERM/GREYSCALE color code (0-255)
	unsigned char fgIntensity; // 22=normal, 1=increased ("bright"), 2=decreased ("dark")
	unsigned char bgIndex; // ANSI/VTERM/GREYSCALE color code (0-255)
	unsigned char bgIntensity; // 22=normal, 1=increased ("bright")
	bool inversion; // flag if bg/fg colors are inverted

	// default colors (ANSI/VTERM/GREYSCALE)
	unsigned char fgIndexDefault; // default ANSI/VTERM/GREYSCALE color code
	unsigned char fgIntensityDefault; // 22=normal, 1=increased, 2=decreased
	unsigned char bgIndexDefault; // default ANSI/VTERM/GREYSCALE color code
	unsigned char bgIntensityDefault; // 22=normal, 1=increased
	bool inversionDefault; // flag if bg/fg colors are inverted

	// current colors (e.g. inverted)
	uint32_t color_fg; // color in RGB (internal BGR)
	uint32_t color_bg; // color in RGB (internal BGR)
} ColorState;

typedef struct PsvDebugScreenFont {
	unsigned char *glyphs, width, height, first, last, size_w, size_h; // only values 0-255
} PsvDebugScreenFont;

#define SCREEN_WIDTH    (960) // screen resolution x
#define SCREEN_HEIGHT   (544) // screen resolution y

#ifdef DEBUG_SCREEN_CODE_INCLUDE // needed once inside a C++ project (e.g. main.cpp)
#include "debugScreen.c"
#else
extern int psvDebugScreenInit();
extern int psvDebugScreenPuts(const char * _text);
extern int psvDebugScreenPrintf(const char *format, ...);
extern void psvDebugScreenGetColorStateCopy(ColorState *copy);
extern void psvDebugScreenGetCoordsXY(int *x, int *y);
extern void psvDebugScreenSetCoordsXY(int *x, int *y);
extern PsvDebugScreenFont *psvDebugScreenGetFont(void);
extern PsvDebugScreenFont *psvDebugScreenSetFont(PsvDebugScreenFont *font);
extern PsvDebugScreenFont *psvDebugScreenScaleFont2x(PsvDebugScreenFont *source_font);
#endif

#endif
