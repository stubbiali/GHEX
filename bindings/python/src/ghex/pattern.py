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

import ghex_py_bindings as _ghex
from ghex.utils.cpp_wrapper_utils import unwrap

if TYPE_CHECKING:
    from ghex.structured.regular.domain_descriptor import DomainDescriptor
    from ghex.structured.regular.halo_generator import HaloGenerator
    from ghex.transport_layer.context import Context


def make_pattern(context: Context, halo_gen: HaloGenerator, domain_range: DomainDescriptor):
    # todo: select based on arg types
    return _ghex.make_pattern(unwrap(context), unwrap(halo_gen), [unwrap(d) for d in domain_range])
