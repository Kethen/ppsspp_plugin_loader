#include <pspmodulemgr.h>
#include <psploadcore.h>

#include <string.h>

#include "logging.h"
#include "system_control.h"

typedef int (* STMOD_HANDLER)(SceModule2 *);

STMOD_HANDLER module_start_handler = NULL;

STMOD_HANDLER sctrlHENSetStartModuleHandler(STMOD_HANDLER new_handler){
	STMOD_HANDLER old_handler = module_start_handler;
	module_start_handler = new_handler;
	LOG("sctrlHENSetStartModuleHandler from 0x%p, 0x%p, 0x%p\n", __builtin_return_address(0), new_handler, old_handler);
	return old_handler;
}

void run_handler(){
	SceUID module_ids[128] = {0};
	int num_module_ids = 0;

	int module_id_list_get_status = sceKernelGetModuleIdList(module_ids, sizeof(module_ids), &num_module_ids);
	if(sceKernelGetModuleIdList < 0){
		LOG("failed fetching module id list, 0x%x\n", module_id_list_get_status);
		return;
	}

	for(int i = 0;i < num_module_ids;i++){
		SceKernelModuleInfo module_info = {0};
		module_info.size = sizeof(SceKernelModuleInfo);
		int module_info_get_status = sceKernelQueryModuleInfo(module_ids[i], &module_info);
		if(module_info_get_status != 0){
			LOG("failed fetching module info for module %d, 0x%x\n", module_ids[i], module_info_get_status);
			continue;
		}

		SceModule2 *module = (SceModule2 *) sceKernelFindModuleByUID(module_ids[i]);
		if(module == NULL){
			LOG("failed fetching module for module %d\n", module_ids[i]);
			continue;
		}

		char module_name_buffer[28] = {0};
		memcpy(module_name_buffer, module_info.name, 27);

		if(module_start_handler != NULL){
			LOG("calling module_start_handler for %s\n", module_name_buffer);
			module_start_handler(module);
		}else{
			LOG("module_start_handler is null, %s\n", module_name_buffer);
		}
	}
}
