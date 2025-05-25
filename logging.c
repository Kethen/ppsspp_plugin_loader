#include "common.h"
#include "logging.h"

#if DEBUG_LOG
int logfd = -1;

void init_logging(){
	sceIoRemove("ms0:/PSP/"MODULE_NAME".log");
	sceIoRemove("ef0:/PSP/"MODULE_NAME".log");	
}
#endif
