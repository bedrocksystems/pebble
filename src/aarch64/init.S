/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
        .section .init
        .global __pebble_start

__pebble_start:
        // First EC in HP
        cmp     x2, #1
        b.ne   1f

        ldr     x2, =__stack_base
        mov     sp, x2

        // save UTCB and Cpu on top of the stack
        sub     sp, sp, #16
        stp     x0, x1, [sp] // save UTCB* and Cpu in stack

        // extern "C" void __main(Nova::Utcb *utcb, Cpu cpu)
        // x0 and x1 are already set with the expected value.
        bl      __main

        // reply
        mov     x1, 0x0
        mov     x0, 0x1
        svc     0

1:
        // secondary ECs in HP

        // call reply
        mov     x1, 0x0
        mov     x0, 0x1
        svc     0

