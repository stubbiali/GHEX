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
#ifndef INCLUDED_GHEX_PYBIND_FIELD_DESCRIPTOR_HPP
#define INCLUDED_GHEX_PYBIND_FIELD_DESCRIPTOR_HPP

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "boost/mp11/algorithm.hpp"

#include "gridtools/meta.hpp"

#include "ghex/arch_list.hpp"
#include "ghex/structured/regular/domain_descriptor.hpp"
#include "ghex/structured/regular/field_descriptor.hpp"

#include "ghex/bindings/python/type_list.hpp"
#include "ghex/bindings/python/utils/demangle.hpp"

namespace py = pybind11;

using namespace pybind11::literals;

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace structured {
namespace regular {

template <int... val>
using int_tuple_constant = gridtools::meta::list<std::integral_constant<int, val>...>;

template <typename architecture_type>
struct buffer_info_accessor {};

template <>
struct buffer_info_accessor<gridtools::ghex::cpu> {
    static py::buffer_info get(py::object& buffer) {
        return buffer.cast<py::buffer>().request();
    }
};

#ifdef __CUDACC__
template <>
struct buffer_info_accessor<gridtools::ghex::gpu> {
    static py::buffer_info get(py::object& buffer) {
        py::dict info = buffer.attr("__cuda_array_interface__");

        bool readonly = info["data"].cast<py::tuple>()[1].cast<bool>();
        assert(!readonly);

        void* ptr = reinterpret_cast<void*>(info["data"].cast<py::tuple>()[0].cast<py::ssize_t>());

        // create buffer protocol format and itemsize from typestr
        py::function memory_view = py::module::import("builtins").attr("memoryview");
        py::function np_array = py::module::import("numpy").attr("array");
        py::buffer empty_buffer = memory_view(np_array(py::list(), "dtype"_a=info["typestr"]));
        py::ssize_t itemsize = empty_buffer.request().itemsize;
        std::string format = empty_buffer.request().format;

        std::vector<py::ssize_t> shape = info["shape"].cast<std::vector<py::ssize_t>>();
        py::ssize_t ndim = shape.size();

        std::vector<py::ssize_t> strides(ndim);
        if (py::isinstance<py::none>(info["strides"])) {
            strides[ndim-1] = 1;
            for (int i=ndim-2; i>=0; --i) {
                strides[i] = (strides[i+1]*shape[i+1]) * itemsize;
            }
        } else {
            strides = info["strides"].cast<std::vector<py::ssize_t>>();
            assert(py::ssize_t(strides.size()) == ndim);
        }

        return py::buffer_info(
            ptr,        /* Pointer to buffer */
            itemsize,   /* Size of one scalar */
            format,     /* Python struct-style format descriptor */
            ndim,       /* Number of dimensions */
            shape,      /* Buffer dimensions */
            strides     /* Strides (in bytes) for each index */
        );
    }
};
#endif

template <typename architecture_type>
py::buffer_info get_buffer_info(py::object& buffer) {
    return buffer_info_accessor<architecture_type>::get(buffer);
}

template <typename architecture_type, typename data_type, typename layout_map_type>
struct field_descriptor_exporter {
    using type_list = gridtools::ghex::bindings::python::type_list;
    using derived_type_list = gridtools::ghex::bindings::python::derived_type_list<
        architecture_type, data_type, layout_map_type>;

    using field_descriptor_type = typename derived_type_list::field_descriptor_type;

    void operator() (py::module_& m) {
        using array_type = std::array<int, type_list::dim_type::value>;
        using domain_descriptor_type = typename derived_type_list::domain_descriptor_type;

        auto wrapper = [] (const domain_descriptor_type& dom, py::object& b, const array_type& offsets, const array_type& extents) {
            py::buffer_info info = get_buffer_info<architecture_type>(b);

            if (info.format != py::format_descriptor<data_type>::format()) {
                std::stringstream error;
                error << "Incompatible format: expected a " << typeid(data_type).name() << " buffer.";
                throw pybind11::type_error(error.str());
            }

            auto ordered_strides = info.strides;
            std::sort(ordered_strides.begin(), ordered_strides.end(), [](int a, int b) { return a > b; });
            std::array<int, 3> layout_map;
            for (size_t i=0; i<3; ++i) {
                auto it = std::find(ordered_strides.begin(), ordered_strides.end(), info.strides[i]);
                layout_map[i] = std::distance(ordered_strides.begin(), it);
                if (layout_map[i] != layout_map_type::at(i)) {
                    throw pybind11::type_error("Buffer has a different layout than specified.");
                }
            }

            if (info.ndim != 3)
                throw std::runtime_error("Incompatible buffer dimension.");

            return gridtools::ghex::wrap_field<architecture_type, layout_map_type>(
                dom, static_cast<data_type*>(info.ptr), offsets, extents, info.strides);
        };

        auto field_descriptor_name = gridtools::ghex::bindings::python::utils::demangle<
            field_descriptor_type>();

        py::class_<field_descriptor_type>(m, field_descriptor_name.c_str())
            .def_property_readonly_static("__cpp_type__", [field_descriptor_name] (const pybind11::object&) {
                return field_descriptor_name;
            })
            .def(py::init(wrapper));
    }
};

void export_field_descriptor (py::module_& m) {
    using exporter_list = boost::mp11::mp_product<
        field_descriptor_exporter,
        typename gridtools::ghex::bindings::python::type_list::architecture_type,
        typename gridtools::ghex::bindings::python::type_list::data_type,
        typename gridtools::ghex::bindings::python::type_list::layout_map_type>;
    boost::mp11::mp_for_each<exporter_list>([&](auto exporter){ exporter(m); });
}

}
}
}
}
}
}
}
#endif