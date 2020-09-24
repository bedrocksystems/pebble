/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <pebble/nova_types.hpp>

enum Errno : uint8 {
    ENONE = 0,
    ENOMEM = 12,
    EINVAL = 22,
    ENOTSUP = 95,
    ETIMEDOUT = 110,
};

__ALWAYS_INLINE__
static inline bool
err_success(Errno err) {
    return err == Errno::ENONE;
}

static inline Errno
nova_err(Pbl::Nova_status status) {
    switch (status) {
    case Pbl::Nova_status::SUCCESS:
        return Errno::ENONE;
    case Pbl::Nova_status::TIMEOUT:
        return Errno::ETIMEDOUT;
    default:
        return Errno::EINVAL;
    }
}
