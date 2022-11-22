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
#ifndef INCLUDED_GHEX_PYBIND_PATTERN_HPP
#define INCLUDED_GHEX_PYBIND_PATTERN_HPP

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ghex/structured/pattern.hpp"
#include "ghex/structured/regular/domain_descriptor.hpp"
#include "ghex/structured/regular/field_descriptor.hpp"
#include "ghex/structured/regular/halo_generator.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {

void pattern_container_exporter(py::module_& m) {
    using architecture_type = typename gridtools::ghex::bindings::python::type_list::architecture_type;
    using context_type = typename gridtools::ghex::bindings::python::type_list::context_type;
    using data_type = typename gridtools::ghex::bindings::python::type_list::data_type;
    using dim_type = typename gridtools::ghex::bindings::python::type_list::dim_type;
    using domain_id_type = typename gridtools::ghex::bindings::python::type_list::domain_id_type;
    using layout_map_type = typename gridtools::ghex::bindings::python::type_list::layout_map_type;

    using domain_descriptor_type = gridtools::ghex::structured::regular::domain_descriptor<
        domain_id_type, dim_type>;
    using domain_range_type = typename gridtools::ghex::bindings::python::type_list::domain_range_type<domain_descriptor_type>;
    using field_descriptor_type = gridtools::ghex::structured::regular::field_descriptor<
        data_type, architecture_type, domain_descriptor_type, layout_map_type>;
    using halo_generator_type = gridtools::ghex::structured::regular::halo_generator<domain_id_type, dim_type>;
    using pattern_container_type = decltype(gridtools::ghex::make_pattern<
        gridtools::ghex::bindings::python::type_list::grid_type>(
            std::declval<context_type&>(),
            std::declval<halo_generator_type&>(),
            std::declval<domain_range_type&>()
        )
    );
    auto pattern_container_name = gridtools::ghex::bindings::python::utils::demangle<pattern_container_type>();

    py::class_<pattern_container_type>(m, pattern_container_name.c_str())
        .def_property_readonly_static("__cpp_type__", [pattern_container_name] (const pybind11::object&) {
            return pattern_container_name;
        })
        .def_property_readonly_static("domain_id_type", [] (const pybind11::object&) {
            return gridtools::ghex::bindings::python::utils::demangle<typename pattern_container_type::domain_id_type>();
        })
        .def_property_readonly_static("grid_type", [] (const pybind11::object&) {
            return gridtools::ghex::bindings::python::utils::demangle<typename pattern_container_type::grid_type>();
        })
        .def("__call__", &pattern_container_type::template operator()<field_descriptor_type>);

    auto make_pattern_wrapper = [] (context_type& context, halo_generator_type& hgen, domain_range_type& d_range) {
        return gridtools::ghex::make_pattern<gridtools::ghex::bindings::python::type_list::grid_type>(context, hgen, d_range);
    };
    m.def("make_pattern", make_pattern_wrapper);
}

}
}
}
}
}
#endif