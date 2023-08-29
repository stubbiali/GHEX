# -*- coding: utf-8 -*-
#
# GridTools
#
# Copyright (c) 2014-2021, ETH Zurich
# All rights reserved.
#
# Please, refer to the LICENSE file in the root directory.
# SPDX-License-Identifier: BSD-3-Clause
from mpi4py import MPI
import warnings


def validate_library_version(_ghex):
    """Check MPI library version string of MPI4Py and bindings match."""
    ghex_mpi_lib_ver = _ghex.utils.mpi_library_version()
    mpi4py_lib_ver = MPI.Get_library_version()
    # fix erroneous nullbyte at the end
    if mpi4py_lib_ver[-1] == "\x00":
        mpi4py_lib_ver = mpi4py_lib_ver[:-1]
    if ghex_mpi_lib_ver != mpi4py_lib_ver:
        warnings.warn(
            f"GHEX and MPI4Py were compiled using different MPI versions.\n"
            f" GHEX:   {ghex_mpi_lib_ver}\n"
            f" MPI4Py: {mpi4py_lib_ver}."
        )
