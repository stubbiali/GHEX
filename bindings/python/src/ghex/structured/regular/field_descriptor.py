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

from ghex.utils.architecture import Architecture
from ghex.utils.cpp_wrapper_utils import dtype_to_cpp, unwrap, cls_from_cpp_type_spec

if TYPE_CHECKING:
    import numpy as np
    from typing import Optional, Union

    try:
        import cupy as cp
    except ImportError:
        cp = np

    from ghex.structured.regular import DomainDescriptor


def _layout_order(
    field: Union[np.ndarray, cp.ndarray], architecture: Architecture
) -> tuple[int, ...]:
    if architecture == Architecture.CPU:
        strides = getattr(field, "__array_interface__", {}).get("strides", None)
    elif architecture == Architecture.GPU:
        strides = getattr(field, "__cuda_array_interface__", {}).get("strides", None)
    else:
        raise ValueError()

    # `strides` field of array interface protocol is empty for C-style contiguous arrays
    if strides is None:
        strides = getattr(field, "strides", None)
    assert strides is not None

    ordered_strides = list(reversed(sorted(strides)))
    layout_map = [ordered_strides.index(stride) for stride in strides]
    # ensure layout map has unique indices in case the size in dimension is one
    for i, val in enumerate(layout_map):
        if val in layout_map[:i]:
            layout_map[i] = max(layout_map) + 1
    return tuple(layout_map)


def get_field_descriptor(
    domain_desc: DomainDescriptor,
    field: Union[np.ndarray, cp.ndarray],
    offsets: tuple[int, ...],
    extents: tuple[int, ...],
    *,
    architecture: Optional[Architecture] = None,
):
    if not architecture:
        if hasattr(field, "__cuda_array_interface__"):
            architecture = Architecture.GPU
        elif hasattr(field, "__array_interface__"):
            architecture = Architecture.CPU
        else:
            raise ValueError()

    if architecture == Architecture.CPU:
        assert hasattr(field, "__array_interface__")
    if architecture == Architecture.GPU:
        assert hasattr(field, "__cuda_array_interface__")

    type_spec = (
        "gridtools::ghex::structured::regular::field_descriptor",
        dtype_to_cpp(field.dtype),
        architecture.value,
        domain_desc.__cpp_type__,
        f"gridtools::layout_map<{', '.join(map(str, _layout_order(field, architecture)))}> ",
    )
    return cls_from_cpp_type_spec(type_spec)(
        unwrap(domain_desc), unwrap(field), unwrap(offsets), unwrap(extents)
    )


def wrap_field(*args):
    return get_field_descriptor(*args)
