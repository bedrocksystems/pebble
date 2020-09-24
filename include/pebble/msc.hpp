/*
 * Copyright (c) 2020 BedRock Systems, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <pebble/errno.hpp>
#include <pebble/nova.hpp>
#include <pebble/types.hpp>

namespace Pbl {
    constexpr static const uint8 MAX_CPU_NUMBER = 64;

    struct Conf {
        mword version;           // Current version of API
        Sel sel_min;             // First capability selector number available to that hyper-process
        Sel sel_max;             // Last capability selector number available to that hyper-process
        Sel exc_base;            // The selector base number of host exception portals
        Sel sel_hst_arch;        // Number of capability selectors reserved for architectural host exceptions
                                 // per cpu
        Sel sel_hst_nova;        // Number of additional capability selectors for host events reserved by NOVA
                                 // per cpu
        Sel sel_gst_arch;        // Number of capability selectors required for architectural guest VM exits
        Sel sel_gst_nova;        // Number of additional capability selectors for guest events required by NOVA
        size_t va_max_bits;      // Max order of user virtual address space
        mword highest_va;        // Highest virtual address we can address
        size_t host_memsz_bytes; // Size of the host physical memory reserved for that hyper-process

        size_t host_ram_bytes;  // Amount of physical memory available to map.
        size_t guest_ram_bytes; // Size of the physical memory reserved for guest as RAM
        size_t guest_rom_bytes; // Size of the physical memory reserved for guest as ROM

        uint64 tfreq; // timer frequency. Used in calculation of timeouts
    };

    static inline Cpu get_current_cpu() {
        /*XXX: Support execution only on CPU0 */
        return 0;
    }

    /* Note: Portals for non-bootstrap CPUs  are initialized by MSC lazily */
    static inline Sel get_msc_pt() {
        Cpu cpu = get_current_cpu();
        return EC_COUNT * cpu + 0x2a;
    }

    static inline Sel self_pd() { return EC_COUNT * MAX_CPU_NUMBER + 0; }
};

namespace Pbl::API {
    enum Msc_method : mword {
        INIT = 0,
        CREATE_EC = 12,
        SERVICE_CREATE = 13,
        ACQUIRE_RES = 17,
        DMA_MMAP = 21,
    };

    struct Init_args {
        Msc_method id{INIT};
    };

    struct Init_ret {
        Errno status;
        Conf conf;
    };

    struct Create_ec_args {
        Msc_method id{CREATE_EC};
        Ec_type type;
        Ec_features features;
        Sel ec;
        Sel own;
        Cpu cpu;
        mword utcb;
        mword sp;
        Sel evt;
    };

    struct Create_ec_ret {
        Errno status;
    };

    struct Srv_create_args {
        Msc_method id{SERVICE_CREATE};
        Uuid uuid;
        Crd crd;
        Sel ec;
        Cpu cpu;
        mword entry;
    };

    struct Srv_create_ret {
        Errno status;
    };

    enum Resource_type {
        RES_REG,
        RES_IRQ,
    };
    const int DEVID_MAX_SIZE = 128;
    struct Acquire_res_args {
        Msc_method id{ACQUIRE_RES};
        Resource_type type;
        uint32 idx;
        Sel sel;
        Cpu cpu;
        bool guest;
        char dev_id[DEVID_MAX_SIZE];
    };
    struct Acquire_res_ret {
        Errno status;
    };

    struct Dma_mmap_args {
        Msc_method id{DMA_MMAP};
        mword va;
        size_t sz;
        uint8 cred;
        bool cached;
    };

    struct Dma_mmap_ret {
        Errno status;
        mword pa;
    };

    static inline Errno init(Utcb *utcb, Conf &conf) {
        Errno err;
        Init_args *args = reinterpret_cast<Init_args *>(utcb->data);
        Init_ret *ret = reinterpret_cast<Init_ret *>(utcb->data);
        Mtd size = (sizeof(*args) + sizeof(mword) - 1) / sizeof(mword);
        mword ret_size = (sizeof(*ret) + sizeof(mword) - 1) / sizeof(mword);

        args->id = INIT;

        err = ipc_call(get_msc_pt(), size);
        if (!err_success(err)) {
            return err;
        }
        if (size < ret_size) {
            return Errno::ENOTSUP;
        }
        if (err_success(ret->status)) {
            conf = ret->conf;
        }
        return ret->status;
    }

    static inline Errno ec_create(Utcb *utcb, Ec_type type, Ec_features features, Sel ec, Sel own, Cpu cpu, mword ec_utcb, mword sp,
                                  Sel evt) {
        Errno err;
        Create_ec_args *args = reinterpret_cast<Create_ec_args *>(utcb->data);
        Create_ec_ret *ret = reinterpret_cast<Create_ec_ret *>(utcb->data);
        Mtd size = (sizeof(*args) + sizeof(mword) - 1) / sizeof(mword);
        mword ret_size = (sizeof(*ret) + sizeof(mword) - 1) / sizeof(mword);

        args->id = CREATE_EC;
        args->type = type;
        args->features = features;
        args->ec = ec;
        args->own = own;
        args->cpu = cpu;
        args->utcb = ec_utcb;
        args->sp = sp;
        args->evt = evt;

        err = ipc_call(get_msc_pt(), size);
        if (!err_success(err)) {
            return err;
        }
        if (size < ret_size) {
            return Errno::ENOTSUP;
        }
        return ret->status;
    }

    static inline Errno srv_create(Utcb *utcb, Sel ec, const Uuid &uuid, Crd crd, Cpu cpu, mword entry) {
        Errno err;
        Srv_create_args *args = reinterpret_cast<Srv_create_args *>(utcb->data);
        Srv_create_ret *ret = reinterpret_cast<Srv_create_ret *>(utcb->data);
        Mtd size = (sizeof(*args) + sizeof(mword) - 1) / sizeof(mword);
        mword ret_size = (sizeof(*ret) + sizeof(mword) - 1) / sizeof(mword);

        args->id = SERVICE_CREATE;
        args->uuid = uuid;
        args->crd = crd;
        args->ec = ec;
        args->cpu = cpu;
        args->entry = entry;

        err = ipc_call(get_msc_pt(), size);
        if (!err_success(err)) {
            return err;
        }
        if (size < ret_size) {
            return Errno::ENOTSUP;
        }
        return ret->status;
    }

    static inline Errno acquire_resource(Utcb *utcb, const char *dev_id, Resource_type type, uint32 idx, Sel sel, Cpu cpu,
                                         bool guest) {
        Errno err;
        Acquire_res_args *args = reinterpret_cast<Acquire_res_args *>(utcb->data);
        Acquire_res_ret *ret = reinterpret_cast<Acquire_res_ret *>(utcb->data);
        Mtd size = (sizeof(*args) + sizeof(mword) - 1) / sizeof(mword);
        mword ret_size = (sizeof(*ret) + sizeof(mword) - 1) / sizeof(mword);

        args->id = ACQUIRE_RES;
        args->type = type;
        args->idx = idx;
        args->sel = sel;
        args->cpu = cpu;
        args->guest = guest;
        /*Copy dev_id */
        int c = 0;
        while ((c < DEVID_MAX_SIZE - 1) && (dev_id[c] != '\0')) {
            args->dev_id[c] = dev_id[c];
            c++;
        };
        args->dev_id[c] = '\0';

        err = ipc_call(get_msc_pt(), size);
        if (!err_success(err)) {
            return err;
        }
        if (size < ret_size) {
            return Errno::ENOTSUP;
        }
        return ret->status;
    }

    static inline Errno dma_mmap(Utcb *utcb, mword va, size_t sz, uint8 cred, bool cached, mword &pa) {
        Errno err;
        Dma_mmap_args *args = reinterpret_cast<Dma_mmap_args *>(utcb->data);
        Dma_mmap_ret *ret = reinterpret_cast<Dma_mmap_ret *>(utcb->data);
        Mtd size = (sizeof(*args) + sizeof(mword) - 1) / sizeof(mword);
        mword ret_size = (sizeof(*ret) + sizeof(mword) - 1) / sizeof(mword);

        args->id = DMA_MMAP;
        args->va = va;
        args->sz = sz;
        args->cred = cred;
        args->cached = cached;

        err = ipc_call(get_msc_pt(), size);
        if (!err_success(err)) {
            return err;
        }
        if (size < ret_size) {
            return Errno::ENOTSUP;
        }
        if (err_success(ret->status)) {
            pa = ret->pa;
        }
        return ret->status;
    }
};
