# Makefile for GNU make
HDRLOC_NOARCH += drivers/urandom

ifeq ($(__ENABLE_URANDOM__), _YES_)
   CSRC_NOARCH   += drivers/urandom/noarch/urandom.c
   CXXSRC_NOARCH +=
endif
