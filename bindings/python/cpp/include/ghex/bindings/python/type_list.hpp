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
#ifndef INCLUDED_GHEX_PYBIND_TYPE_LIST_HPP
#define INCLUDED_GHEX_PYBIND_TYPE_LIST_HPP

#include "boost/mp11/list.hpp"

#include "gridtools/common/layout_map.hpp"

#include "ghex/arch_list.hpp"
#include "ghex/structured/grid.hpp"
#include "ghex/structured/pattern.hpp"
#include "ghex/structured/regular/domain_descriptor.hpp"
#include "ghex/structured/regular/field_descriptor.hpp"
#include "ghex/structured/regular/halo_generator.hpp"
#include "ghex/transport_layer/context.hpp"
#include "ghex/transport_layer/mpi/context.hpp"

namespace gridtools {
namespace ghex {
namespace bindings {
namespace python {

// fundamental types
struct type_list {
#ifdef GHEX_CUDACC
    using architecture_type = boost::mp11::mp_list<gridtools::ghex::cpu, gridtools::ghex::gpu>;
#else
    using architecture_type = boost::mp11::mp_list<gridtools::ghex::cpu>;
#endif
    using domain_id_type = int;
    using data_type = boost::mp11::mp_list<float, double>;
    using grid_type = gridtools::ghex::structured::grid;
    using layout_map_type = boost::mp11::mp_list<
        gridtools::layout_map<0, 1, 2>,  // 3d cpu_kfirst
        gridtools::layout_map<2, 0, 1>,  // 3d cpu_ifirst
        gridtools::layout_map<2, 1, 0>,  // 3d gpu
        gridtools::layout_map<0, 1>,     // 2d column-major
        gridtools::layout_map<1, 0>      // 2d row-major
    >;
    using transport_type = gridtools::ghex::tl::mpi_tag;
};

// retrieve dim type from layout
template <typename layout_map_type>
using dim_type = std::integral_constant<int, layout_map_type::max_arg + 1>;

// container of derived types (i.e. templated in fundamental types)
template <typename... T>
struct derived_type_list;

// collection of composite types which are not templated in either arch, data type, or layout
template <typename dim_type>
struct derived_type_list<dim_type> {
    using context_type = typename gridtools::ghex::tl::context_factory<
        typename type_list::transport_type>::context_type;
    using communicator_type = typename context_type::communicator_type;

    using domain_descriptor_type = gridtools::ghex::structured::regular::domain_descriptor<
        typename type_list::domain_id_type, dim_type>;
    using domain_range_type = std::vector<domain_descriptor_type>;
    using halo_generator_type = gridtools::ghex::structured::regular::halo_generator<
        typename type_list::domain_id_type, dim_type>;
    using pattern_container_type = decltype(gridtools::ghex::make_pattern<
        type_list::grid_type>(
            std::declval<context_type&>(),
            std::declval<halo_generator_type&>(),
            std::declval<domain_range_type&>()
        )
    );

    using grid_impl_type = typename type_list::grid_type::template type<domain_descriptor_type>;
};

// collection of composite types which are templated in either arch, data type, or layout
template <typename architecture_type, typename data_type, typename layout_map_type>
struct derived_type_list<architecture_type, data_type, layout_map_type> {
    using inner = derived_type_list<dim_type<layout_map_type>>;

    using domain_descriptor_type = typename inner::domain_descriptor_type;
    using field_descriptor_type = gridtools::ghex::structured::regular::field_descriptor<
        data_type, architecture_type, domain_descriptor_type, layout_map_type>;
};

}
}
}
}

#endif