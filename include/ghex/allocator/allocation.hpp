/* 
 * GridTools
 * 
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 * 
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef INCLUDED_GHEX_ALLOCATOR_ALLOCATION_HPP
#define INCLUDED_GHEX_ALLOCATOR_ALLOCATION_HPP

#include <memory>

namespace gridtools {
    namespace ghex {
        namespace allocator {

            template<typename Allocator, typename ValueType = typename Allocator::value_type>
            struct allocation
            {
                using alloc_type         = typename std::allocator_traits<Allocator>::template rebind_alloc<ValueType>;
                using alloc_traits       = std::allocator_traits<alloc_type>;
                using value_type         = typename alloc_traits::value_type;
                using pointer            = typename alloc_traits::pointer;
                using const_pointer      = typename alloc_traits::const_pointer;
                using const_void_pointer = typename alloc_traits::const_void_pointer;

                alloc_type  m_alloc;
                pointer     m_pointer = nullptr;
                std::size_t m_capacity = 0u;

                ~allocation()
                {
                    if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                    m_pointer = nullptr;
                    m_capacity = 0u;
                }

                //operator value_type*() { return m_pointer; }

                allocation(alloc_type alloc)
                : m_alloc{alloc}
                {}

                allocation(alloc_type alloc, std::size_t n)
                : m_alloc{alloc}
                , m_pointer{alloc_traits::allocate(m_alloc, n)}
                , m_capacity{n}
                {}

                allocation(const allocation& other)
                : m_alloc{alloc_traits::select_on_container_copy_construction(other.m_alloc)}
                , m_pointer{alloc_traits::allocate(m_alloc, other.m_capacity)}
                , m_capacity{other.m_capacity}
                { }

                allocation(allocation&& other)
                : m_alloc{std::move(other.m_alloc)}
                , m_pointer{other.m_pointer}
                , m_capacity{other.m_capacity}
                {
                    other.m_pointer = nullptr;
                    other.m_capacity = 0u;
                }

                allocation& operator=(const allocation& other)
                {
                    using propagate = typename alloc_traits::propagate_on_container_copy_assignment;
                    if ( propagate::value && m_alloc != other.m_alloc)
                    {
                        if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                        void* ptr = &m_alloc;
                        m_alloc.~alloc_type();
                        new(ptr) alloc_type(other.m_alloc);
                        m_pointer = alloc_traits::allocate(m_alloc, other.m_capacity);
                        m_capacity = other.m_capacity;
                    }
                    if ( propagate::value && m_alloc == other.m_alloc)
                    {
                        if (m_capacity < other.m_capacity)
                        {
                            if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                            void* ptr = &m_alloc;
                            m_alloc.~alloc_type();
                            new(ptr) alloc_type(other.m_alloc);
                            m_pointer = alloc_traits::allocate(m_alloc, other.m_capacity);
                            m_capacity = other.m_capacity;
                        }
                        else
                        {
                            void* ptr = &m_alloc;
                            m_alloc.~alloc_type();
                            new(ptr) alloc_type(other.m_alloc);
                        }
                    }
                    if (!propagate::value && m_alloc == other.m_alloc)
                    {
                        if (m_capacity < other.m_capacity)
                        {
                            if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                            m_pointer = alloc_traits::allocate(m_alloc, other.m_capacity);
                            m_capacity = other.m_capacity;
                        }
                    }
                    if (!propagate::value && m_alloc != other.m_alloc)
                    {
                        if (m_capacity < other.m_capacity)
                        {
                            if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                            m_pointer = alloc_traits::allocate(m_alloc, other.m_capacity);
                            m_capacity = other.m_capacity;
                        }
                    }
                    return *this;
                }

                allocation& operator=(allocation&& other)
                {
                    using propagate = typename alloc_traits::propagate_on_container_move_assignment;
                    if (propagate::value || m_alloc == other.m_alloc)
                    {
                        // deallocate memory
                        if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                        // move data
                        if (propagate::value)
                        {
                            void* ptr = &m_alloc;
                            m_alloc.~alloc_type();
                            new(ptr) alloc_type(std::move(other.m_alloc));
                        }
                        m_pointer = other.m_pointer;
                        m_capacity = other.m_capacity;
                        other.m_pointer = nullptr;
                        other.m_capacity = 0u;
                    }
                    else
                    {
                        // reallocate memory if necessary
                        if (m_capacity < other.m_capacity)
                        {
                            if (m_pointer) alloc_traits::deallocate(m_alloc, m_pointer, m_capacity);
                            m_pointer = alloc_traits::allocate(m_alloc, other.m_capacity);
                            m_capacity = other.m_capacity;
                        }
                    }
                    return *this;
                }

                void swap(allocation& other)
                {
                    using propagate = typename alloc_traits::propagate_on_container_swap;
                    if (propagate::value || (!propagate::value && (m_alloc != other.m_alloc)))
                    {
                        auto a = std::move(m_alloc);
                        void* ptr_a = &m_alloc;
                        void* ptr_b = &other.m_alloc;
                        m_alloc.~alloc_type();
                        new(ptr_a) alloc_type{std::move(other.m_alloc)};
                        ~other.m_alloc();
                        new(ptr_b) alloc_type{std::move(a)};
                    }
                    auto ptr = m_pointer;
                    auto c   = m_capacity;
                    m_pointer  = other.m_pointer;
                    m_capacity = other.m_capacity;
                    other.m_pointer  = ptr;
                    other.m_capacity = c;
                }
            };

            template<typename A, typename V>
            void swap(allocation<A,V>& a, allocation<A,V>& b)
            {
                a.swap(b);    
            }

        } // namespace allocator
    } // namespace ghex
} // namespace gridtools

#endif /* INCLUDED_GHEX_ALLOCATOR_ALLOCATION_HPP */

