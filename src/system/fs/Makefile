# Makefile for GNU make
ifeq ($(__DEVFS_ENABLE__), 1)
include $(FS_LOC)/devfs/Makefile
endif

ifeq ($(__FATFS_ENABLE__), 1)
include $(FS_LOC)/fatfs/Makefile
endif

ifeq ($(__LFS_ENABLE__), 1)
include $(FS_LOC)/lfs/Makefile
endif

ifeq ($(__PROCFS_ENABLE__), 1)
include $(FS_LOC)/procfs/Makefile
endif
