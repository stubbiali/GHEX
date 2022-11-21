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
#include <utility>
#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ghex/structured/pattern.hpp>

#include "ghex/bindings/type_list.hpp"

namespace py = pybind11;


PYBIND11_MODULE(ghex, m) {
    using domain_descriptor_type = gridtools::ghex::structured::regular::domain_descriptor<
        typename gridtools::ghex::bindings::python::type_list::domain_id_type,
        typename gridtools::ghex::bindings::python::type_list::dim>;

    using pattern_container_type = decltype(gridtools::ghex::make_pattern<
        typename gridtools::ghex::bindings::python::type_list::grid_type>(
            std::declval<typename gridtools::ghex::bindings::python::type_list::context_type&>(),
            std::declval<typename gridtools::ghex::bindings::python::type_list::halo_generator_type&>(),
            std::declval<typename gridtools::ghex::bindings::python::type_list::domain_range_type<domain_descriptor_type>&>()
        )
    );

    m.class_<pattern_container_type>(m, pattern_container_type.c_str())
        .def_property_readonly_static("grid_type", [] (const pybind11::object&) {
            return pattern_container_type::grid_type;
        });
        .def_property_readonly_static("domain_id_type", [] (const pybind11::object&) {
            return pattern_container_type::domain_id_type;
        });
//        std::cout << "PatternContainer: " << gridtools::ghex::bindings::python::utils::demangle<pattern_container_type>() << std::endl;
        auto make_pattern_wrapper = [] (
                typename gridtools::ghex::bindings::python::type_list::context_type& context,
                typename gridtools::ghex::bindings::python::type_list::halo_generator_type& hgen,
                typename gridtools::ghex::bindings::python::type_list::domain_range_type& d_range) {
            return gridtools::ghex::make_pattern<typename gridtools::ghex::bindings::python::type_list::grid_type>(context, hgen, d_range);
        };
        // TODO: specialize name
        m.def("make_pattern", make_pattern_wrapper);

    }
}