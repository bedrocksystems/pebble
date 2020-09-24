/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <pebble/types.hpp>

extern "C" __attribute__((nonnull)) __attribute__((used)) void *
memset(void *d, int c, size_t n) {
    size_t p;
    for (p = 0; p < n; p++) {
        reinterpret_cast<uint8 *>(d)[p] = static_cast<uint8>(c);
    }
    return d;
}

extern "C" __attribute__((nonnull)) __attribute__((used)) void *
memcpy(void *d, void const *s, size_t n) {
    size_t p;
    for (p = 0; p < n; p++) {
        reinterpret_cast<uint8 *>(d)[p] = reinterpret_cast<uint8 const *>(s)[p];
    }
    return d;
}

void
operator delete(void *)noexcept {
    /*XXX: Not implemented */
}

void
operator delete(void *p, size_t) noexcept {
    operator delete(p);
}

extern "C" __attribute__((noreturn)) void
__cxa_pure_virtual() {
    for (;;) {
        __builtin_trap();
    }
}

extern "C" int
__cxa_atexit(void (*)(void *), void *, void *) {
    return 0;
}

extern "C" {
void *__dso_handle = reinterpret_cast<void *>(&__dso_handle);
}

extern "C" void
init_ctors() {
    extern void (*__preinit_array_start[])(void) __attribute__((weak));
    extern void (*__preinit_array_end[])(void) __attribute__((weak));
    extern void (*__init_array_start[])(void) __attribute__((weak));
    extern void (*__init_array_end[])(void) __attribute__((weak));

    size_t count = static_cast<size_t>(__preinit_array_end - __preinit_array_start);
    for (size_t i = 0; i < count; i++) {
        __preinit_array_start[i]();
    }

    count = static_cast<size_t>(__init_array_end - __init_array_start);
    for (size_t i = 0; i < count; i++) {
        __init_array_start[i]();
    }
}
