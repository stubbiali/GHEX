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

#include <memory>

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

template <typename dim_type>
class pattern_container_exporter {
    private:
        using type_list = gridtools::ghex::bindings::python::type_list;
        using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
            dim_type>;

        using pattern_container_type = typename derived_type_list::pattern_container_type;

    public:
        using exported_type = py::class_<pattern_container_type>;

    private:
        static bool initialized;
        static std::unique_ptr<exported_type> exported_obj;

        void set_exported_obj (py::module_& m) {
            auto pattern_container_name = gridtools::ghex::bindings::python::utils::demangle<
                pattern_container_type>();
            auto exported_obj = exported_type(m, pattern_container_name.c_str())
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
            pattern_container_exporter<dim_type>::exported_obj = std::make_unique<exported_type>(exported_obj);
        }

    public:
        exported_type operator() (py::module_& m) {
            if (!pattern_container_exporter<dim_type>::initialized) {
                set_exported_obj(m);

                auto make_pattern_wrapper = [] (
                    typename derived_type_list::context_type& context,
                    typename derived_type_list::halo_generator_type& hgen,
                    typename derived_type_list::domain_range_type& d_range) {
                    return gridtools::ghex::make_pattern<typename type_list::grid_type>(context, hgen, d_range); };
                m.def("make_pattern", make_pattern_wrapper);

                pattern_container_exporter<dim_type>::initialized = true;
            }
            return *pattern_container_exporter<dim_type>::exported_obj;
        }
};

template <typename architecture_type, typename data_type, typename layout_map_type_>
class pattern_container_enricher {
    public:
        using layout_map_type = layout_map_type_;

    private:
        using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
            architecture_type, data_type, layout_map_type>;

        using pattern_container_type = typename derived_type_list::inner::pattern_container_type;
        using field_descriptor_type = typename derived_type_list::field_descriptor_type;

        using exporter_type = pattern_container_exporter<
            gridtools::ghex::bindings::python::template dim_type<layout_map_type>>;
        using exported_type = typename exporter_type::exported_type;

    public:
        void operator() (exported_type& exported_obj) {
            exported_obj.def("__call__", &pattern_container_type::template operator()<
                field_descriptor_type>);
        }
};

void export_pattern_container (py::module_& m);

}
}
}
}
}
#endif