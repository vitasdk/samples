#ifdef __vita__
#include <psp2/kernel/processmgr.h>
#else
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define sceKernelDelayThread(L) 0/*usleep*/
#endif

#include "debugScreen.h"
#define printf  psvDebugScreenPrintf
#define puts(S) psvDebugScreenPuts(S"\n")

//                        RRGGBB
#define CUSTOM_BG_BLUE  0x00003F  // "dark" blue
#define CUSTOM_BG_RED   0x3F0000  // "dark" red
#define CUSTOM_BG_GREEN 0x003F00  // "dark" green

/* Custom font, used for the font swaping demo */
PsvDebugScreenFont psvDebugScreenFont_xs = {glyphs:(unsigned char*)
"\x00\x00\x00\x44\x44\x04\x55\x00\x00\x05\xf6\xfa\x27\xa6\x3e\x05\x16\x8a\x4a\x4b\xa5\x44\x40\x00"
"\x12\x44\x21\x42\x11\x24\x96\xf6\x90\x04\xe4\x00\x00\x02\x24\x00\xe0\x00\x00\x00\x66\x11\x24\x88"
"\x69\xbd\x96\x26\x22\x27\x69\x16\x8f\x69\x21\x96\x35\x9f\x11\xf8\xe1\x1e\x78\xe9\x96\xf1\x24\x44"
"\x69\x69\x96\x69\x71\x16\x00\x20\x20\x02\x02\x24\x24\x84\x20\x0f\x0f\x00\x42\x12\x40\x69\x12\x02"
"\xe1\x5d\xd6\x69\xf9\x99\xe9\xe9\x9e\x78\x88\x87\xe9\x99\x9e\xf8\xe8\x8f\xf8\xe8\x88\x78\x8b\x96"
"\x99\xf9\x99\xe4\x44\x4e\x71\x11\x96\x9a\xca\x99\x88\x88\x8f\x9f\xf9\x99\x9d\xdb\xb9\x69\x99\x96"
"\xe9\xe8\x88\x69\x9b\xa5\xe9\xe9\x99\x78\x61\x1e\xe4\x44\x44\x99\x99\x96\x99\x9a\xc8\x99\x9f\xf9"
"\x99\x69\x99\x99\x71\x1e\xf1\x24\x8f\x74\x44\x47\x88\x42\x11\x71\x11\x17\x69\x00\x00\x00\x00\x0f"
"\x42\x10\x00\x06\x17\x97\x08\x8e\x9e\x07\x88\x87\x01\x17\x97\x06\x9f\x87\x03\x4f\x44\x06\x97\x16"
"\x08\x8e\x99\x40\x44\x44\x20\x22\xa4\x09\xac\xa9\x04\x44\x46\x09\xff\x99\x0e\x99\x99\x06\x99\x96"
"\x0e\x9e\x88\x07\x97\x11\x0b\xc8\x88\x07\x86\x1e\x4f\x44\x43\x09\x99\x96\x09\x9a\xc8\x09\x9f\xf9"
"\x09\x96\x99\x09\x97\x16\x0f\x12\x4f\x64\x88\x46\x44\x44\x44\x62\x11\x26\x0d\xb0\x00\x00\x00\x00",
width:4, height:6, first :32, last:127, size_w:5, size_h:8 };

int main(int argc, char *argv[]) {(void)argc;(void)argv;
	PsvDebugScreenFont *psvDebugScreenFont_default_1x;
	PsvDebugScreenFont *psvDebugScreenFont_default_2x;
	PsvDebugScreenFont *psvDebugScreenFont_previous;
	PsvDebugScreenFont *psvDebugScreenFont_current;
	unsigned char backup;
	int oldCoordX, oldCoordY;
	int i, j;
	int modes[]={3,9,4,10};
	int mode;
	int code1, code2;
	ColorState colorsCopy;
	uint32_t color;

	psvDebugScreenInit();
	// get default font
	psvDebugScreenFont_previous = psvDebugScreenFont_default_1x = psvDebugScreenGetFont();
	// create a scaled by 2 version of default font
	psvDebugScreenFont_default_2x = psvDebugScreenScaleFont2x(psvDebugScreenFont_default_1x);
	if (!psvDebugScreenFont_default_2x) {
		// TODO: error message check font data
		psvDebugScreenFont_default_2x = psvDebugScreenFont_default_1x;
	} else {
		// set scaled default font
		psvDebugScreenFont_current = psvDebugScreenSetFont(psvDebugScreenFont_default_2x);
		if (psvDebugScreenFont_current != psvDebugScreenFont_default_2x) { // font was not set
			// TODO: error message check font data
		}
	}

	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE); // if reset, the BG color will be default again (normally black)

	printf("\e[2J");                    /* Code J Clear (2=whole screen)*/
	printf("Will be cleared\nThis too Left behind" "\e[11D");
	psvDebugScreenSetBgColor(CUSTOM_BG_RED);
	printf("\e[1J" "\n");/* Code J Clear (1=up to the beginning of the screen)*/
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);

	printf("Welcome " // strings can be split on multiple parts
	       "to the psvDebug" // they will be join at compilation anyway
	       "Screen showcase !\n\n"); // this will be used later for CSI
	/* the \r character will move back the cursor to the begining of the line
	   Then the rest of the text will print "Cariage" over "my dummy" */
	printf("my dummy return demo\rCarriage\n");

	/*
	 * Terminal pimping using CSI sequence "\e[#;#;#X"
	 * where X is the CSI code, #;#;# are the comma separated params
	 * see https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_sequences
	 * Colors = 0:black, 1:red, 2: green, 3:yellow, 4:blue, 5:magenta, 6:cyan, 7:white
	 */

	/* Code m: Select Graphic Rendition (SGR), syntax=\e[#;#;...#m */
	printf("\e[31m"     "A Red text ");            /* 3X = set the foreground color to X */
	printf("\e[30;42m"  "Black text on Green BG ");/* 4X = set the background color to X */
	printf("\e[7m"      "inverted");               /* 7 = invert FG/BG color */
	printf("\e[27m"     "reverted");               /* 27 = disable inversion */
	printf("\e[39;49m"  "default\n");              /* 39/49 = reset FG/BG color */
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);

	printf("\e[97m"     "White+ text ");           /* 9X = set bright foreground color (keep green BG)*/
	printf("\e[91;106m" "Red+ text on Cyan+ BG "); /* 10X= set bright background color */
	printf("\e[7m"      "inverted");
	printf("\e[m"       "default\n");              /* no param = reset FG/BG/inversion */
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);

	printf("---->\n"
	       "---->");
	printf("\e[1A"   "1Up");        /* Code A: Moves cursor up # lines */
	printf("\e[4C"   "4right");     /* Code C: Moves cursor forward # spaces */
	printf("\e[1B"   "1Down");      /* Code B: Moves cursor down # lines */
	printf("\e[18D"  "18Left\n");   /* Code D: Moves cursor back # spaces */
	printf("\e[s");                 /* Code s: Saves cursor position */
	printf("\e[4;9H" "Text at 4:9");/* Code H: or Code f : Moves cursor to a line;column */
	printf("\e[u"    "I'm back\n"); /* Code u: Return to saved cursor position */
	// Line clearing demo: print "pingpong" then place the cursor in the middle
	printf("\e[37;2;100mpingpong" "\e[4D"); /* "dark" white on "bright" black */
	psvDebugScreenSetBgColor(CUSTOM_BG_RED);
	printf("\e[0K" "\n");          /* Code K: Clear 0=cur to EOL */
	printf("\e[m");                /* no param = reset FG/BG */
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);
	//
	printf("pingpong" "\e[4D");
	psvDebugScreenSetBgColor(CUSTOM_BG_GREEN);
	printf("\e[1K" "\n");/* Code K: Clear 1=BOL to cur */
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);
	//
	printf("pingpong" "\e[4D");
	psvDebugScreenSetBgColor(CUSTOM_BG_RED);
	printf("\e[2K" "\n");/* Code K: Clear 2=whole line */
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);
	// Screen clearing demo: print "ping\npong" then place the cursor back to the first string line
	printf("pingn\npong" "\e[1A");
	psvDebugScreenSetBgColor(CUSTOM_BG_GREEN);
	printf("\e[0J" "\e[1B");/* Code J: Clear 0=cur to end of screen */
	psvDebugScreenSetBgColor(CUSTOM_BG_BLUE);

	// Missing glyph demo: shows simple dummy glyph
	backup  = psvDebugScreenFont_current->last;
	psvDebugScreenFont_current->last = 0;
	printf("missing\n");
	psvDebugScreenFont_current->last = backup;

	// Font manipulation demo
	psvDebugScreenFont_current->size_w -= 2;  // May lead to incomplete wide glyphs
	printf("This is a narrow text\n");
	psvDebugScreenFont_current->size_w += 2;

	// Font exchange demo
	psvDebugScreenFont_previous = psvDebugScreenGetFont();; // backup current font
	psvDebugScreenFont_current = psvDebugScreenSetFont(&psvDebugScreenFont_xs); // switch to our font
	if (psvDebugScreenFont_current != &psvDebugScreenFont_xs) { // font was not set
		// TODO: error message check font data
	}
	printf("Glyph Table using custom font:\n");
	for (i = psvDebugScreenFont_current->first; i <= psvDebugScreenFont_current->last; i++) {
		if ((i%32==0)||(i==psvDebugScreenFont_current->first)) printf("%02X:", i);
		printf("%c", ((i>='\n')&&(i<='\r'))?' ':i);
		if (i%32==31) printf("\n");
	}

	// Pixel exact positioning, e.g. useful when working with different font heights
	oldCoordX = 0;
	psvDebugScreenGetCoordsXY(NULL, &oldCoordY);
	psvDebugScreenPuts("\e[H");
	psvDebugScreenSetCoordsXY(&oldCoordX, &oldCoordY);

	psvDebugScreenFont_current = psvDebugScreenSetFont(psvDebugScreenFont_previous); // restore previous font
	if (psvDebugScreenFont_current != psvDebugScreenFont_previous) { // font was not set
		// TODO: error message check font data
	}
	printf("Glyph Table using default font:\n");
	for (i = psvDebugScreenFont_current->first; i <= psvDebugScreenFont_current->last; i++) {
		if ((i%32==0)||(i==psvDebugScreenFont_current->first)) printf("%02X:", i);
		printf("%c", ((i>='\n')&&(i<='\r'))?' ':i);
		if (i%32==31) printf("\n");
	}

	// show all colors as hex code
	for (i = 0; i < (sizeof(modes)/sizeof(*modes)); i++) {
		mode = modes[i];
		code1 = mode * 10;
		code2 = code1 + (mode&1 ? 10 : -10); // black <-> white
		for (j = 0; j < 8; j++, code1++) {
			code2 -= code2 % 10;
			if ((j == 0)||(j == 1)||(j == 4)||(j == 5)) code2 += 7; // color <-> black
			//
			printf("\e[%im\e[%im", code1, code2);
			psvDebugScreenGetColorStateCopy(&colorsCopy);
			color = mode&1 ? colorsCopy.color_fg : colorsCopy.color_bg;
			printf(" %i=%06x ", code1, (color & 0x00FFFFFF));
		}
	}

	#ifndef __vita__ // You can generate a RGB screen dump when building on PC (for easy testing)
	//convert the dump into PNG with: convert -depth 8 -size 960x544+0 RGB:screen.data out.png;
	int fdump = open("screen.data", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	for (int i=0;i<sizeof(base);i += 4)write(fdump, base+i, 3);
	close(fdump);
	#endif
	return sceKernelDelayThread(~0);
}
