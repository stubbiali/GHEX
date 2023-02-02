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

#include "ghex/bindings/python/types/pattern.hpp"

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {
namespace types {

// specializations for 3d fields
using exporter_3d = pattern_container_exporter<std::integral_constant<int, 3>>;
template <>
bool exporter_3d::initialized = false;
template <>
std::unique_ptr<exporter_3d::exported_type> exporter_3d::exported_obj = nullptr;

// specializations for 2d fields
using exporter_2d = pattern_container_exporter<std::integral_constant<int, 2>>;
template <>
bool exporter_2d::initialized = false;
template <>
std::unique_ptr<exporter_2d::exported_type> exporter_2d::exported_obj = nullptr;

void export_pattern_container (py::module_& m) {
    using enricher_type_list = boost::mp11::mp_product<
        pattern_container_enricher,
        typename gridtools::ghex::bindings::python::type_list::architecture_type,
        typename gridtools::ghex::bindings::python::type_list::data_type,
        typename gridtools::ghex::bindings::python::type_list::layout_map_type>;
    boost::mp11::mp_for_each<enricher_type_list>([&](auto enricher){
        using layout_map_type = typename decltype(enricher)::layout_map_type;
        using dim_type = gridtools::ghex::bindings::python::dim_type<layout_map_type>;
        auto exporter = pattern_container_exporter<dim_type>();
        auto exported_obj = exporter(m);
        enricher(exported_obj); });
}

}
}
}
}
}
