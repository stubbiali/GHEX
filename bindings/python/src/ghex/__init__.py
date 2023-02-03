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

from ghex.utils.setup import update_path, validate_library_version

update_path()

import ghex_py_bindings as _ghex

validate_library_version(_ghex)
