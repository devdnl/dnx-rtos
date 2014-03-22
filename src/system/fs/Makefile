# Makefile for GNU make
ifeq ($(FSCONFIG__DEVFS_ENABLE), 1)
include $(FS_LOC)/devfs/Makefile
endif

ifeq ($(FSCONFIG__FATFS_ENABLE), 1)
include $(FS_LOC)/fatfs/Makefile
endif

ifeq ($(FSCONFIG__LFS_ENABLE), 1)
include $(FS_LOC)/lfs/Makefile
endif

ifeq ($(FSCONFIG__PROCFS_ENABLE), 1)
include $(FS_LOC)/procfs/Makefile
endif
