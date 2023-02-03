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
from typing import TYPE_CHECKING

from ghex.utils.cpp_wrapper_utils import CppWrapper

if TYPE_CHECKING:
    from ghex.utils.index_space import ProductSet


class DomainDescriptor(CppWrapper):
    def __init__(self, id_: int, sub_domain_indices: ProductSet):

        super(DomainDescriptor, self).__init__(
            (
                "gridtools::ghex::structured::regular::domain_descriptor",
                "int",
                f"std::integral_constant<int, {sub_domain_indices.dim}> ",
            ),
            id_,
            sub_domain_indices[0, 0, 0],
            sub_domain_indices[-1, -1, -1],
        )
