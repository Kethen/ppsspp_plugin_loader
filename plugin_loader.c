#include <pspmodulemgr.h>

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "disc_ident.h"
#include "logging.h"

void load_and_start_plugins(){
	char disc_id[128];
	char disc_version[128];
	int disc_ident_status = get_disc_id_version(disc_id, disc_version);

	if(disc_ident_status != 0){
		LOG("failed fetching disc id version\n");
		return;
	}

	char plugin_dir_path[512] = {0};
	sprintf(plugin_dir_path, "ms0:/ppsspp_plugin_loader/%s", disc_id);

	DIR *plugin_dir = opendir(plugin_dir_path);
	if(plugin_dir == NULL){
		LOG("cannot open %s\n", plugin_dir_path);
		return;
	}

	struct dirent *entry = NULL;
 	while((entry = readdir(plugin_dir)) != NULL){
		if(strcmp("..", entry->d_name) == 0){
			continue;
		}
		char plugin_path[512] = {0};
		sprintf(plugin_path, "%s/%s", plugin_dir_path, entry->d_name);
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

	closedir(plugin_dir);
}
