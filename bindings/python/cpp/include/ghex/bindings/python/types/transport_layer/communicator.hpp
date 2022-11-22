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
#ifndef INCLUDED_GHEX_PYBIND_COMMUNICATOR_HPP
#define INCLUDED_GHEX_PYBIND_COMMUNICATOR_HPP

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace transport_layer {

void communicator_exporter (py::module_& m) {
    using communicator_type = typename gridtools::ghex::bindings::python::type_list::communicator_type;
    auto communicator_name = gridtools::ghex::bindings::python::utils::demangle<communicator_type>();

    py::class_<communicator_type>(m, communicator_name.c_str());
}

}
}
}
}
}
}
#endif