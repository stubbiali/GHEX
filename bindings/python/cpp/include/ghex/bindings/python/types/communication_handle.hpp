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
#ifndef INCLUDED_GHEX_PYBIND_COMMUNICATION_HANDLE_HPP
#define INCLUDED_GHEX_PYBIND_COMMUNICATION_HANDLE_HPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ghex/buffer_info.hpp"
#include "ghex/communication_object_2.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {

template <typename dim_type>
class communication_handle_exporter {
    private:
        using type_list = gridtools::ghex::bindings::python::type_list;
        using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<dim_type>;

        using communication_handle_type = gridtools::ghex::communication_handle<
            typename derived_type_list::communicator_type,
            typename derived_type_list::grid_impl_type,
            typename type_list::domain_id_type>;

    public:
        void operator() (py::module_& m) {
            auto communication_handle_name = gridtools::ghex::bindings::python::utils::demangle<
                communication_handle_type>();

            py::class_<communication_handle_type>(m, communication_handle_name.c_str())
                .def_property_readonly_static(
                    "__cpp_type__",
                    [communication_handle_name] (const pybind11::object&) {
                        return communication_handle_name;
                    }
                )
                .def("wait", &communication_handle_type::wait);
        }
};

void export_communication_handle (py::module_& m);

}
}
}
}
}
#endif