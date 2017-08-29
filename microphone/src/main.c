/*
	Simple microphone test program
	- displays audio in data
	- displays small VU meter based on
	  an average of the audioIn buffer
	- increase or decrease the sensitivity
	  of the VU meter with up/down buttons
	- exit with select

	Enjoy,
		-pyroesp
*/


#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/audioin.h>
#include <psp2/ctrl.h>

#include <stdlib.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

#define SEC_MULTIPLIER 1000000
#define MAX_WORDS_PER_LINE 23
#define MAX_VU 32

/* Tested sample rates - grain */
/* 16kHz ; 256 */
/* 48kHz ; 768  (= 16kHz*3 ; 256*3) */

int main(int argc, char *argv[]){
	int exit = 0;
	/* sample rate */
	int freq = 16000;
	/* grain */
	int grain = 256;
	/* audio in port */
	int port = 0;

	/* If size > grain */
	/* audio buffer will be filled only up to [grain - 1] */ 
	int size = 256;
	short *audioIn = NULL;

	psvDebugScreenInit();
	printf("Microphone test:\n\n");
	printf("Press up/down for VU sensitivity.\nPress select to quit.\n\n");

	audioIn = (short*)malloc(sizeof(short) * size);
	memset(audioIn, 0, sizeof(short) * size);
	/* Open port */
	port = sceAudioInOpenPort(SCE_AUDIO_IN_PORT_TYPE_VOICE, grain, freq,
				SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO);

	printf("Port value 0x%X - buff size %d\n", port, size);
	
	/* Check for SceAudioInErrorCode enums */
	if (0 > port){
		exit = 1;		
	}

	printf("Audio buff address = 0x%X\n\n", audioIn);
	sceKernelDelayThread(2 * SEC_MULTIPLIER);
	printf("Read audio:\n");

	int i, j;
	int average;
	int audioInMax;
	int sensitivity = 3;
	int retVal;
	int originY = psvDebugScreenCoordY;

	SceCtrlData ctrl, oldCtrl;

	while (!exit){
		average = 0;
		audioInMax = 0;
		psvDebugScreenCoordY = originY;
		psvDebugScreenCoordX = 0;
		/* Read audio */
		retVal = sceAudioInInput(port, (void*)audioIn);
		if (retVal){
			exit = 1;
			break;
		}

		/* Print all values in audio buffer, 23 WORDs per line */
		for (i = 0; i < size; i += MAX_WORDS_PER_LINE){
			for (j = 0; j < MAX_WORDS_PER_LINE && size > (i + j); j++){
				/* remove unwanted values < 0 */
				if (0 > audioIn[i + j])
					audioIn[i + j] = 0;

				printf("%04X ", audioIn[i + j] & 0xFFFF);
				average += audioIn[i + j];

				if (audioInMax < audioIn[i + j])
					audioInMax = audioIn[i + j];
			}		
			printf("\n");
		}
		
		average /= size;
		average = ((average * sensitivity) * MAX_VU) / audioInMax;	
		/* Get microphone status */
		/* Other values than 1 in GetStatus returns 0x80260106 */
		printf("\nYour microphone is %s\n\n", sceAudioInGetStatus(1)?"disabled.": "enabled. ");
		printf("\nSimple VU meter: (sensitivity = %3d)\n\n\n", sensitivity);		
		for (i = 0; i < MAX_VU; i++){
			psvDebugScreenCoordX = 56;
			if (i < average){
				if (MAX_VU/2 > i)
					psvDebugScreenSetBgColor(COLOR_GREEN);
				else if (MAX_VU*3/4 > i)
					psvDebugScreenSetBgColor(COLOR_YELLOW);
				else
					psvDebugScreenSetBgColor(COLOR_RED);

			}else{
				psvDebugScreenSetBgColor(COLOR_BLACK);
			}
			printf("   \n");
		}
		psvDebugScreenSetBgColor(COLOR_BLACK);

		sceCtrlPeekBufferPositive(0, &ctrl, 1);

		if ((ctrl.buttons & SCE_CTRL_UP) && !(oldCtrl.buttons & SCE_CTRL_UP))
			sensitivity++;
		else if ((ctrl.buttons & SCE_CTRL_DOWN) && !(oldCtrl.buttons & SCE_CTRL_DOWN))
			sensitivity--;
		else if (ctrl.buttons & SCE_CTRL_SELECT)
			exit = 1;

		if (0 >= sensitivity)
			sensitivity = 1;

		memcpy(&oldCtrl, &ctrl, sizeof(SceCtrlData));

		sceKernelDelayThread(10000);		
	}

	free(audioIn);
	sceAudioInReleasePort(port);
	sceKernelExitProcess(0);
	return 0;
}
