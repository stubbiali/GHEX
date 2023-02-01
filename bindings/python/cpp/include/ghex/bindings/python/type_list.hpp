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
#pragma once

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

struct type_list {
#ifdef __CUDACC__
    using architecture_type = boost::mp11::mp_list<gridtools::ghex::cpu, gridtools::ghex::gpu>;
#else
    using architecture_type = boost::mp11::mp_list<gridtools::ghex::cpu>;
#endif
    using domain_id_type = int;
    using data_type = boost::mp11::mp_list<float, double>;
    using dim_type = std::integral_constant<int, 3>;
    using grid_type = gridtools::ghex::structured::grid;
    using layout_map_type = boost::mp11::mp_list<
        gridtools::layout_map<0, 1, 2>, // cpu_kfirst
        gridtools::layout_map<2, 0, 1>, // cpu_ifirst
        gridtools::layout_map<2, 1, 0>  // gpu
    >;
    using transport_type = gridtools::ghex::tl::mpi_tag;
};

template<typename architecture_type, typename data_type, typename layout_map_type>
struct derived_type_list {
    using context_type = typename gridtools::ghex::tl::context_factory<
        typename type_list::transport_type>::context_type;
    using communicator_type = typename context_type::communicator_type;

    using domain_descriptor_type = gridtools::ghex::structured::regular::domain_descriptor<
        typename type_list::domain_id_type, typename type_list::dim_type>;
    using domain_range_type = std::vector<domain_descriptor_type>;
    using halo_generator_type = gridtools::ghex::structured::regular::halo_generator<
        typename type_list::domain_id_type, typename type_list::dim_type>;
    using pattern_container_type = decltype(gridtools::ghex::make_pattern<
        type_list::grid_type>(
            std::declval<context_type&>(),
            std::declval<halo_generator_type&>(),
            std::declval<domain_range_type&>()
        )
    );
    using field_descriptor_type = gridtools::ghex::structured::regular::field_descriptor<
        data_type, architecture_type, domain_descriptor_type, layout_map_type>;

    using grid_impl_type = typename type_list::grid_type::template type<domain_descriptor_type>;
};

}
}
}
}