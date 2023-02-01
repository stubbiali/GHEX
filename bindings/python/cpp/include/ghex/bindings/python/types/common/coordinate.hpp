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
#ifndef INCLUDED_GHEX_PYBIND_COORDINATE_HPP
#define INCLUDED_GHEX_PYBIND_COORDINATE_HPP

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ghex/common/coordinate.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace common {

void export_coordinate (py::module_& m) {
    using coordinate_type = gridtools::ghex::coordinate<
        std::array<
            typename gridtools::ghex::bindings::python::type_list::domain_id_type,
            gridtools::ghex::bindings::python::type_list::dim_type::value
        >
    >;
    auto coordinate_name = gridtools::ghex::bindings::python::utils::demangle<coordinate_type>();

    py::class_<coordinate_type>(m, coordinate_name.c_str())
        .def_property_readonly_static("__cpp_type__", [coordinate_name] (const pybind11::object&) {
            return coordinate_name;
        });
}

}
}
}
}
}
}
#endif