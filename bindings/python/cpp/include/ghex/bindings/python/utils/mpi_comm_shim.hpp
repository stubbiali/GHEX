/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#pragma once

#include <mpi.h>

// https://github.com/arbor-sim/arbor/commit/1d6a48d0ce4b96f59acf931efd61d55c571c5e68
// A shim is required for MPI_Comm, because OpenMPI defines it as a pointer to
// a forward-declared type, which pybind11 won't allow as an argument.
// MPICH and its derivatives use an integer.
struct mpi_comm_shim {
    MPI_Comm comm = MPI_COMM_WORLD;

    mpi_comm_shim() = default;
    mpi_comm_shim(MPI_Comm c): comm(c) {}
};