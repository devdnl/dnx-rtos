# Makefile for GNU make
ifeq ($(FSCONFIG__DEVFS_ENABLE), ENABLE)
include $(FS_LOC)/devfs/Makefile
endif

ifeq ($(FSCONFIG__FATFS_ENABLE), ENABLE)
include $(FS_LOC)/fatfs/Makefile
endif

ifeq ($(FSCONFIG__LFS_ENABLE), ENABLE)
include $(FS_LOC)/lfs/Makefile
endif

ifeq ($(FSCONFIG__PROCFS_ENABLE), ENABLE)
include $(FS_LOC)/procfs/Makefile
endif
