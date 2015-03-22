# Makefile for GNU make
ifeq ($(__ENABLE_DEVFS__), _YES_)
include $(FS_LOC)/devfs/Makefile
endif

ifeq ($(__ENABLE_FATFS__), _YES_)
include $(FS_LOC)/fatfs/Makefile
endif

ifeq ($(__ENABLE_LFS__), _YES_)
include $(FS_LOC)/lfs/Makefile
endif

ifeq ($(__ENABLE_PROCFS__), _YES_)
include $(FS_LOC)/procfs/Makefile
endif

ifeq ($(__ENABLE_EXT2FS__), _YES_)
include $(FS_LOC)/ext2fs/Makefile
endif
