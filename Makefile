#
# Makefile for misc devices that really don't fit anywhere else.
#

obj-y += jd9161_480p_dsi_vdo.o


ccflags-$(CONFIG_MTK_LCM) += -I$(srctree)/drivers/misc/mediatek/lcm/inc

