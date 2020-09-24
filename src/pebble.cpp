/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <pebble/pebble.hpp>

extern void pbl_main(Pbl::Utcb *utcb, Cpu cpu);

namespace Pbl {
    Pbl::Conf conf;

    static void init(Pbl::Utcb *utcb, Cpu) { Pbl::API::init(utcb, conf); }
};

extern "C" void init_ctors();

// Startup Functions
// this is the actual entry point
extern "C" void
__main(Pbl::Utcb *utcb, Cpu cpu) {
    init(utcb, cpu);

    init_ctors();

    pbl_main(utcb, cpu);
}
