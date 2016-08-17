#include <psp2/kernel/processmgr.h>
#include <psp2/touch.h>

#include <stdio.h>
#include <string.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

/* TODO: why touch[port].report[i].force is always at 128 ? */

int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	printf("swipe to the bottom with 1 finger to stop\n");
	/* should use SCE_TOUCH_SAMPLING_STATE_START instead of 1 but old SDK have an invalid values */
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, 1);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);
	
	SceTouchData touch_old[SCE_TOUCH_PORT_MAX_NUM];
	SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];
	while (1) {
		memcpy(touch_old, touch, sizeof(touch_old));
		printf("\e[0;5H");
		int port,i;
		/* sample both back and front surfaces */
		for(port = 0; port < SCE_TOUCH_PORT_MAX_NUM; port++){
			sceTouchPeek(port, &touch[port], 1);
			printf("%s",((const char*[]){"FRONT","BACK "})[port]);
			/* print every touch coordinates on that surface */
			for(i = 0; i < SCE_TOUCH_MAX_REPORT; i++)
				printf("\e[9%im%4i:%-4i ", (i < touch[port].reportNum)? 7:0,
				       touch[port].report[i].x,touch[port].report[i].y);
			printf("\n");
		}

		if ( (touch[SCE_TOUCH_PORT_FRONT].reportNum == 1)
		  && (touch_old[SCE_TOUCH_PORT_FRONT].reportNum == 1)
		  && (touch[SCE_TOUCH_PORT_FRONT].report[0].y >= 1000)
		  && (touch_old[SCE_TOUCH_PORT_FRONT].report[0].y < 1000))
		break;
	}
	sceKernelExitProcess(0);
	return 0;
}
