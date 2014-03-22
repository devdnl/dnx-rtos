include ./config/fs.config
include ./config/arch.config
#include ./config/toolchain.config

#ifeq ($(ARCHCONFIG__TARGET), stm32f1)
#include ./config/stm32f1/cpu.config
#endif

CONFIG_DEF += -D__DEVFS_ENABLE__=$(FSCONFIG__DEVFS_ENABLE)
CONFIG_DEF += -D__LFS_ENABLE__=$(FSCONFIG__LFS_ENABLE)
CONFIG_DEF += -D__FATFS_ENABLE__=$(FSCONFIG__FATFS_ENABLE)
CONFIG_DEF += -D__PROCFS_ENABLE__=$(FSCONFIG__PROCFS_ENABLE)
#CONFIG_DEF += -DARCH_$(ARCHCONFIG__TARGET)
