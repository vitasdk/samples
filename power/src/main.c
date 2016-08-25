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
	
	printf("\nPress X to set ARM between 444 and 333 mHz\n");
	
	while (1) 
	{
		memset(&pad, 0, sizeof(pad));
		sceCtrlPeekBufferPositive(0, &pad, 1);
		
		//check to see if the user pressed X to toggle CPU speed
		if (pad.buttons & SCE_CTRL_CROSS) 
		{
			if (i == 0) 
			{
				i = scePowerSetArmClockFrequency(444);
				if (i == 0) 
					i = 1;
				else 
				{
					printf("\nCould not set CPU to 333mHz (0x%08X)\n", i);
					i = 0;
				}
			} 
		}
		else 
		{
			i = scePowerSetArmClockFrequency(333);
			if (i != 0) 
			{
				printf("\nCould not set CPU to 222mHz (0x%08X)\n", i);
				i = 1;
			}
		}
	
		if (pad.buttons & SCE_CTRL_START)
			break;
	}

	sceKernelExitProcess(0);

	return 0;
}