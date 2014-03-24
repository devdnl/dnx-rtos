# Makefile for GNU make
ifeq ($(ENABLE_DEVFS), YES)
include $(FS_LOC)/devfs/Makefile
endif

ifeq ($(ENABLE_FATFS), YES)
include $(FS_LOC)/fatfs/Makefile
endif

ifeq ($(ENABLE_LFS), YES)
include $(FS_LOC)/lfs/Makefile
endif

ifeq ($(ENABLE_PROCFS), YES)
include $(FS_LOC)/procfs/Makefile
endif
