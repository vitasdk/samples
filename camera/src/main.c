#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/camera.h>
#include <psp2/kernel/processmgr.h>

#define ARRAYSIZE(x) (sizeof(x)/sizeof(*x))
#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 368

int main(void){
	void* base;
	SceUID memblock = sceKernelAllocMemBlock("camera", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, 256 * 1024 * 5, NULL);
	sceKernelGetMemBlockBase(memblock, &base);

	SceDisplayFrameBuf dbuf = { sizeof(SceDisplayFrameBuf), base, DISPLAY_WIDTH, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT};

	int cur_res = 1;// 1-8 (maybe more)
	int cur_cam = 0;//front:0, back:1
	int cur_fps = 6;
	int cur_fmt = 5;

	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
	SceCtrlData ctrl_peek;SceCtrlData ctrl_press;
	do{
		int cur_sat=0,cur_rev=0,cur_efx=0,cur_iso=0,cur_zom=10,cur_nim=0,cur_wbl=0;
		size_t res_table[][2] = {{0,0},{640,480},{320,240},{160,120},{352,288},{176,144},{480,272},{640,360},{640,360}};
		size_t fps_table[] = {3,5,7,10,15,20,30};
		sceCameraOpen(cur_cam, &(SceCameraInfo){
			.size = sizeof(SceCameraInfo),
			.format = cur_fmt,//422_TO_ABGR
			.resolution = cur_res,
			.framerate = fps_table[cur_fps],
			.sizeIBase = 4*res_table[cur_res][0]*res_table[cur_res][1],
			.pitch     = 4*(DISPLAY_WIDTH-res_table[cur_res][0]),
			.pIBase = base,
		});
		sceCameraStart(cur_cam);
		do{
			ctrl_press = ctrl_peek;
			sceCtrlPeekBufferPositive(0, &ctrl_peek, 1);
			ctrl_press.buttons = ctrl_peek.buttons & ~ctrl_press.buttons;

			sceCameraSetBrightness(cur_cam, ctrl_press.rx);
			sceCameraSetContrast(cur_cam, ctrl_press.ry-100);
			sceCameraSetEV(cur_cam, (ctrl_press.lx-128)/7);//-20...20
			sceCameraSetGain(cur_cam, (ctrl_press.ly<100)?ctrl_press.ly/6:0);//0..16 //don't seem to work
			if(ctrl_press.buttons & SCE_CTRL_TRIANGLE)
				sceCameraSetSaturation(cur_cam,   cur_sat=((cur_sat+5)%45));
			if(ctrl_press.buttons & SCE_CTRL_CIRCLE)
				sceCameraSetReverse(cur_cam,      cur_rev=((cur_rev+1)%4));
			if(ctrl_press.buttons & SCE_CTRL_CROSS)
				sceCameraSetISO(cur_cam,(int[]){1,100,200,400}[cur_iso=((cur_iso+1)%4)]);
			if(ctrl_press.buttons & SCE_CTRL_SQUARE)
				sceCameraSetEffect(cur_cam,       cur_efx=((cur_efx+1)%7));
			if(ctrl_press.buttons & SCE_CTRL_LTRIGGER)
				sceCameraSetWhiteBalance(cur_cam, cur_wbl=((cur_wbl+1)%4));
			if(ctrl_press.buttons & SCE_CTRL_RTRIGGER)
				sceCameraSetNightmode(cur_cam,    cur_nim=((cur_nim+1)%4));
			if(ctrl_press.buttons & SCE_CTRL_SELECT)
				sceCameraSetZoom(cur_cam,         cur_zom=((cur_zom+1)%41));
			/* TODO: find more button combo to trigg :
				sceCameraSetAntiFlicker(int cur_cam, int mode);
				sceCameraSetBacklight(int cur_cam, int mode);
			*/
			if(sceCameraIsActive(cur_cam)>0){
				SceCameraRead read = {sizeof(SceCameraRead),0/*<Blocking*/};
				sceCameraRead(cur_cam, &read);
				sceDisplaySetFrameBuf(&dbuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
			}
		}while(!(ctrl_press.buttons & (SCE_CTRL_UP|SCE_CTRL_DOWN|SCE_CTRL_LEFT|SCE_CTRL_RIGHT|SCE_CTRL_START)));
		sceCameraStop(cur_cam);
		sceCameraClose(cur_cam);
		if(ctrl_press.buttons & SCE_CTRL_UP)
			cur_res=(cur_res+1)%ARRAYSIZE(res_table);
		if(ctrl_press.buttons & SCE_CTRL_DOWN)
			cur_cam=(cur_cam+1)%2;
		if(ctrl_press.buttons & SCE_CTRL_LEFT)
			cur_fps=(cur_fps+1)%ARRAYSIZE(fps_table);
		//may overflow the drawbuffer => TODO:reallocate drawbuff on format change
		//if(ctrl_press.buttons & SCE_CTRL_RIGHT)
		//	cur_fmt=(cur_fmt+1)%9;

	}while(!(ctrl_press.buttons & SCE_CTRL_START));
	sceKernelFreeMemBlock(memblock);
	return 0;
}
