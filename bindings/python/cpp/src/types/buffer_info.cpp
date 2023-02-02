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

#include "ghex/bindings/python/types/buffer_info.hpp"

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {

void export_buffer_info (py::module_& m) {
    using exporter_list = boost::mp11::mp_product<
        buffer_info_exporter,
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
