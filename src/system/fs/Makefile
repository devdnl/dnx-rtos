# Makefile for GNU make
CSRC_CORE   += fs/fsctrl.c
CSRC_CORE   += fs/fs_registration.c
CSRC_CORE   += fs/vfs.c
HDRLOC_CORE += fs

ifeq ($(__ENABLE_DEVFS__), _YES_)
include $(SYS_FS_LOC)/devfs/Makefile
endif

ifeq ($(__ENABLE_FATFS__), _YES_)
include $(SYS_FS_LOC)/fatfs/Makefile
endif

ifeq ($(__ENABLE_LFS__), _YES_)
include $(SYS_FS_LOC)/lfs/Makefile
endif

ifeq ($(__ENABLE_PROCFS__), _YES_)
include $(SYS_FS_LOC)/procfs/Makefile
endif

ifeq ($(__ENABLE_EXT2FS__), _YES_)
include $(SYS_FS_LOC)/ext2fs/Makefile
endif
