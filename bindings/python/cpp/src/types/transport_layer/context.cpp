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

#include "ghex/bindings/python/types/transport_layer/context.hpp"

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace transport_layer {

void export_context (py::module_& m) {
    using layout_map_type = boost::mp11::mp_at_c<
        typename gridtools::ghex::bindings::python::type_list::layout_map_type, 0>;
    using dim_type = gridtools::ghex::bindings::python::dim_type<layout_map_type>;
    using exporter_type = context_exporter<dim_type>;
    exporter_type()(m);
}

}
}
}
}
}
}
