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

#include "boost/mp11/algorithm.hpp"
#include "boost/mp11/bind.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {


void export_pattern_container (py::module_& m) {
    using type_list = gridtools::ghex::bindings::python::type_list;
    using generic_derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
        boost::mp11::_1, boost::mp11::_2, boost::mp11::_3>;

    using pattern_container_type = typename generic_derived_type_list::pattern_container_type;
    auto pattern_container_name = gridtools::ghex::bindings::python::utils::demangle<
        pattern_container_type>();

    // export methods which are independent of arch, data type and layout
    auto cls = py::class_<pattern_container_type>(m, pattern_container_name.c_str())
        .def_property_readonly_static("__cpp_type__", [pattern_container_name]
            (const pybind11::object&) { return pattern_container_name;
        })
        .def_property_readonly_static("domain_id_type", [] (const pybind11::object&) {
            return gridtools::ghex::bindings::python::utils::demangle<
                typename pattern_container_type::domain_id_type>();
        })
        .def_property_readonly_static("grid_type", [] (const pybind11::object&) {
            return gridtools::ghex::bindings::python::utils::demangle<
                typename pattern_container_type::grid_type>();
        });

    // export methods which depend on arch, data type and layout
    using derived_type_list = boost::mp11::mp_product<
        gridtools::ghex::bindings::python::derived_type_list,
        typename type_list::architecture_type,
        typename type_list::data_type,
        typename type_list::layout_map_type>;
    boost::mp11::mp_for_each<derived_type_list>([&](auto t){
        cls.def("__call__", &pattern_container_type::template operator()<
            typename decltype(t)::field_descriptor_type>); });

    auto make_pattern_wrapper = [] (
        typename generic_derived_type_list::context_type& context,
        typename generic_derived_type_list::halo_generator_type& hgen,
        typename generic_derived_type_list::domain_range_type& d_range) {
        return gridtools::ghex::make_pattern<typename type_list::grid_type>(context, hgen, d_range);
    };
    m.def("make_pattern", make_pattern_wrapper);
}

}
}
}
}
}
#endif