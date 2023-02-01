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

#include "boost/mp11/bind.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace transport_layer {

void export_communicator (py::module_& m) {
    using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
        boost::mp11::_1, boost::mp11::_2, boost::mp11::_3>;
    using communicator_type = typename derived_type_list::communicator_type;
    auto communicator_name = gridtools::ghex::bindings::python::utils::demangle<communicator_type>();

    py::class_<communicator_type>(m, communicator_name.c_str())
        .def_property_readonly_static("__cpp_type__", [communicator_name] (const pybind11::object&) {
            return communicator_name;
        });
}

}
}
}
}
}
}
#endif