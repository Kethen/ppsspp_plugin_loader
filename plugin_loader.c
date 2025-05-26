#include <pspmodulemgr.h>
#include <psploadcore.h>
#include <pspthreadman.h>

#include <stdio.h>
#include <string.h>

#include "disc_ident.h"
#include "logging.h"
#include "system_control.h"

void load_and_start_plugins(){
	char disc_id[128];
	char disc_version[128];
	int disc_ident_status = get_disc_id_version(disc_id, disc_version);

	if(disc_ident_status != 0){
		LOG("failed fetching disc id version\n");
		return;
	}

	char plugin_dir_path[512] = {0};
	sprintf(plugin_dir_path, "ms0:/psp/ppsspp_plugin_loader/%s", disc_id);

	SceUID plugin_dir = sceIoDopen(plugin_dir_path);
	if(plugin_dir < 0){
		LOG("cannot open directory %s, 0x%x\n", plugin_dir_path, plugin_dir);
		return;
	}

	LOG("scanning %s for plugins\n", plugin_dir_path);

	while(1){
		struct SceIoDirent entry = {0};
		int directory_read_status = sceIoDread(plugin_dir, &entry);
		if(directory_read_status == 0){
			break;
		}
		if(directory_read_status < 0){
			LOG("sceIoDread on %s failed, 0x%x\n", plugin_dir_path, directory_read_status);
			break;
		}

		if(strcmp("..", entry.d_name) == 0){
			continue;
		}
		if(strcmp(".", entry.d_name) == 0){
			continue;
		}

		char plugin_path[512] = {0};
		sprintf(plugin_path, "%s/%s", plugin_dir_path, entry.d_name);
		LOG("loading and starting %s\n", plugin_path);
		SceUID plugin_load_status = sceKernelLoadModule(plugin_path, 0, NULL);
		if(plugin_load_status < 0){
			LOG("failed loading %s, 0x%x\n", plugin_path, plugin_load_status);
			continue;
		}
		int start_status = 0;
		int plugin_start_status = sceKernelStartModule(plugin_load_status, 0, NULL, &start_status, NULL);
		if(plugin_start_status < 0){
			LOG("failed starting %s, 0x%x\n", plugin_path, plugin_start_status);
		}
	}

	int directory_close_status = sceIoDclose(plugin_dir);
	if(directory_close_status < 0){
		LOG("failed closing directory %s, 0x%x\n", plugin_dir_path, directory_close_status);
	}
}

void dump_module_info(){
	SceUID module_ids[128] = {0};
	int num_module_ids = 0;

	LOG("dumping module info\n");

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

		char name_buf[28] = {0};
		memcpy(name_buf, module_info.name, 27);

		LOG("module #%d %d:\n"
			"SceKernelModuleInfo:\n"
			" size :%d\n"
			" nsegment: %d\n"
			" reserved: 0x%x 0x%x 0x%x\n"
			" segmentaddr: 0x%x 0x%x 0x%x 0x%x\n"
			" segmentsize: 0x%x 0x%x 0x%x 0x%x\n"
			" entry_addr: 0x%x\n"
			" gp_value: 0x%x\n"
			" text_addr: 0x%x\n"
			" text_size: 0x%x\n"
			" data_size: 0x%x\n"
			" bss_size: 0x%x\n"
			" attribute: 0x%x\n"
			" version: 0x%x 0x%x\n"
			" name: %s\n",
			i, module_ids[i],
			module_info.size,
			module_info.nsegment,
			module_info.reserved[0], module_info.reserved[1], module_info.reserved[2],
			module_info.segmentaddr[0], module_info.segmentaddr[1], module_info.segmentaddr[2], module_info.segmentaddr[3],
			module_info.segmentsize[0], module_info.segmentsize[1], module_info.segmentsize[2], module_info.segmentsize[3],
			module_info.entry_addr,
			module_info.gp_value,
			module_info.text_addr,
			module_info.text_size,
			module_info.data_size,
			module_info.bss_size,
			module_info.attribute,
			module_info.version[0], module_info.version[1],
			name_buf
		);

		#if 1
		SceModule2 *module = (SceModule2 *)sceKernelFindModuleByUID(module_ids[i]);
		if(module == NULL){
			LOG("cannot fetch module for %d %s\n", module_ids[i], name_buf);
		}else{
			memcpy(name_buf, module->modname, 27);
			LOG(
				"SceModule2:\n"
				" attribute: 0x%x\n"
				" version: 0x%x 0x%x\n"
				" modname: %s\n"
				" terminal: 0x%x\n"
				" unknown1: 0x%x\n"
				" unknown2: 0x%x\n"
				" modid: %d\n"
				" unknown3: 0x%x 0x%x\n"
				" mpid_text: 0x%x\n"
				" mpid_data: 0x%x\n"
				" ent_top: 0x%x\n"
				" ent_size: 0x%x\n"
				" stub_top: 0x%x\n"
				" stub_size: 0x%x\n"
				" unknown4: 0x%x 0x%x 0x%x 0x%x 0x%x\n"
				" entry_addr: 0x%x\n"
				" gp_value: 0x%x\n"
				" text_addr: 0x%x\n"
				" text_size: 0x%x\n"
				" data_size: 0x%x\n"
				" bss_size: 0x%x\n"
				" nsegment: %d\n"
				" segmentaddr: 0x%x 0x%x 0x%x 0x%x\n"
				" segmentsize: 0x%x 0x%x 0x%x 0x%x\n",
				module->attribute,
				module->version[0], module->version[1],
				name_buf,
				module->terminal,
				module->unknown1,
				module->unknown2,
				module->modid,
				module->unknown3[0], module->unknown3[1],
				(unsigned int)module->mpid_text,
				(unsigned int)module->mpid_data,
				(unsigned int)module->ent_top,
				module->ent_size,
				(unsigned int)module->stub_top,
				module->stub_size,
				module->unknown4[0], module->unknown4[1], module->unknown4[2], module->unknown4[3], module->unknown4[4],
				module->entry_addr,
				module->gp_value,
				module->text_addr,
				module->text_size,
				module->data_size,
				module->bss_size,
				module->nsegment,
				module->segmentaddr[0], module->segmentaddr[1], module->segmentaddr[2], module->segmentaddr[3],
				module->segmentsize[0], module->segmentsize[1], module->segmentsize[2], module->segmentsize[3]
			);
		}
		#endif
	}
}
