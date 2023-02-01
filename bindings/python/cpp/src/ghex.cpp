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
#include <sstream>
#include <utility>

#include <mpi.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ghex/bindings/python/types/buffer_info.hpp"
#include "ghex/bindings/python/types/communication_handle.hpp"
#include "ghex/bindings/python/types/communication_object.hpp"
#include "ghex/bindings/python/types/pattern.hpp"
#include "ghex/bindings/python/types/common/coordinate.hpp"
#include "ghex/bindings/python/types/structured/regular/domain_descriptor.hpp"
#include "ghex/bindings/python/types/structured/regular/field_descriptor.hpp"
#include "ghex/bindings/python/types/structured/regular/halo_generator.hpp"
#include "ghex/bindings/python/types/transport_layer/communicator.hpp"
#include "ghex/bindings/python/types/transport_layer/context.hpp"
#include "ghex/bindings/python/utils/mpi_comm_shim.hpp"

namespace ghex4py = gridtools::ghex::bindings::python;

namespace py = pybind11;

PYBIND11_MODULE(ghex_py_bindings, m) {
    m.doc() = "pybind11 ghex bindings"; // optional module docstring

    m.def_submodule("utils")
        .def("hash_str", [] (const std::string& val) { return std::hash<std::string>()(val); })
        .def("mpi_library_version", [] () {
            int resultlen;
            char version[MPI_MAX_LIBRARY_VERSION_STRING];
            MPI_Get_library_version(version, &resultlen);
            return std::string(version);
        });

    pybind11::class_<mpi_comm_shim> mpi_comm(m, "mpi_comm");
    mpi_comm.def(pybind11::init<>());

    ghex4py::types::export_buffer_info(m);
    ghex4py::types::export_communication_handle(m);
    ghex4py::types::export_communication_object(m);
    ghex4py::types::export_pattern_container(m);
    ghex4py::types::common::export_coordinate(m);
    ghex4py::types::structured::regular::export_domain_descriptor(m);
    ghex4py::types::structured::regular::export_field_descriptor(m);
    ghex4py::types::structured::regular::export_halo_generator(m);
    ghex4py::types::transport_layer::export_communicator(m);
    ghex4py::types::transport_layer::export_context(m);
}