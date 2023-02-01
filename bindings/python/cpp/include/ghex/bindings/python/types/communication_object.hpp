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
#ifndef INCLUDED_GHEX_PYBIND_COMMUNICATION_OBJECT_HPP
#define INCLUDED_GHEX_PYBIND_COMMUNICATION_OBJECT_HPP

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "boost/mp11/algorithm.hpp"

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

template <typename architecture_type, typename data_type, typename layout_map_type>
struct communication_object_exporter {
    using type_list = gridtools::ghex::bindings::python::type_list;
    using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
        architecture_type, data_type, layout_map_type>;

    using buffer_info_type = gridtools::ghex::buffer_info<
        typename derived_type_list::pattern_container_type::value_type,
        architecture_type,
        typename derived_type_list::field_descriptor_type>;
    using communication_object_type = gridtools::ghex::communication_object<
        typename derived_type_list::communicator_type,
        typename derived_type_list::grid_impl_type,
        typename type_list::domain_id_type>;

    void operator() (py::class_<communication_object_type>& cls) {
        cls.def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b) {
                return co.exchange(b);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2) {
                return co.exchange(b1, b2);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3) {
                return co.exchange(b1, b2, b3);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4) {
                return co.exchange(b1, b2, b3, b4);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5) {
                return co.exchange(b1, b2, b3, b4, b5);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6) {
                return co.exchange(b1, b2, b3, b4, b5, b6);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7,
                                 buffer_info_type& b8) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7, b8);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7,
                                 buffer_info_type& b8,
                                 buffer_info_type& b9) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7, b8, b9);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7,
                                 buffer_info_type& b8,
                                 buffer_info_type& b9,
                                 buffer_info_type& b10) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7, b8, b9, b10);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7,
                                 buffer_info_type& b8,
                                 buffer_info_type& b9,
                                 buffer_info_type& b10,
                                 buffer_info_type& b11) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7,
                                 buffer_info_type& b8,
                                 buffer_info_type& b9,
                                 buffer_info_type& b10,
                                 buffer_info_type& b11,
                                 buffer_info_type& b12) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12);
            })
            .def("exchange", [] (communication_object_type& co,
                                 buffer_info_type& b1,
                                 buffer_info_type& b2,
                                 buffer_info_type& b3,
                                 buffer_info_type& b4,
                                 buffer_info_type& b5,
                                 buffer_info_type& b6,
                                 buffer_info_type& b7,
                                 buffer_info_type& b8,
                                 buffer_info_type& b9,
                                 buffer_info_type& b10,
                                 buffer_info_type& b11,
                                 buffer_info_type& b12,
                                 buffer_info_type& b13) {
                return co.exchange(b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13);
            });
    }
};

void export_communication_object (py::module_& m) {
    // get the exported class
    using generic_exporter = communication_object_exporter<
        boost::mp11::_1, boost::mp11::_2, boost::mp11::_3>;
    auto communication_object_name = gridtools::ghex::bindings::python::utils::demangle<
        typename generic_exporter::communication_object_type>();

    auto cls = py::class_<typename generic_exporter::communication_object_type>(
        m, communication_object_name.c_str())
        .def_property_readonly_static("__cpp_type__", [communication_object_name] (const pybind11::object&) {
            return communication_object_name;
        })
        .def(py::init<typename generic_exporter::derived_type_list::communicator_type>());

    // enrich the class with methods to exchange
    using exporter_list = boost::mp11::mp_product<
        communication_object_exporter,
        typename gridtools::ghex::bindings::python::type_list::architecture_type,
        typename gridtools::ghex::bindings::python::type_list::data_type,
        typename gridtools::ghex::bindings::python::type_list::layout_map_type>;
    boost::mp11::mp_for_each<exporter_list>([&](auto exporter){ exporter(cls); });
}

}
}
}
}
}
#endif