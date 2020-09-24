/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long long uint64;
typedef signed long long int64;
typedef unsigned long size_t;
typedef unsigned long mword;

typedef mword Sel;
typedef mword Cpu;
typedef mword Crd;
typedef mword Mtd;

struct Uuid {
    uint8 bytes[16];
};

static const uint16 PAGE_SIZE = 4096;

#define __ALWAYS_INLINE__ __attribute__((always_inline))
