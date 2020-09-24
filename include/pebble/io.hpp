/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include <pebble/types.hpp>

/** MMIO */
__ALWAYS_INLINE__
static inline uint8
inb(mword port) {
    uint8 ret = *reinterpret_cast<volatile uint8*>(port);
    return ret;
}

__ALWAYS_INLINE__
static inline uint16
inw(mword port) {
    uint16 ret = *reinterpret_cast<volatile uint16*>(port);
    return ret;
}

__ALWAYS_INLINE__
static inline uint32
ind(mword port) {
    uint32 ret = *reinterpret_cast<volatile uint32*>(port);
    return ret;
}

__ALWAYS_INLINE__
static inline void
outb(mword port, uint8 val) {
    *reinterpret_cast<volatile uint8*>(port) = val;
}

__ALWAYS_INLINE__
static inline void
outw(mword port, uint16 val) {
    *reinterpret_cast<volatile uint16*>(port) = val;
}

__ALWAYS_INLINE__
static inline void
outd(mword port, uint32 val) {
    *reinterpret_cast<volatile uint32*>(port) = val;
}
