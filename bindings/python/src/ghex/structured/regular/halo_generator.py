# -*- coding: utf-8 -*-
#
# GridTools
#
# Copyright (c) 2014-2021, ETH Zurich
# All rights reserved.
#
# Please, refer to the LICENSE file in the root directory.
# SPDX-License-Identifier: BSD-3-Clause
from __future__ import annotations
from dataclasses import dataclass
from typing import TYPE_CHECKING

from ghex.utils.cpp_wrapper_utils import CppWrapper
from ghex.utils.index_space import ProductSet, union

if TYPE_CHECKING:
    from typing import Union

    from ghex.structured.regular.domain_descriptor import DomainDescriptor
    from ghex.utils.index_space import CartesianSet


@dataclass
class HaloContainer:
    local: CartesianSet
    global_: CartesianSet


class HaloGenerator(CppWrapper):
    def __init__(
        self,
        glob_domain_indices: ProductSet,
        halos: tuple[Union[int, tuple[int, int]], ...],
        periodicity: tuple[bool, ...],
    ) -> None:
        assert glob_domain_indices.dim == len(halos)
        assert glob_domain_indices.dim == len(periodicity)

        # canonanicalize integer halos, e.g. turn (h0, (h1, h2), h3) into ((h0, h0), (h1, h2), ...)
        halos = ((halo, halo) if isinstance(halo, int) else halo for halo in halos)
        flattened_halos = tuple(h for halo in halos for h in halo)

        super(HaloGenerator, self).__init__(
            (
                "gridtools::ghex::structured::regular::halo_generator",
                "int",
                f"std::integral_constant<int, {glob_domain_indices.dim}> ",
            ),
            glob_domain_indices[0, 0, 0],
            glob_domain_indices[-1, -1, -1],
            flattened_halos,
            periodicity,
        )

    def __call__(self, domain: DomainDescriptor) -> HaloContainer:
        result = self.__wrapped_call__("__call__", domain)
        local = union(
            *(
                ProductSet.from_coords(tuple(box2.local.first), tuple(box2.local.last))
                for box2 in result
            )
        )
        global_ = union(
            *(
                ProductSet.from_coords(tuple(box2.global_.first), tuple(box2.global_.last))
                for box2 in result
            )
        )
        return HaloContainer(local, global_)
