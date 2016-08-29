#include <stdio.h>
#include <stdbool.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/motion.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

int main(){
	psvDebugScreenInit();
	
	SceCtrlData ctrl;
	float threshold;
	bool is_sampling=false;
	while(1){
		sceCtrlReadBufferPositive(0, &ctrl, 1);
		printf("\e[60;35HPress Start to quit\n");
		if(ctrl.buttons & SCE_CTRL_START)
			break;
		printf("\e[H");/*reset the cursor position to 0,0*/
		
		printf("Sampling:%3s (X:ON, O:OFF)\n",is_sampling?"ON":"OFF");
		if((ctrl.buttons & SCE_CTRL_CROSS) && !is_sampling)
			is_sampling=(sceMotionStartSampling()==0);
		if((ctrl.buttons & SCE_CTRL_CIRCLE) && is_sampling)
			is_sampling=(sceMotionStopSampling()!=0);

		printf("Deadband:%3s ([]: ON, /\\:OFF)\n",sceMotionGetDeadband()?"ON":"OFF");
		if(ctrl.buttons & SCE_CTRL_SQUARE)
			sceMotionSetDeadband(1);
		if(ctrl.buttons & SCE_CTRL_TRIANGLE)
			sceMotionSetDeadband(0);

		threshold = sceMotionGetAngleThreshold();
		printf("AngleThreshold:%+1.3f (Up:+=0.1,Down:-=0.1)\n",threshold);
		if(ctrl.buttons & SCE_CTRL_UP)
			sceMotionSetAngleThreshold(threshold+0.1f);
		if(ctrl.buttons & SCE_CTRL_DOWN)
			sceMotionSetAngleThreshold(threshold-0.1f);

		printf("TiltCorrection:%i (RIGHT:ON, LEFT:OFF)\n",sceMotionGetTiltCorrection());
		if(ctrl.buttons & SCE_CTRL_LEFT)
			sceMotionSetTiltCorrection(0);
		if(ctrl.buttons & SCE_CTRL_RIGHT)
			sceMotionSetTiltCorrection(1);

		printf("\n");
		/* no need to further if we are not sampling */
		if(!is_sampling)
			continue;
			
		printf("Magnetometer:%3s (L:ON, R:OFF)\n",sceMotionGetMagnetometerState()?"ON":"OFF");
		if(ctrl.buttons & SCE_CTRL_LTRIGGER)
			sceMotionMagnetometerOn();
		if(ctrl.buttons & SCE_CTRL_RTRIGGER)
			sceMotionMagnetometerOff();

		printf("\nPress Select to calibrate as Origin\n");
		if(ctrl.buttons & SCE_CTRL_SELECT)
			sceMotionReset();

		SceMotionState state;
		sceMotionGetState(&state);
		printf("Acceleration <x:%+1.3f y:%+1.3f z:%+1.3f>     \n",state.acceleration.x, state.acceleration.y, state.acceleration.z);
		printf("Ang.Velocity <x:%+1.3f y:%+1.3f z:%+1.3f>     \n",state.angularVelocity.x, state.angularVelocity.y, state.angularVelocity.z);
		printf("Orientation  <x:%+1.3f y:%+1.3f z:%+1.3f>     \n",state.basicOrientation.x, state.basicOrientation.y, state.basicOrientation.z);
		printf("Device       <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.deviceQuat.x, state.deviceQuat.y, state.deviceQuat.z, state.deviceQuat.w);
		printf("Rotation.X   <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.rotationMatrix.x.x, state.rotationMatrix.x.y, state.rotationMatrix.x.z, state.rotationMatrix.x.w);
		printf("Rotation.Y   <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.rotationMatrix.y.x, state.rotationMatrix.y.y, state.rotationMatrix.y.z, state.rotationMatrix.y.w);
		printf("Rotation.Z   <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.rotationMatrix.z.x, state.rotationMatrix.z.y, state.rotationMatrix.z.z, state.rotationMatrix.z.w);
		printf("Rotation.W   <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.rotationMatrix.w.x, state.rotationMatrix.w.y, state.rotationMatrix.w.z, state.rotationMatrix.w.w);
		printf("NedMatrix.X  <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.nedMatrix.x.x, state.nedMatrix.x.y, state.nedMatrix.x.z, state.nedMatrix.x.w);
		printf("NedMatrix.Y  <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.nedMatrix.y.x, state.nedMatrix.y.y, state.nedMatrix.y.z, state.nedMatrix.y.w);
		printf("NedMatrix.Z  <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.nedMatrix.z.x, state.nedMatrix.z.y, state.nedMatrix.z.z, state.nedMatrix.z.w);
		printf("NedMatrix.W  <x:%+1.3f y:%+1.3f z:%+1.3f w:%+1.3f>\n",state.nedMatrix.w.x, state.nedMatrix.w.y, state.nedMatrix.w.z, state.nedMatrix.w.w);
		
		printf("\n");
		
		SceMotionSensorState sensor;
		sceMotionGetSensorState(&sensor, 1);
		printf("SensorCounter:%i                \n",sensor.counter);
		printf("accelerometer<x:%+1.3f y:%+1.3f z:%+1.3f>   \n",sensor.accelerometer.x, sensor.accelerometer.y, sensor.accelerometer.z);

		SceFVector3 basicOrientation;
		sceMotionGetBasicOrientation(&basicOrientation);
		printf("basicOrient. <x:%+1.3f y:%+1.3f z:%+1.3f>   \n",basicOrientation.x, basicOrientation.y, basicOrientation.z);
		
		/*		sceMotionRotateYaw(float radians);*/
	}
	sceKernelExitProcess(0);
	return 0;
}
