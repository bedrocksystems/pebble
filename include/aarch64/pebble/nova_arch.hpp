/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include <pebble/errno.hpp>
#include <pebble/types.hpp>

namespace Pbl {
    struct Utcb {
        mword data[PAGE_SIZE / sizeof(mword)];
    };

    constexpr static const uint8 PARENT_PORTAL_IDX = 0x2a;
    constexpr static const uint8 EC_COUNT = 0x42; /* nova arch selectors + 2 for MSC usage */

    inline Errno ipc_call(Sel pt, Mtd &mtd, bool disable_blocking = false) {
        Errno errno;
        register mword x0 __asm__("x0") = mword(pt << 8 | (disable_blocking ? 1 : 0) | Hypercall::IPC_CALL);
        register mword x1 __asm__("x1") = mtd;
        __asm__ volatile("svc 0" : "+r"(x0), "+r"(x1) : : "memory");
        uint8 err = static_cast<uint8>(x0);
        if (err == 0) {
            mtd = x1;
            errno = Errno::ENONE;
        } else {
            errno = nova_err(static_cast<Nova_status>(x0));
        }
        return errno;
    }

    inline Errno ctrl_sm(Sel sm, Sm_ops op, uint64 timeout_tsc = 0ULL) {
        Errno err;
        register mword x0 __asm__("x0") = mword((sm << 8) | ((mword(op) & 0xf) << 4) | Hypercall::CTRL_SM);
        register mword x1 __asm__("x1") = timeout_tsc;

        __asm__ volatile("svc 0" : "+r"(x0) : "r"(x1) : "memory");
        err = nova_err(static_cast<Nova_status>(x0));
        return err;
    }
};
