# Makefile for GNU make
HDRLOC_NOARCH += drivers/tty

ifeq ($(__ENABLE_TTY__), _YES_)
   CSRC_NOARCH   += drivers/tty/noarch/tty.c
   CSRC_NOARCH   += drivers/tty/noarch/tty_bfr.c
   CSRC_NOARCH   += drivers/tty/noarch/tty_cmd.c
   CSRC_NOARCH   += drivers/tty/noarch/tty_editln.c
   CXXSRC_NOARCH += 
endif
