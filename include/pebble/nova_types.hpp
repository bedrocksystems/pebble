/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

namespace Pbl {
    enum Hypercall : unsigned char {
        IPC_CALL = 0x0,
        IPC_REPLY = 0x1,
        CREATE_PD = 0x2,
        CREATE_EC = 0x3,
        CREATE_SC = 0x4,
        CREATE_PT = 0x5,
        CREATE_SM = 0x6,
        CTRL_PD = 0x7,
        CTRL_EC = 0x8,
        CTRL_SC = 0x9,
        CTRL_PT = 0xa,
        CTRL_SM = 0xb,
        CTRL_HW = 0xc,
        ASSIGN_INT = 0xd,
        ASSIGN_DEV = 0xe,
    };

    enum Nova_status : unsigned char {
        SUCCESS = 0x0,
        TIMEOUT = 0x1,
        ABORTED = 0x2,
        OVRFLOW = 0x3,
        BAD_HYP = 0x4,
        BAD_CAP = 0x5,
        BAD_PAR = 0x6,
        BAD_FTR = 0x7,
        BAD_CPU = 0x8,
        BAD_DEV = 0x9,
        INS_MEM = 0xa,
    };

    enum Ec_type : unsigned int {
        LOCAL = 0,
        GLOBAL = 1,
        VCPU = 2,
    };

    enum Ec_features : unsigned int {
        NONE = 0x0,
        FPU = 0x1,
    };

    enum Sm_ops : unsigned char {
        SM_UP = 0x0,
        SM_DOWN = 0x1,
        SM_DOWN_ZERO = 0x3,
    };
};
