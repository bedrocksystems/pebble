/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <pebble/types.hpp>
#include <pebble/msc.hpp>

typedef void (*pt_entry)(mword arg, mword mtd, void *stack);
#define PT_ENTRY(_fname_) _nova_pt_##_fname_
#define PBL_PORTAL(_name_, _arg_, _mtd_, _stack_) extern "C" mword _name_(_arg_, _mtd_, _stack_)
#include <pebble/pebble_arch.hpp>

namespace Pbl {
    extern "C" mword __pebble_heap_start;
    static inline mword heap_start() { return reinterpret_cast<mword>(&__pebble_heap_start); }

    static inline Errno create_local_ec(Utcb* utcb, Sel sel, Cpu cpu, mword utcb_va, mword sp_va, Sel evt_base) {
        Ec_features features = Ec_features::FPU;
        return Pbl::API::ec_create(utcb, Ec_type::LOCAL, features, sel, self_pd(), cpu, utcb_va, sp_va, evt_base);
    }

    static inline Sel sels_base() {
        extern Conf conf;
        return conf.sel_min;
    }
}
