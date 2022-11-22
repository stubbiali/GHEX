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

    ghex4py::types::buffer_info_exporter(m);
    ghex4py::types::communication_object_exporter(m);
    ghex4py::types::pattern_container_exporter(m);
    ghex4py::types::common::coordinate_exporter(m);
    ghex4py::types::structured::regular::domain_descriptor_exporter(m);
    ghex4py::types::structured::regular::field_descriptor_exporter(m);
    ghex4py::types::structured::regular::halo_generator_exporter(m);
    ghex4py::types::transport_layer::communicator_exporter(m);
    ghex4py::types::transport_layer::context_exporter(m);
}