#include <stdint.h>
#include <math.h>

#include <psp2/ctrl.h>
#include <psp2/audioout.h>
#include <psp2/kernel/processmgr.h>

#include "debugScreen.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_speech.h"

SoLoud::Soloud gSoloud; // SoLoud engine
SoLoud::Wav gWave;      // One wave file

#define printf psvDebugScreenPrintf

int main(void) {
	psvDebugScreenInit();

	printf("soloud demo\n");

	gSoloud.init(); // Initialize SoLoud
	printf("init\n");
	gWave.load("app0:/music.ogg"); // Load a wave
	printf("load\n");
	gSoloud.play(gWave); // Play the wave
	SoLoud::Speech sp;
	sp.setText("Hello world.  You will be assimilated.");
	gSoloud.play(sp);
	printf("play\n");

	while (1) {
		sceKernelDelayThread(1000 * 1000);
	}


	sceKernelExitProcess(0);
	return 0;
}