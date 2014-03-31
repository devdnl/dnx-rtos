# Makefile for GNU make
ifeq ($(ENABLE_DEVFS), __YES__)
include $(FS_LOC)/devfs/Makefile
endif

ifeq ($(ENABLE_FATFS), __YES__)
include $(FS_LOC)/fatfs/Makefile
endif

ifeq ($(ENABLE_LFS), __YES__)
include $(FS_LOC)/lfs/Makefile
endif

ifeq ($(ENABLE_PROCFS), __YES__)
include $(FS_LOC)/procfs/Makefile
endif
