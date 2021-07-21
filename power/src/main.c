#include <stdio.h>
#include <string.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>
#include <psp2/rtc.h>

#include "debugScreen.h"

/* Define printf, just to make typing easier */
#define printf psvDebugScreenPrintf


/* Power Callback - Runs when anything related do with power happens;
	 e.g. pressed power button, sleep, etc. */
int powerCallback(int notifyId, int notifyCount, int powerInfo, void *common) {
	// doesn't work without a file handle open...?
	FILE *fd = fopen("ux0:data/test_power_cb.txt", "w");

	SceDateTime time;
	sceRtcGetCurrentClockLocalTime(&time);
	printf("%04d/%02d/%02d %02d:%02d:%02d:%05d notifyId %i, notifyCount %i, powerInfo 0x%08X\n", 
		sceRtcGetYear(&time), sceRtcGetMonth(&time), sceRtcGetDay(&time),
		sceRtcGetHour(&time), sceRtcGetMinute(&time), sceRtcGetSecond(&time), sceRtcGetMicrosecond(&time),
		notifyId, notifyCount, powerInfo);

	fclose(fd);
}

/* Our thread to handle any callbacks. */
int CallbackThread(SceSize args, void *argp) {
	// create and register a callback in this thread.
	int cbid;
	cbid = sceKernelCreateCallback("Power Callback", 0, powerCallback, NULL);
	int registered = scePowerRegisterCallback(cbid);
	printf("\e[16;0H Callback registered id: %i  registered: %i.\n\n", cbid, registered);

	// this thread only handles callbacks.
	// so delay it forever, but handle callbacks.
	while (1) {
		sceKernelDelayThreadCB(10 * 1000 * 1000);
	}

	return registered;
}

/* Sets up the callback thread and returns its thread id */
int setupCallbacks(void) {
	int thid = 0;

	// The main thread does not handle callbacks, so we need to make one to handle them.
	thid = sceKernelCreateThread("callbackThread", CallbackThread, 0x10000100, 0x10000, 0, 0, NULL);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, 0);

	printf("\e[15;0H Thread created %i\n.", thid);
	return thid;
}

/* main routine */
int main(int argc, char *argv[]) 
{	
	SceCtrlData pad;
	int i = 0;
	int batteryLifeTime = 0;

	psvDebugScreenInit();
	
	printf("PS Vita Power Sample v0.1\n\n");
	printf("External power: %s\n", scePowerIsPowerOnline()? "yes" : "no ");
	printf("Low charge: %s\n", scePowerIsLowBattery()? "yes" : "no ");
	printf("Charging: %s\n", scePowerIsBatteryCharging()? "yes" : "no ");
	printf("Battery life percent: %d%%\n", scePowerGetBatteryLifePercent());
	batteryLifeTime = scePowerGetBatteryLifeTime();
	printf("Battery life time: (%02dh%02dm)\n", batteryLifeTime/60, batteryLifeTime-(batteryLifeTime/60*60));
	printf("Clock frequency of the ARM: %d mHz\n", scePowerGetArmClockFrequency());
	printf("Clock frequency of the BUS: %d mHz\n", scePowerGetBusClockFrequency());
	SceInt32 corefreq = 0, mpfreq = 0;
	scePowerGetGpuClockFrequency(&corefreq, &mpfreq);
	printf("Clock frequency of the GPU: core %d mHz, mp %d mHz\n", corefreq, mpfreq);

	printf("\n\nExperiment by sleeping vita and turning back on. Press start to exit. \n");
	setupCallbacks();

	while (1)
	{
		memset(&pad, 0, sizeof(pad));
		sceCtrlPeekBufferPositive(0, &pad, 1);
		
		if (pad.buttons & SCE_CTRL_START)
			break;
	}

	sceKernelExitProcess(0);

	return 0;
}
