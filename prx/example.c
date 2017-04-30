#include <psp2/types.h>
#include <psp2/kernel/threadmgr.h>

int mainloop(SceSize args, void *argp) {
    for(;;){
        // do stuff here
    }
}

int _start(SceSize args, void *argp) {
    SceUID thread_id = sceKernelCreateThread("thread", mainloop, 0x40, 0x40000, 0, 0, NULL);
    if (thread_id >= 0)
        sceKernelStartThread(thread_id, 0, NULL);
    return 0;
}
