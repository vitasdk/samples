#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "gxm_base.h"
#include <psp2/gxm.h>
#include <psp2/display.h>
#include <psp2/message_dialog.h>

#define zero_memory(v) memset(&v, 0, sizeof(v))
#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

typedef struct{
	void*data;
	SceGxmSyncObject*sync;
	SceGxmColorSurface surf;
	SceUID uid;
}displayBuffer;


typedef struct MessageBoxButton
{
	char* text;
	int buttonID;
} MessageBoxButton;

typedef struct MessageBoxData
{
	char* title;
	char* message;
	unsigned char numbuttons;
	MessageBoxButton buttons[3];
} MessageBoxData;

bool VITA_ShowMessageBox(const MessageBoxData *messageboxdata, int *buttonID);


int main(int argc, const char *argv[]) {
	MessageBoxData msgBox;

	msgBox.title = "Box displays 'OK' as an option when you use numbuttons == 1";
	msgBox.message = "We are testing a really big message. Please you should try testing a bigger text here.";
	msgBox.numbuttons = 1;
	msgBox.buttons[0].text = "Custom Button 1";
	msgBox.buttons[0].buttonID = 0;
	msgBox.buttons[1].text = "Custom Button 2";
	msgBox.buttons[1].buttonID = 1;
	msgBox.buttons[2].text = "Custom Button 3";
	msgBox.buttons[2].buttonID = 2;
	int selectedButton;

	for(int i = 1; i < 4; i++)
	{
		msgBox.numbuttons = i;
		if(i == 2)
			msgBox.title = "Box displays Yes and No whenever you use numbuttons == 2";
		if(i == 3)
			msgBox.title = "Box displays custom messages whenever you use numbuttons == 3";

		VITA_ShowMessageBox(&msgBox, &selectedButton);

	}
	//here
	return 0;
}



bool VITA_ShowMessageBox(const MessageBoxData *messageboxdata, int *buttonID)
{
    SceMsgDialogParam param;
    SceMsgDialogUserMessageParam msgParam;
    SceMsgDialogButtonsParam buttonParam;
    SceDisplayFrameBuf dispparam;
    char message[512];

    SceMsgDialogResult dialog_result;
    SceCommonDialogErrorCode init_result;
    bool setup_minimal_gxm = false;

    if (messageboxdata->numbuttons > 3) {
        return false;
    }

    zero_memory(param);

    sceMsgDialogParamInit(&param);
    param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;

    zero_memory(msgParam);

    snprintf(message, sizeof(message), "%s\r\n\r\n%s", messageboxdata->title, messageboxdata->message);

    msgParam.msg = (const SceChar8 *)message;
    zero_memory(buttonParam);

    if (messageboxdata->numbuttons == 3) {
        msgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_3BUTTONS;
        msgParam.buttonParam = &buttonParam;
        buttonParam.msg1 = messageboxdata->buttons[0].text;
        buttonParam.msg2 = messageboxdata->buttons[1].text;
        buttonParam.msg3 = messageboxdata->buttons[2].text;
    } else if (messageboxdata->numbuttons == 2) {
        msgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_YESNO;
    } else if (messageboxdata->numbuttons == 1) {
        msgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_OK;
    }
    param.userMsgParam = &msgParam;

    dispparam.size = sizeof(dispparam);

    init_result = sceMsgDialogInit(&param);

    // Setup display if it hasn't been initialized before
    if (init_result == SCE_COMMON_DIALOG_ERROR_GXM_IS_UNINITIALIZED) {
        gxm_minimal_init_for_common_dialog();
        init_result = sceMsgDialogInit(&param);
        setup_minimal_gxm = true;
    }

    gxm_init_for_common_dialog();

    if (init_result >= 0) {
        while (sceMsgDialogGetStatus() == SCE_COMMON_DIALOG_STATUS_RUNNING) {
            gxm_swap_for_common_dialog();
        }
        zero_memory(dialog_result);
        sceMsgDialogGetResult(&dialog_result);

        if (dialog_result.buttonId == SCE_MSG_DIALOG_BUTTON_ID_BUTTON1) {
            *buttonID = messageboxdata->buttons[0].buttonID;
        } else if (dialog_result.buttonId == SCE_MSG_DIALOG_BUTTON_ID_BUTTON2) {
            *buttonID = messageboxdata->buttons[1].buttonID;
        } else if (dialog_result.buttonId == SCE_MSG_DIALOG_BUTTON_ID_BUTTON3) {
            *buttonID = messageboxdata->buttons[2].buttonID;
        } else if (dialog_result.buttonId == SCE_MSG_DIALOG_BUTTON_ID_YES) {
            *buttonID = messageboxdata->buttons[0].buttonID;
        } else if (dialog_result.buttonId == SCE_MSG_DIALOG_BUTTON_ID_NO) {
            *buttonID = messageboxdata->buttons[1].buttonID;
        } else if (dialog_result.buttonId == SCE_MSG_DIALOG_BUTTON_ID_OK) {
            *buttonID = messageboxdata->buttons[0].buttonID;
        }
        sceMsgDialogTerm();
    } else {
        return false;
    }

    gxm_term_for_common_dialog();

    if (setup_minimal_gxm) {
        gxm_minimal_term_for_common_dialog();
    }

    return true;
}