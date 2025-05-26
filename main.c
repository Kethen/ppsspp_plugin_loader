#include <pspthreadman.h>

#include <string.h>

#include "common.h"
#include "disc_ident.h"
#include "logging.h"
#include "plugin_loader.h"
#include "system_control.h"

PSP_MODULE_INFO(MODULE_NAME, PSP_MODULE_KERNEL, 1, 0);

int dump_module_thread_func(SceSize args, void *argp){
	sceKernelDelayThread(3000000);
	dump_module_info();
	return 0;
}

int module_start(SceSize args, void *argp){
	init_logging();
	LOG("module started\n");
	load_and_start_plugins();
	//XXX sceKernelStartThread seems a bit racy
	sceKernelDelayThread(10000);
	run_handler();

	#if 0
	SceUID thid = sceKernelCreateThread("dump_module_thread", dump_module_thread_func, 0x18, 0x10000, 0, NULL);
	if(thid < 0){
		LOG("failed creating module info dumping thread");
	}
	sceKernelStartThread(thid, 0, NULL);
	#endif

	return 0;
}

int module_stop(SceSize args, void *argp){
	LOG("attempting to stop this module, but unload is not really implemented...");
	return 0;
}
