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

template <typename Arch>
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

template <typename Arch>
py::buffer_info get_buffer_info(py::object& buffer) {
    return buffer_info_accessor<Arch>::get(buffer);
}

void field_descriptor_exporter(py::module_& m) {
    using architecture_type = typename gridtools::ghex::bindings::python::type_list::architecture_type;
    using data_type = typename gridtools::ghex::bindings::python::type_list::data_type;
    using dim_type = typename gridtools::ghex::bindings::python::type_list::dim_type;
    using domain_id_type = typename gridtools::ghex::bindings::python::type_list::domain_id_type;
    using layout_map = typename gridtools::ghex::bindings::python::type_list::layout_map;

    using domain_descriptor_type = gridtools::ghex::structured::regular::domain_descriptor<
        domain_id_type, dim_type>;
    using field_descriptor_type = gridtools::ghex::structured::regular::field_descriptor<
        data_type, architecture_type, domain_descriptor_type, layout_map>;
    auto field_descriptor_name = gridtools::ghex::bindings::python::utils::demangle<field_descriptor_type>();
    using array_type = std::array<int, dim_type::value>;

    auto wrapper = [] (const domain_descriptor_type& dom, py::object& b, const array_type& offsets, const array_type& extents) {
        py::buffer_info info = get_buffer_info<architecture_type>(b);

        if (info.format != py::format_descriptor<data_type>::format()) {
            std::stringstream error;
            error << "Incompatible format: expected a " << typeid(data_type).name() << " buffer.";
            throw pybind11::type_error(error.str());
        }
        std::array<int, 3> buffer_order = {0, 1, 2};
        std::sort(buffer_order.begin(), buffer_order.end(), [&info](int a, int b) {
            return info.strides[a] > info.strides[b];
        });
        for (size_t i=0; i<3; ++i) {
            if (buffer_order[i] != layout_map::at(i)) {
                throw pybind11::type_error("Buffer has a different layout than specified.");
            }
        }

        if (info.ndim != 3)
            throw std::runtime_error("Incompatible buffer dimension.");

        return gridtools::ghex::wrap_field<architecture_type, layout_map>(
            dom, static_cast<data_type*>(info.ptr), offsets, extents);
    };

    py::class_<field_descriptor_type>(m, field_descriptor_name.c_str())
        .def(py::init(wrapper));
}

}
}
}
}
}
}
}
#endif