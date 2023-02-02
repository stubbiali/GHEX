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

#include "ghex/bindings/python/types/structured/regular/domain_descriptor.hpp"

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {
namespace structured {
namespace regular {

void export_domain_descriptor (py::module_& m) {
    using dim_type_list = boost::mp11::mp_unique<
        boost::mp11::mp_transform<
            gridtools::ghex::bindings::python::dim_type,
            typename gridtools::ghex::bindings::python::type_list::layout_map_type>>;
    using exporter_type_list = boost::mp11::mp_transform<domain_descriptor_exporter, dim_type_list>;
    boost::mp11::mp_for_each<exporter_type_list>([&](auto exporter){ exporter(m); });
}

}
}
}
}
}
}
}
