# SPDX-License-Identifier: GPL-2.0

#
# External kernel makefile for the JHD1313 LCD Display panel.
#
#   make -C sources/linux \
#	O=build/linux \
#	ARCH=arm64 \
#	CROSS_COMPILE=aarch64-linux-gnu- \
#	M=$(PATH_TO_THIS_DIRECTORY) \
#	modules
#

obj-m += jhd1313.o
