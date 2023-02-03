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

import ghex_py_bindings as _ghex


# note: we don't use the CppWrapper to avoid the runtime overhead
def get_communication_object(communicator, grid_type: str, domain_id_type: str):
    cls = getattr(
        _ghex,
        f"gridtools::ghex::communication_object<{communicator.__cpp_type__}, {grid_type}, {domain_id_type}>",
    )
    return cls(communicator)
