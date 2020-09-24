/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#define __STRARGS__(x...) #x
#define __QUOTES__(x) __STRARGS__(x)

// clang-format off
#define EXPORT_PORTAL(_fn_, _arg_type_)                      \
__asm__(".globl " __QUOTES__(PT_ENTRY(_fn_)) "\n"        \
        ".extern " #_fn_ "\n"               \
        __QUOTES__(PT_ENTRY(_fn_)) ":\n"                 \
        "mov x19, sp \n"                                            \
        "mov x2, x19 \n"                                            \
        "bl " #_fn_ "\n"                    \
        "mov sp, x19 \n"                                            \
        "mov x1, x0 \n"                                             \
        "mov x0, 1 ; svc 0 \n");                                    \
    extern "C" void PT_ENTRY(_fn_)(_arg_type_, mword)

// clang-format on
