/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.hpp"

// ELF Segment Attributes (p_flags)
#define LDS_PF_X 0x1
#define LDS_PF_W 0x2
#define LDS_PF_R 0x4

// PF_MASKOS value
#define LDS_NULL (0x10 << 16)
#define LDS_LOAD (0x11 << 16)
#define LDS_HEAP (0x13 << 16)

#define LDS_PF_MASKOS   0x00ff0000
#define LDS_PF_MASKPROC 0xff000000

// Size for global EC virtual address space
#define EC_VA_BITS 21

/* Define default values for config */
#ifndef PBL_STACK_SIZE
#define PBL_STACK_SIZE 4K
#endif

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-bigaarch64", "elf64-littleaarch64")
OUTPUT_ARCH("aarch64")

ENTRY(__pebble_start);

/*
 * Define the order and permission for all sections
 */
PHDRS {
  text PT_LOAD FILEHDR PHDRS FLAGS(LDS_PF_R | LDS_PF_X);
  data PT_LOAD FLAGS(LDS_PF_R | LDS_PF_W);

  ztnl PT_NULL FLAGS(LDS_NULL | LDS_PF_R);
  ztld PT_LOAD FLAGS(LDS_LOAD | LDS_PF_R | LDS_PF_W);

  bss PT_LOAD  FLAGS(LDS_PF_R | LDS_PF_W);
  heap PT_NULL FLAGS(LDS_HEAP | LDS_PF_R | LDS_PF_W);
}

SECTIONS
{
     . = 0x1000000000 + SIZEOF_HEADERS;

    .text : {
        *(.init)
        *(.text .text.* .gnu.linkonce.t.*)
        *(.fini)
        *(.got.plt)
    } : text = 0xd503201f

    . = ALIGN(0x8);
    .preinit_array : {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } : text = 0xd503201f

    . = ALIGN(0x8);
    .init_array : {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
    } : text = 0xd503201f

    . = ALIGN(0x8);
    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array))
        PROVIDE_HIDDEN (__init_array_end = .);
    } : text = 0xd503201f

    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(4K); /* Everything in .text PAGE_ALIGN */
    } : text = 0x0

    .data : ALIGN(4K) {
        *(.data .data.* .gnu.linkonce.d.*)
    } : data

    .zeta : ALIGN(1 << EC_VA_BITS) {
        PROVIDE(__main_utcb = .);
        . += 8K;   // 4K for the UTCB + 4K for protection
        PROVIDE(__ZIP = .); /* Zeta Information Page */
        . += 8K;
    } : ztnl

    // At the end of the EC_VA_SIZE
    . = ALIGN(1 << EC_VA_BITS) - PBL_STACK_SIZE - 4K;

    .stack : ALIGN(4K) {
        . += PBL_STACK_SIZE;
        PROVIDE(__stack_base = .);
        . += 4K;
    } : ztld

    .bss : ALIGN(4K) {
        *(.bss .bss.* .gnu.linkonce.b.* COMMON)
    } : bss

    . += 4K;

    // Must be at the end with one virtual page to get a base address.
    .heap : ALIGN(4K) {
        PROVIDE(__pebble_heap_start = .);
        . += PBL_HEAP_SIZE;
    } : ztld

    /DISCARD/ : {
        *(.note)
        *(.note.GNU-stack)
        *(.note.ABI-tag)
        *(.comment*)
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.gcc_except_table.*)
        *(.dynamic)
        *(.note.gnu.*)
    }
}
