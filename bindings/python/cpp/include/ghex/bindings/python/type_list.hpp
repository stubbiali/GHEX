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

#include "ghex/arch_list.hpp"
#include "ghex/communication_object_2.hpp"
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
    using architecture_type = gridtools::ghex::gpu;
#else
    using architecture_type = gridtools::ghex::cpu;
#endif
    using domain_id_type = int;
    using data_type = double;
    using grid_type = gridtools::ghex::structured::grid;
    using dim = std::integral_constant<int, 3>;
    using layout_map = gridtools::layout_map<0, 1, 2>;

    template <typename DomainDescriptor>
    using domain_range_type = std::vector<DomainDescriptor>;

    using transport_type = gridtools::ghex::tl::mpi_tag;
    using context_type = typename gridtools::ghex::tl::context_factory<transport>::context_type;
    using communicator_type = typename context_type::communicator_type;

    using halo_generator_type = gridtools::ghex::structured::regular::halo_generator<int,std::integral_constant<int, 3>>;
};

}
}
}
}