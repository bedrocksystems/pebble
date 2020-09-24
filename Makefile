#
# Copyright (c) 2020 BedRock Systems, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
export BLDDIR ?= $(CURDIR)/build-$(BOARD)/
export TARGET ?= $(ARCH)
export PBL_ROOT=$(realpath .)/
export LIB_ROOT=$(realpath ../)

CMDGOAL = $(if $(strip $(MAKECMDGOALS)),$(MAKECMDGOALS),all)

LIBNAME = $(shell basename `pwd`)

OBJDIR ?= $(BLDDIR)/

CC_SRCS = pebble.cpp compiler.cpp
S_SRCS = init.S
OBJS    = $(CC_SRCS:%.cpp=$(OBJDIR)%.o) $(S_SRCS:%.S=$(OBJDIR)%.o)

include $(CURDIR)/support/build/rules.mk
