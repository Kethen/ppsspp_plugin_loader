TARGET = ppsspp_plugin_loader
OBJS = main.o logging.o disc_ident.o exports.o system_control.o plugin_loader.o

CFLAGS = $(CFLAGS_EXTRA) -O2 -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -fno-builtin -DDEBUG_LOG=1
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

# use kernel "libc"
USE_KERNEL_LIBS = 1

#LIBS =

PSP_FW_VERSION=661

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
