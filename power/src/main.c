#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>

#include "debugScreen.h"

/* Define printf, just to make typing easier */
#define printf psvDebugScreenPrintf

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
	printf("Clock frequency of the GPU: %d mHz\n", scePowerGetGpuClockFrequency());
	
	printf("\n\nPress start to exit\n");
	
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