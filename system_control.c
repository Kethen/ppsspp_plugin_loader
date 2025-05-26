#include <pspmodulemgr.h>

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
		
		SceModule2 module_info_2 = {0};
		module_info_2.attribute = module_info.attribute;
		memcpy(module_info_2.version, module_info.version, sizeof(char) * 2);
		memcpy(module_info_2.modname, module_info.name, sizeof(char) * 27);
		module_info_2.modid = module_ids[i];
		module_info_2.entry_addr = module_info.entry_addr;
		module_info_2.gp_value = module_info.gp_value;
		module_info_2.text_addr = module_info.text_addr;
		module_info_2.text_size = module_info.text_size;
		module_info_2.data_size = module_info.data_size;
		module_info_2.bss_size = module_info.bss_size;
		module_info_2.nsegment = module_info.nsegment;
		memcpy(module_info_2.segmentaddr, module_info.segmentaddr, sizeof(int) * 4);
		memcpy(module_info_2.segmentsize, module_info.segmentsize, sizeof(int) * 4);

		// XXX hack, ppsspp's current text_addr seems off
		module_info_2.text_addr -= 0x28;

		if(module_start_handler != NULL){
			LOG("calling module_start_handler for %s\n", module_info_2.modname);
			module_start_handler(&module_info_2);
		}else{
			LOG("module_start_handler is null, %s\n", module_info_2.modname);
		}
	}
}
