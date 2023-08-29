# -*- coding: utf-8 -*-
#
# GridTools
#
# Copyright (c) 2014-2021, ETH Zurich
# All rights reserved.
#
# Please, refer to the LICENSE file in the root directory.
# SPDX-License-Identifier: BSD-3-Clause
__copyright__ = "Copyright (c) 2014-2021 ETH Zurich"
__license__ = "BSD-3-Clause"

import ghex_py_bindings as _ghex
from ghex.utils.setup import validate_library_version

validate_library_version(_ghex)
