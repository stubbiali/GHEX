# -*- coding: utf-8 -*-
#
# GridTools
#
# Copyright (c) 2014-2021, ETH Zurich
# All rights reserved.
#
# Please, refer to the LICENSE file in the root directory.
# SPDX-License-Identifier: BSD-3-Clause
import os
import sys
import warnings

try:
    import mpi4py
except ImportError:
    mpi4py = None


def update_path():
    ghex_py_lib_path = os.environ.get("GHEX_PY_LIB_PATH", None)

    if ghex_py_lib_path is None:
        current_dir = os.path.dirname(os.path.relpath(__file__))
        project_root = os.path.abspath(os.path.join(current_dir, "../../../../.."))
        ghex_py_lib_path = os.path.join(project_root, "build")
    if not os.path.exists(ghex_py_lib_path):
        raise ValueError(
            f"The build directory `{ghex_py_lib_path}` does not exist.\n"
            f"Please set the correct build directory via the env variable GHEX_PY_LIB_PATH."
        )

    sys.path.append(ghex_py_lib_path)


def validate_library_version(_ghex):
    """Check MPI library version string of MPI4Py and bindings match."""
    if mpi4py is None:
        return

    from mpi4py import MPI

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
