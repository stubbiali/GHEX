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
#include "boost/mp11/algorithm.hpp"

#include "gridtools/common/defs.hpp"

#include "ghex/bindings/python/types/structured/regular/field_descriptor.hpp"

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace structured {
namespace regular {

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
#ifdef __HIP__
        py::dict info = buffer.attr("__hip_array_interface__");
#else
        py::dict info = buffer.attr("__cuda_array_interface__");
#endif

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
