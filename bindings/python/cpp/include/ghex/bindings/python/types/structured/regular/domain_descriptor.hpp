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
#ifndef INCLUDED_GHEX_PYBIND_DOMAIN_DESCRIPTOR_HPP
#define INCLUDED_GHEX_PYBIND_DOMAIN_DESCRIPTOR_HPP

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
namespace structured {
namespace regular {

template <std::size_t... I, typename U>
constexpr auto as_tuple(const U &arr, std::index_sequence<I...>) {
    return std::make_tuple(arr[I]...);
}

template <typename T, std::size_t N>
constexpr auto as_tuple(const std::array<T, N> &arr) {
    return as_tuple(arr, std::make_index_sequence<N>{});
}

void export_domain_descriptor (py::module_& m) {
    using type_list = gridtools::ghex::bindings::python::type_list;
    using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
        boost::mp11::_1, boost::mp11::_2, boost::mp11::_3>;

    using domain_descriptor_type = typename derived_type_list::domain_descriptor_type;
    auto domain_descriptor_name = gridtools::ghex::bindings::python::utils::demangle<
        domain_descriptor_type>();

    using dim_array_t = std::array<int, type_list::dim_type::value>;

    py::class_<domain_descriptor_type>(m, domain_descriptor_name.c_str())
        .def_property_readonly_static("__cpp_type__", [domain_descriptor_name] (const pybind11::object&) {
            return domain_descriptor_name;
        })
        .def(py::init<typename type_list::domain_id_type, dim_array_t, dim_array_t>())
        .def("domain_id", &domain_descriptor_type::domain_id)
        .def("first", [] (const domain_descriptor_type& domain_desc) { return as_tuple(domain_desc.first()); })
        .def("last", [] (const domain_descriptor_type& domain_desc) { return as_tuple(domain_desc.last()); });
}

}
}
}
}
}
}
}

#endif