/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include <pebble/errno.hpp>
#include <pebble/nova_arch.hpp>
#include <pebble/nova_types.hpp>
#include <pebble/types.hpp>

namespace Pbl {
    static inline Errno sm_up(Sel sm_sel) { return ctrl_sm(sm_sel, SM_UP, 0); }

    static inline Errno sm_down(Sel sm_sel, uint64 timeout_tsc, bool zero) {
        return ctrl_sm(sm_sel, zero ? SM_DOWN_ZERO : SM_DOWN, timeout_tsc);
    }
};
