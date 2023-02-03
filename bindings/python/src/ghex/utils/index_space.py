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
from collections.abc import Sequence
import functools
import itertools
import math
import operator
from typing import Any, Literal, Optional, Union


Integer = Union[int, Literal[math.inf], Literal[-math.inf]]


def is_integer_like(val):
    return isinstance(val, int) or val == math.inf or val == -math.inf


class Set:
    def __mul__(self, other: Set) -> Set:
        ...

    @property
    def bounds(self) -> Set:
        ...

    def complement(self, simplify: bool) -> Set:
        ...

    @property
    def empty(self) -> bool:
        ...

    @staticmethod
    def empty_set() -> Set:
        ...

    def intersect(self, other: Union[Set, UnionMixin]) -> Set:
        ...

    def issubset(self, other: Union[Set, UnionMixin]) -> bool:
        ...

    @staticmethod
    def union_type() -> type:
        ...

    def without(self, other: Set, *tail: Set, simplify: bool) -> Set:
        ...


class IntegerSet(Set):
    """A set containing integers."""

    @staticmethod
    def empty_set() -> UnitRange:
        return UnitRange(0, 0)

    @staticmethod
    def universe() -> UnitRange:
        return UnitRange(-math.inf, math.inf)

    @staticmethod
    def primitive_type():
        return UnitRange

    @staticmethod
    def union_type():
        return UnionRange


class UnitRange(IntegerSet):
    """Range from `start` to `stop` with step size one."""

    start: Integer
    stop: Integer

    def __init__(self, start: Integer, stop: Integer) -> None:
        assert stop >= start

        self.start = start
        self.stop = stop

        # canonicalize
        if self.empty:
            self.start = 0
            self.stop = 0

    @property
    def size(self) -> int:
        """Return the number of elements."""
        assert self.start <= self.stop
        return self.stop - self.start

    @property
    def empty(self) -> bool:
        """Return if the range is empty"""
        return self.start >= self.stop

    @property
    def bounds(self) -> UnitRange:
        """Smallest range containing all elements. In this case itelf."""
        return self

    def __eq__(self, other: Any) -> bool:
        """Return if `self` and `other` contain the same elements."""
        if isinstance(other, Set):
            return self.issubset(other) and other.issubset(self)

        return False

    def __contains__(self, arg: Integer) -> bool:
        """Is (the integer-like) `arg` element of this range"""
        assert is_integer_like(arg)

        return self.start <= arg < self.stop

    def issubset(self, arg: Set) -> bool:
        """Return if `self` is a subset of `arg`."""
        return arg.complement(simplify=False).intersect(self).empty

    def __getitem__(self, arg: Union[int, slice]) -> Union[int, UnitRange]:
        """Return element(s) at relative index (slice)"""
        if isinstance(arg, slice):
            assert arg.step in [1, None]

            if arg.start is None:
                start = self.start
            elif arg.start < 0:
                start = self.stop + arg.start
            else:
                start = self.start + arg.start

            if arg.stop is None:
                stop = self.stop
            elif arg.stop < 0:
                stop = self.stop + arg.stop
            else:
                stop = self.start + arg.stop

            return UnitRange(start, stop)
        elif isinstance(arg, int):
            result = (self.start if arg >= 0 else self.stop) + arg

            if result not in self:
                raise IndexError()

            return result

        raise ValueError(f"Invalid argument `{arg}`")

    def __str__(self):
        return f"UnitRange({self.start}, {self.stop})"

    def __mul__(self, other: Set) -> Set:
        """Cartesian product of `self` with `other`"""
        if isinstance(other, ProductSet):
            return ProductSet(self, *other.args)
        elif isinstance(other, UnitRange):
            return ProductSet(self, other)
        elif isinstance(other, UnionRange) or isinstance(other, UnionCartesian):
            return union(
                *(self * arg for arg in other.args), disjoint=other.disjoint, simplify=False
            )

        raise NotImplementedError()

    def __iter__(self):
        """Return an iterator over all elements of the set"""
        return range(self.start, self.stop).__iter__()

    def __hash__(self) -> int:
        return hash((self.start, self.stop))

    def intersect(self, other: Set) -> Union[UnitRange, UnionRange]:
        """Return intersection of `self` with `other`"""
        if isinstance(other, UnitRange):
            start = max(self.start, other.start)
            stop = max(start, min(self.stop, other.stop))
            return UnitRange(start, stop)
        elif isinstance(other, UnionRange):
            return other.intersect(self)

        raise NotImplementedError()

    def without(
        self,
        other: Union[UnitRange, UnionRange],
        *tail: Union[UnitRange, UnionRange],
        simplify=True,
    ) -> Set:
        """
        Return range containing all elements in self, but not in other,
        i.e. the complement of `other` with `self`.
        """
        result = other.complement(self, simplify=simplify)

        return result if len(tail) == 0 else result.without(*tail, simplify=simplify)

    def complement(self, other: Optional[Set] = None, simplify: bool = True) -> Set:
        """Return the complement of self in other."""
        result = union(
            UnitRange(-math.inf, self.start),
            UnitRange(self.stop, math.inf),
            disjoint=True,
            simplify=simplify,
        )

        return result.intersect(other) if other else result

    def union(self, *others: Set) -> Set:
        """Return the union of `self` with `other`"""
        return union(self, *others)

    def extend(self, arg: Union[int, tuple[int, int]]) -> UnitRange:
        if self.empty:
            raise ValueError("The empty set can not be extended.")

        if isinstance(arg, int):
            arg = (arg, arg)

        return UnitRange(self.start - arg[0], self.stop + arg[1])

    def shrink(self, arg: Union[int, tuple[int, int]]) -> UnitRange:
        if isinstance(arg, int):
            arg = (arg, arg)

        return self.extend(tuple(-v for v in arg))

    def translate(self, arg: int) -> UnitRange:
        """Return a range shifted by arg."""
        if self.empty:
            raise ValueError("The empty set can not be translated.")

        return UnitRange(self.start + arg, self.stop + arg)

    def as_tuple(self) -> tuple[int, int]:
        return self.start, self.stop

    def __repr__(self) -> str:
        return f"UnitRange({self.start}, {self.stop})"


def union(*args: Set, simplify: bool = True, disjoint: bool = False) -> Set:
    assert len(args) > 0
    empty_set = args[0].empty_set()
    union_type = args[0].union_type()

    # remove empty sets
    args = [arg for arg in args if not arg.empty]

    # flatten
    args = functools.reduce(
        operator.add, [list(arg.args) if isinstance(arg, UnionMixin) else [arg] for arg in args], []
    )

    if len(args) == 0:
        return empty_set
    if len(args) == 1:
        return args[0]

    result = union_type(*args, disjoint=disjoint)

    return result.simplify() if simplify else result


def intersect(a: Set, *args: Set) -> Set:
    if len(args) == 0:
        return a
    return functools.reduce(lambda x, y: x.intersect(y), args, a)


class UnionMixin:
    # todo: abstract bounds property
    args: Sequence[Union[UnitRange, ProductSet]]

    disjoint: bool

    @staticmethod
    def primitive_type() -> type:
        ...

    def universe(self) -> UnionMixin:
        ...

    def __init__(self, *args: UnitRange, disjoint: bool = False) -> None:
        assert len(args) > 1

        if not all(isinstance(arg, self.primitive_type()) for arg in args):
            raise ValueError(
                "Union can only be constructed from primitive sets. Use `union` instead."
            )

        if any(arg.empty for arg in args):
            raise ValueError("Empty set given as argument. Use `union` instead.")

        self.args = args
        self.disjoint = disjoint

    @property
    def size(self) -> int:
        overlap = 0
        if not self.disjoint:
            for i, arg1 in enumerate(self.args):
                for j, arg2 in enumerate(self.args[i + 1 :], start=i + 1):
                    overlap += arg1.intersect(arg2).size

        return functools.reduce(operator.add, (arg.size for arg in self.args)) - overlap

    @property
    def empty(self) -> bool:
        return all(arg.empty for arg in self.args)

    def union(self, *args: Union[UnitRange, UnionRange]) -> Set:
        return union(*self.args, *args)

    def without(self, *others: Set, simplify: bool = True) -> Set:
        return union(
            *(s.without(*others, simplify=simplify) for s in self.args),
            disjoint=self.disjoint,
            simplify=simplify,
        )

    def complement(self, other: Optional[Set] = None, simplify: bool = True) -> Set:
        if not other:
            other = self.universe()

        return other.without(*self.args, simplify=simplify)

    def intersect(self, other: Set) -> Set:
        return union(
            *(s.intersect(other) for s in self.args), disjoint=self.disjoint, simplify=False
        )

    def translate(self, *args) -> Set:
        return union(*(s.translate(*args) for s in self.args), disjoint=self.disjoint)

    def simplify(self) -> Union[Set, UnionMixin]:
        if not self.disjoint:
            return self.make_disjoint()

        return self

    def make_disjoint(self) -> Union[Set, UnionMixin]:
        if self.disjoint:
            return self

        args = list(self.args)
        for i, arg1 in enumerate(args):
            for j, arg2 in enumerate(args[i + 1 :], start=i + 1):
                args[j] = arg2.without(arg1, simplify=False)

        return union(*args, disjoint=True, simplify=False)

    def __iter__(self):
        for arg in self.args:
            for p in arg:
                yield p

    def __eq__(self, other: Any) -> bool:
        if isinstance(other, Set):
            return self.issubset(other) and other.issubset(self)

        return False

    def issubset(self, other: Union[Set, UnionMixin]) -> bool:
        """Return if `self` is a subset of `other`."""
        return other.complement(simplify=False).intersect(self).empty

    def __contains__(self, arg: Integer) -> bool:
        return any(arg in comp for comp in self.args)

    def __repr__(self) -> str:
        return "union(" + ", ".join(str(arg) for arg in self.args) + ")"


class UnionRange(IntegerSet, UnionMixin):
    """Union of a set of integer sets"""

    def __init__(self, *args, **kwargs) -> None:
        UnionMixin.__init__(self, *args, **kwargs)

    @property
    def bounds(self) -> UnitRange:
        """Smallest UnitRange containing all elements"""
        return UnitRange(
            functools.reduce(min, (arg.start for arg in self.args)),
            functools.reduce(max, (arg.stop for arg in self.args)),
        )

    def simplify(self) -> Set:
        if not self.disjoint:
            # note: just return as UnionMixin.simplify indirectly calls UnionRange.simplify after it made its components
            #  disjoint
            return self.simplify()

        # do some basic fusing
        assert all(isinstance(arg, UnitRange) for arg in self.args)
        args = sorted(self.args, key=lambda arg: (arg.start, arg.stop))
        fused_args = [args[0]]
        for arg in args[1:]:
            if fused_args[-1].stop == arg.start:
                fused_args = [*fused_args[0:-1], UnitRange(fused_args[-1].start, arg.stop)]
            else:
                fused_args.append(arg)

        return union(*fused_args, simplify=False, disjoint=self.disjoint)

    def __mul__(self, other: Union[Set, UnionMixin]) -> Set:
        # todo: may user facing interface should simplify
        return union(*(arg * other for arg in self.args), disjoint=self.disjoint, simplify=False)

    def __hash__(self):
        return hash(tuple(hash(arg) for arg in self.args))


_empty_cartesian_cache = {}


class CartesianSet(Set):
    """A set of cartesian indices, i.e. tuples of integers."""

    @property
    def dim(self) -> int:
        ...

    # todo: implement abstract methods
    def empty_set(self) -> Set:
        if self.dim not in _empty_cartesian_cache:
            _empty_cartesian_cache[self.dim] = functools.reduce(
                operator.mul, itertools.repeat(UnitRange(0, 0), self.dim)
            )
        return _empty_cartesian_cache[self.dim]

    def universe(self) -> Set:
        return functools.reduce(
            operator.mul, itertools.repeat(UnitRange(-math.inf, math.inf), self.dim)
        )

    @staticmethod
    def union_type() -> type[UnionCartesian]:
        return UnionCartesian

    @staticmethod
    def primitive_type() -> type[ProductSet]:
        return ProductSet

    def simplify(self) -> CartesianSet:
        return self


class ProductSet(CartesianSet):
    """Cartesian product of a set of `UnitRange`s"""

    args: Sequence[UnitRange]

    def __init__(self, *args: UnitRange) -> None:
        assert all(isinstance(arg, UnitRange) for arg in args)
        assert len(args) > 1

        self.args = args

    @classmethod
    def from_coords(cls, p1: tuple, p2: tuple) -> ProductSet:
        assert len(p1) == len(p2)
        return functools.reduce(
            operator.mul, (UnitRange(first, last + 1) for (first, last) in zip(p1, p2))
        )

    @property
    def size(self) -> int:
        return functools.reduce(operator.mul, self.shape)

    @property
    def bounds(self) -> ProductSet:
        return self

    @property
    def shape(self) -> tuple[int, ...]:
        return tuple(arg.size for arg in self.args)

    @property
    def empty(self) -> bool:
        return any(arg.empty for arg in self.args)

    @property
    def dim(self) -> int:
        return len(self.args)

    def without(
        self, other: Union[ProductSet, UnionCartesian], *tail: ProductSet, simplify: bool = True
    ) -> Set:
        if isinstance(other, ProductSet):
            # if there is no overlap in any dimension nothing is to be removed
            if any(r1.intersect(r2).empty for r1, r2 in zip(self.args, other.args)):
                result = self
            else:
                if len(self.args) == 2:  # break recursion
                    result = union(
                        self.args[0].without(other.args[0], simplify=simplify) * self.args[1],
                        self.args[0].intersect(other.args[0]) * self.args[1].without(other.args[1]),
                        simplify=simplify,
                    )
                else:
                    result = union(
                        self.args[0].without(other.args[0], simplify=simplify)
                        * ProductSet(*self.args[1:]),
                        self.args[0].intersect(other.args[0])
                        * ProductSet(*self.args[1:]).without(
                            ProductSet(*other.args[1:]), simplify=simplify
                        ),
                        simplify=simplify,
                    )

            return result if len(tail) == 0 else result.without(*tail, simplify=simplify)
        elif isinstance(other, UnionCartesian):
            return self.without(other.args[0], *other.args[1:], *tail)

        raise NotImplementedError()

    def complement(self, arg: Optional[ProductSet] = None, simplify: bool = True) -> Set:
        if not arg:
            arg = self.universe()

        return arg.without(self, simplify=simplify)

    def intersect(self, other: Union[ProductSet, UnionCartesian]) -> Set:
        if isinstance(other, ProductSet):
            return functools.reduce(
                operator.mul, (arg1.intersect(arg2) for (arg1, arg2) in zip(self.args, other.args))
            )
        elif isinstance(other, UnionCartesian):
            return other.intersect(self)

        raise ValueError(f"Invalid argument `{other}`")

    def extend(self, *args: Sequence[Union[int, tuple[int, int]]]) -> ProductSet:
        if self.empty:
            raise ValueError("Empty set can not be extended")
        assert len(self.args) == len(args)
        return functools.reduce(operator.mul, (r.extend(arg) for r, arg in zip(self.args, args)))

    def shrink(self, *args: Sequence[Union[int, tuple[int, int]]]) -> ProductSet:
        args = tuple((-arg, -arg) if isinstance(arg, int) else (-arg[0], -arg[1]) for arg in args)
        return self.extend(*args)

    def translate(self, *args: Sequence[int]) -> ProductSet:
        if self.empty:
            raise ValueError("Empty set can not be translated")
        assert len(self.args) == len(args)
        return functools.reduce(operator.mul, (r.translate(arg) for r, arg in zip(self.args, args)))

    def as_tuple(self) -> tuple[[tuple[int, int]], ...]:
        return tuple(arg.as_tuple() for arg in self.args)

    def __iter__(self):
        # memory-lightweight itertools.product like iterator
        for i in self.args[0]:
            if len(self.args[1:]) > 1:
                for tail in functools.reduce(operator.mul, self.args[1:]):
                    yield i, *tail
            else:  # break recursion
                for j in self.args[1]:
                    yield i, j

    def __eq__(self, other: Set) -> bool:
        if isinstance(other, Set):
            return self.issubset(other) and other.issubset(self)

        return False

    def __contains__(self, arg: Sequence[Integer]) -> bool:
        assert all(is_integer_like(i) for i in arg)
        assert len(arg) == len(self.args)

        return all(i in r for r, i in zip(self.args, arg))

    def issubset(self, arg: Set) -> bool:
        # if isinstance(arg, ProductSet):
        #    assert len(arg.args) == len(self.args)
        #    return all(subr.issubset(r) for subr, r in zip(self.args, arg.args))
        if isinstance(arg, Set):
            return arg.complement(simplify=False).intersect(self).empty

        raise ValueError("Invalid argument `f{arg}`")

    def __getitem__(
        self, args: tuple[Union[int, slice], ...]
    ) -> Union[tuple[int, ...], ProductSet]:
        if all(isinstance(arg, int) for arg in args):
            return tuple(r[i] for r, i in zip(self.args, args))
        elif all(isinstance(arg, slice) for arg in args):
            return ProductSet(*(r[s] for r, s in zip(self.args, args)))

        raise ValueError("Invalid argument `f{arg}`")

    def __mul__(self, other: UnitRange) -> ProductSet:
        if not isinstance(other, UnitRange):
            raise ValueError("Invalid argument `f{other}`")

        return ProductSet(
            *(self.args if not other.empty else self.empty_set().args),
            other if not self.empty else other.empty_set(),
        )

    def __hash__(self) -> int:
        return hash(tuple(hash(arg) for arg in self.args))

    def __repr__(self) -> str:
        return " * ".join(repr(arg) for arg in self.args)


class UnionCartesian(CartesianSet, UnionMixin):
    """(Set)union of a set of cartesian sets"""

    def __init__(self, *args, **kwargs) -> None:
        UnionMixin.__init__(self, *args, **kwargs)

    @property
    def bounds(self) -> Set:
        return functools.reduce(
            operator.mul,
            (
                union(*comp_ranges, simplify=False).bounds
                for comp_ranges in zip(*(ps.args for ps in self.args))
            ),
        )

    @property
    def dim(self):
        assert all(arg.dim == self.args[0].dim for arg in self.args)

        return self.args[0].dim

    def simplify(self):
        a = UnionMixin.simplify(self)

        if isinstance(a, ProductSet):
            return a

        converged = False
        while not converged:
            converged = True
            for curr in a.args:
                touching_sets = [
                    other
                    for other in a.args
                    if not curr.extend(*(1 for _ in range(0, self.dim))).intersect(other).empty
                    and other != curr
                ]
                for touching_set in touching_sets:
                    covering = union(touching_set, curr, simplify=False).bounds
                    if covering.issubset(a):
                        rest = [
                            set_.without(curr)
                            for set_ in a.args
                            if not set_.issubset(covering) and set_ != curr and set_ != touching_set
                        ]
                        merged = union(
                            union(*rest, disjoint=a.disjoint, simplify=False)
                            if len(rest) > 0
                            else a.empty_set(),
                            covering,
                            disjoint=False,
                            simplify=False,
                        )
                        if isinstance(merged, ProductSet):
                            return merged
                        elif isinstance(merged, UnionCartesian):
                            merged = UnionMixin.simplify(merged)
                            if len(merged.args) < len(a.args):
                                # we found something that has lower complexity
                                converged = False
                                a = merged
                                break
                        else:
                            raise RuntimeError()
                # reset simplification to merged a
                if not converged:
                    break
        return a.make_disjoint()

    def __hash__(self) -> int:
        return hash(tuple(hash(arg) for arg in self.args))
