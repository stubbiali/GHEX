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

#include <memory>

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
class communication_object_exporter {
    private:
        using type_list = gridtools::ghex::bindings::python::type_list;
        using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<dim_type>;

    public:
        using communication_object_type = gridtools::ghex::communication_object<
            typename derived_type_list::communicator_type,
            typename derived_type_list::grid_impl_type,
            typename type_list::domain_id_type>;
        using exported_type = py::class_<communication_object_type>;

    private:
        static bool initialized;
        static std::unique_ptr<exported_type> exported_obj;

        void set_exported_obj (py::module_& m) {
            auto communication_object_name = gridtools::ghex::bindings::python::utils::demangle<
                communication_object_type>();
            auto exported_obj = exported_type(m, communication_object_name.c_str())
                .def_property_readonly_static("__cpp_type__", [communication_object_name] (const pybind11::object&) {
                    return communication_object_name;
                })
                .def(py::init<typename derived_type_list::communicator_type>());
            communication_object_exporter<dim_type>::exported_obj = std::make_unique<exported_type>(exported_obj);
        }

    public:
        exported_type operator() (py::module_& m) {
            if (!communication_object_exporter<dim_type>::initialized) {
                set_exported_obj(m);
                communication_object_exporter<dim_type>::initialized = true;
            }
            return *communication_object_exporter<dim_type>::exported_obj;
        }
};

template <typename architecture_type, typename data_type, typename layout_map_type_>
class communication_object_enricher {
    public:
        using layout_map_type = layout_map_type_;

    private:
        using type_list = gridtools::ghex::bindings::python::type_list;
        using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
            architecture_type, data_type, layout_map_type>;

        using dim_type = gridtools::ghex::bindings::python::dim_type<layout_map_type>;

        using buffer_info_type = gridtools::ghex::buffer_info<
            typename derived_type_list::inner::pattern_container_type::value_type,
            architecture_type,
            typename derived_type_list::field_descriptor_type>;

        using exporter_type = communication_object_exporter<
            gridtools::ghex::bindings::python::template dim_type<layout_map_type>>;
        using communication_object_type = typename exporter_type::communication_object_type;
        using exported_type = typename exporter_type::exported_type;

    public:
        void operator() (exported_type& exported_obj) {
            exported_obj.def("exchange", [] (communication_object_type& co,
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

void export_communication_object (py::module_& m);

}
}
}
}
}
#endif