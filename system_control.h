#ifndef __SYSTEM_CONTROL_H
#define __SYSTEM_CONTROL_H
// definition obtained from https://github.com/MrColdbird/procfw/blob/master/include/systemctrl.h
typedef struct SceModule2 {
    struct SceModule2   *next;
    unsigned short      attribute;
    unsigned char       version[2];
    char                modname[27];
    char                terminal;
    unsigned int        unknown1;
    unsigned int        unknown2;
    SceUID              modid;
    unsigned int        unknown3[2];
    u32         mpid_text;  // 0x38
    u32         mpid_data; // 0x3C
    void *              ent_top;
    unsigned int        ent_size;
    void *              stub_top;
    unsigned int        stub_size;
    unsigned int        unknown4[5];
    unsigned int        entry_addr;
    unsigned int        gp_value;
    unsigned int        text_addr;
    unsigned int        text_size;
    unsigned int        data_size;
    unsigned int        bss_size;
    unsigned int        nsegment;
    unsigned int        segmentaddr[4];
    unsigned int        segmentsize[4];
} SceModule2;

void run_handler();
#endif
