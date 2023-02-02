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
#ifndef INCLUDED_GHEX_PYBIND_BUFFER_INFO_HPP
#define INCLUDED_GHEX_PYBIND_BUFFER_INFO_HPP

#include <pybind11/pybind11.h>

#include "ghex/buffer_info.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {

template <typename architecture_type, typename data_type, typename layout_map_type>
class buffer_info_exporter {
    private:
        using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
            architecture_type, data_type, layout_map_type>;

        using buffer_info_type = gridtools::ghex::buffer_info<
            typename derived_type_list::inner::pattern_container_type::value_type,
            architecture_type,
            typename derived_type_list::field_descriptor_type>;

    public:
        void operator() (py::module_& m) {
            auto buffer_info_name = gridtools::ghex::bindings::python::utils::demangle<
                buffer_info_type>();

            py::class_<buffer_info_type>(m, buffer_info_name.c_str())
                .def_property_readonly_static("__cpp_type__", [buffer_info_name] (const pybind11::object&) {
                    return buffer_info_name;
                });
        }
};

void export_buffer_info (py::module_& m);

}
}
}
}
}
#endif