#
# Copyright (c) 2020 BedRock Systems, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
ARCH ?= aarch64
TARGET ?= $(ARCH)
LLVM ?= 1
BLDDIR ?= $(CURDIR)/build-$(ARCH)/

SRCDIR		?= src/
INCDIR		?= include/

ifeq ($(LLVM), 1)
	CC  = $(LLVM_BASE_DIR)clang
	CXX = $(LLVM_BASE_DIR)clang++
	LD  = $(LLVM_BASE_DIR)ld.lld
	AR  = $(LLVM_BASE_DIR)llvm-ar
	CPP = $(LLVM_BASE_DIR)clang-cpp
	OBJCOPY = $(LLVM_BASE_DIR)llvm-objcopy
	TARGET_FLAG = --target=$(TARGET)-none-elf
ifeq ($(LTO), 1)
	LTO_FLAG = -flto=thin
endif

else
	CC  = $(CROSS_COMPILE)gcc
	CXX = $(CROSS_COMPILE)g++
	# Link with g++ to allow LTO easily (invoking LD manually would require some plugin config)
	LD  = $(CROSS_COMPILE)g++
	AR  = $(CROSS_COMPILE)gcc-ar
	CPP = $(CROSS_COMPILE)cpp
	OBJCOPY = $(CROSS_COMPILE)objcopy
ifeq ($(LTO), 1)
	LTO_FLAG = -flto
endif

endif

GIT_DIR = $(shell git rev-parse --show-toplevel 2>/dev/null)/
GIT_REV	= $(shell git rev-parse --short HEAD 2>/dev/null)

MFLAGS = -MP -MMD -pipe
CXXVERSION ?= -std=gnu++17
FFLAGS = -funit-at-a-time \
	-fdata-sections -ffunction-sections -fomit-frame-pointer -fno-asynchronous-unwind-tables \
	-fno-stack-protector -fvisibility=hidden -fvisibility-inlines-hidden -foptimize-sibling-calls \
	-fmerge-all-constants -fno-builtin -ffreestanding

ifeq ($(LLVM), 0)
#Fixes memset optimization as a call to memset (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56888)
FFLAGS += -fno-tree-loop-distribute-patterns
FFLAGS += -freorder-blocks -freg-struct-return
endif

FFLAGS += $(CXXVERSION)
DFLAGS ?=
WFLAGS = -Wall -Wextra -Wcast-align -Wcast-qual -Wconversion \
	-Wdisabled-optimization -Wformat=2 -Wmissing-format-attribute \
	-Wmissing-noreturn -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wsign-promo -Wstrict-overflow=5 \
	-Wwrite-strings -Wzero-as-null-pointer-constant
XFLAGS = -fno-exceptions -fno-rtti -fno-threadsafe-statics
# Linker flags
# -nostartfiles not needed with -nostdlib
# --no-relax architecture dependent
COMMON_LFLAGS = -nostdlib -static
NAKED_LFLAGS = --gc-sections --whole-archive  --build-id=none -z max-page-size=0x1000 $(LFLAGS_$(TARGET))

ifeq ($(ARCH), aarch64)
AFLAGS ?= -march=armv8-a -mgeneral-regs-only -mstrict-align
endif

ifeq ($(DEBUG), 1)
OFLAGS = -g
else
OFLAGS = -O2
endif

RPATH_TO_CURDIR=$(realpath $(CURDIR))
find_common_path=$(patsubst $(1)%,%, $(2))
COMMON_PATH_WITH_PBL=$(patsubst %lib/pebble,%, $(realpath $(PBL_ROOT)))
# If no common path, pick the name of the lib or app (only one variable will be defined)
common_path_or_name=$(if $(findstring $(1),$(2)),$(call find_common_path,$(1),$(2)),$(APPNAME)$(LIBNAME))

TARGET_PATH=$(call common_path_or_name,$(COMMON_PATH_WITH_PBL),$(RPATH_TO_CURDIR))

BLDDIR_TARGET = $(BLDDIR)$(TARGET_PATH)/

OBJDIR = $(BLDDIR_TARGET)

LINK_SCRIPT ?= $(PBL_ROOT)/src/$(ARCH)/pebble.lds

find_path_to_lib = $(foreach d, $(LIBDIR), $(wildcard $(d)$(1)))
real_path_to_lib=$(realpath $(call find_path_to_lib,$(1)))
common_path_or_libname=$(if $(findstring $(1),$(2)),$(call find_common_path,$(1),$(2)),$(3))
find_common_path_to_lib=$(call common_path_or_libname,$(COMMON_PATH_WITH_PBL),$(call real_path_to_lib,$(1)),$(1))
find_path_to_lib_objs=$(BLDDIR)$(call find_common_path_to_lib,$(1))
find_path_to_archive=$(call find_path_to_lib_objs,$(1))/lib$(1).a

INCLS  = $(foreach l, $(LIBS), $(addsuffix /include,$(call find_path_to_lib,$l)))
INCLS += $(foreach l, $(LIBS), $(addsuffix /include/$(ARCH),$(call find_path_to_lib,$l)))

APPINCL = $(INCDIR) $(INCDIR)$(ARCH)
IFLAGS = $(addprefix -I, $(APPINCL)) $(addprefix -I, $(INCLS))
LINKDEPS = $(foreach l,$(LINKLIBS),$(call find_path_to_archive,$l))
EXTRA_LINK =

SFLAGS	?= $(TARGET_FLAG) $(MFLAGS) $(AFLAGS) $(HFLAGS) $(DFLAGS) $(addprefix -I, $(INCLS))
CXXFLAGS ?= $(TARGET_FLAG) $(MFLAGS) $(AFLAGS) $(OFLAGS) $(FFLAGS) $(XFLAGS) $(IFLAGS) $(DFLAGS) $(WFLAGS)

ifeq ($(LLVM), 1)
COMPILER_SPECIFIC_LFLAGS = $(NAKED_LFLAGS)
else
# we link using gcc so we need to explicitely pass some args to the linker
COMPILER_SPECIFIC_LFLAGS = $(CXXFLAGS) $(addprefix -Xlinker , $(NAKED_LFLAGS))
endif
LFLAGS	:= $(COMMON_LFLAGS) $(COMPILER_SPECIFIC_LFLAGS)

DEPS = $(patsubst %.o,%.d, $(OBJS))

BUILD_CONF_DEPS = $(wildcard $(PBL_ROOT)/support/build/* $(RPATH_TO_CURDIR)/Makefile $(RPATH_TO_CURDIR)/deps.mk)

$(OBJDIR)%.o : $(SRCDIR)$(ARCH)/%.S $(BUILD_CONF_DEPS)
	@- mkdir -p $(@D) 2>&1 > /dev/null
	@echo "[CC] $(subst $(GIT_DIR),,$(abspath $@))"
	$(CXX) $(SFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o : $(SRCDIR)$(ARCH)/%.cpp $(BUILD_CONF_DEPS)
	@- mkdir -p $(@D) 2>&1 > /dev/null
	@echo "[CC] $(subst $(GIT_DIR),,$(abspath $@))"
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o : $(SRCDIR)%.cpp $(BUILD_CONF_DEPS)
	@- mkdir -p $(@D) 2>&1 > /dev/null
	@echo "[CC] $(subst $(GIT_DIR),,$(abspath $@))"
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ifdef LIBNAME
all: $(OBJDIR)lib$(LIBNAME).a

$(OBJDIR)lib$(LIBNAME).a: $(OBJS)
	$(AR) rcs $@ $^

else ifdef APPNAME

all: $(OBJDIR)$(APPNAME)

LINK_SCRIPT_POST = $(OBJDIR)$(notdir $(LINK_SCRIPT))

$(LINK_SCRIPT_POST): $(LINK_SCRIPT) $(BUILD_CONF_DEPS)
	@- mkdir -p $(@D) 2>&1 > /dev/null
	@echo "[CPP] $(subst $(GIT_DIR),,$(abspath $@))"
	$(CPP) -I. -I$(TOPDIR)include $(SFLAGS) -MT $@ -P $< -o $@

$(OBJDIR)$(APPNAME) : $(LINK_SCRIPT_POST) $(OBJS) $(LINKDEPS)
	@echo "[LD] $(subst $(GIT_DIR),,$(abspath $@))"
	$(LD) $(LFLAGS) -T $(LINK_SCRIPT_POST) -o $@ $(OBJS) \
		$(addprefix -L,$(dir $(LINKDEPS))) $(patsubst %,-l%,$(LINKLIBS)) \
		$(EXTRA_LINK)

else

$(error "Not sure what to build...")

endif

clean:
	rm -rf $(BLDDIR)

default: all
.PHONY: default all clean

-include $(DEPS)
