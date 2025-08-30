/*
-----------------
 Persistent info
-----------------

This file is part of the "Zv3D" project.

MIT License

Copyright (c) 2012-2099 Marat Sungatullin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

......
 Web:
......

 + https:// (for questions and help)

-------------
 Description
-------------
Purpose: blank file for headers.

----------------------
 For developers notes
----------------------

*/
#pragma once


#include "zvdtypes.h"
#include "zvddefaultallocator.h"
#include "zvddebug.h"

#include <type_traits>
#include <utility>
#include <memory>
#include <stdexcept>

template<
    typename T,
    typename TAllocator = ZvdcDefaultAllocator<T>
>
class ZvdcDArray
{
public:
    using value_type = T;
    using allocator_type = TAllocator;
    using size_type = size_t;
    using iterator = T*;
    using const_iterator = const T*;

    ZvdcDArray() noexcept = default;

    ~ZvdcDArray()
    {
        clear(); 
        GetAllocator()->deallocate(m_pData, m_nCapacity);
    }

    ZvdcDArray(const ZvdcDArray& other)
    {
        try
        {
            clear_and_reserve(other.size());
        }
        catch (const std::bad_alloc&)
        {
            throw;
        }

        if constexpr (std::is_trivially_copyable_v<T>)
        {
            std::memcpy(m_pData, other.data(), other.size() * sizeof(T));
        }
        else
        {
            std::uninitialized_copy_n(other.data(), other.size(), m_pData);
        }
        m_nCount = other.size();
    }

    ZvdcDArray& operator=(const ZvdcDArray& other)
    {
        if (this == &other)
        {
            return *this;
        }

        try
        {
            clear_and_reserve(other.size());
        }
        catch (const std::bad_alloc&)
        {
            throw;
        }

        if constexpr (std::is_trivially_copyable_v<T>)
        {
            std::memcpy(m_pData, other.data(), other.size() * sizeof(T));
        }
        else
        {
            std::uninitialized_copy_n(other.data(), other.size(), m_pData);
        }
        m_nCount = other.size();

        return *this;
    }

    void push_back(const T& val)
    {
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            PushBackPOD(val);
        }
        else
        {
            if (m_nCount < m_nCapacity)
            {
                PushBackInPlace(val);
            }
            else
            {
                PushBackReallocate(val);
            }
        }
    }

    void pop_back()
    {
        ZVD_ASSERT(!empty(), "pop_back() called on empty array");
        --m_nCount;

        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            std::destroy_at(m_pData + m_nCount);
        }
    }

    iterator erase(const_iterator pos)
    {
        ZVD_ASSERT(pos >= begin() && pos < end(), "erase() iterator out of bounds");

        iterator it = begin() + (pos - begin());

        if constexpr (std::is_trivially_copyable_v<T>)
        {
            std::memmove(it, it + 1, (end() - (it + 1)) * sizeof(T));
        }
        else
        {
            std::move(it + 1, end(), it);

            std::destroy_at(end() - 1);
        }

        --m_nCount;
        
        return it;
    }

    void clear() noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            std::destroy_n(m_pData, m_nCount);
        }

        // Reset the size counter. Capacity remains.
        m_nCount = 0;
    }

    void clear_and_reserve(size_type nNewCapacity)
    {
        clear();

        if (nNewCapacity > m_nCapacity)
        {
            allocator_type* pAllocator = GetAllocator();

            // Let's assume reallocate might throw bad_alloc.
            T* pNewData = pAllocator->reallocate(m_pData, nNewCapacity);
            if (!pNewData && nNewCapacity > 0)
            {
                throw std::bad_alloc();
            }

            m_pData = pNewData;
            m_nCapacity = nNewCapacity;
        }
    }

   
    size_type size() const noexcept { return m_nCount; }
    size_type capacity() const noexcept { return m_nCapacity; }
    bool empty() const noexcept { return m_nCount == 0; }

    size_type max_size() const noexcept
    {
        // A theoretical maximum based on size_t and element size.
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    T& operator[](size_type nIndex)
    {
        ZVD_ASSERT(nIndex < m_nCount, "Index out of bounds");
        return m_pData[nIndex];
    }

    const T& operator[](size_type nIndex) const
    {
        ZVD_ASSERT(nIndex < m_nCount, "Index out of bounds");
        return m_pData[nIndex];
    }

    T& at(size_type nIndex)
    {
        if (nIndex >= m_nCount)
        {
            throw std::out_of_range("ZvdcDArray::at index out of bounds");
        }
        return m_pData[nIndex];
    }

    const T& at(size_type nIndex) const
    {
        if (nIndex >= m_nCount)
        {
            throw std::out_of_range("ZvdcDArray::at index out of bounds");
        }
        return m_pData[nIndex];
    }

    T& front()
    {
        ZVD_ASSERT(!empty(), "front() called on empty array");
        return m_pData[0];
    }

    const T& front() const
    {
        ZVD_ASSERT(!empty(), "front() called on empty array");
        return m_pData[0];
    }

    T& back()
    {
        ZVD_ASSERT(!empty(), "back() called on empty array");
        return m_pData[m_nCount - 1];
    }

    const T& back() const
    {
        ZVD_ASSERT(!empty(), "back() called on empty array");
        return m_pData[m_nCount - 1];
    }

    T* data() noexcept { return m_pData; }
    const T* data() const noexcept { return m_pData; }

    iterator begin() noexcept { return m_pData; }
    iterator end() noexcept { return m_pData + m_nCount; }
    const_iterator begin() const noexcept { return m_pData; }
    const_iterator end() const noexcept { return m_pData + m_nCount; }
private:
    //-------------------------------------------------------------------------
    // Implementation Helpers
    //-------------------------------------------------------------------------
    TAllocator* GetAllocator()
    {
        if (TAllocator::CanBeCreatedOnStack()) {
            static ZvdByte allocatorMem[TAllocator::GetInstanceSize()];
            return TAllocator::CreateOnStack(allocatorMem);

        }
        return nullptr;
    }

    void PushBackInPlace(const T& val)
    {
        ZVD_ASSERT(m_nCount < m_nCapacity, "PushBackInPlace called with no capacity left");

        std::construct_at(std::addressof(m_pData[m_nCount]), val);

        ++m_nCount;
    }

    void PushBackReallocate(const T& val)
    {
        allocator_type* pAllocator = GetAllocator();
        ZVD_ASSERT(pAllocator);
        const size_type nNewCapacity = pAllocator->GetNewCapacity(m_nCount + 1, m_nCapacity);

        if (nNewCapacity > max_size()) {
            throw std::length_error("ZvdcDArray reallocation exceeds max_size()");
        }

        T* pNewData{};

        try
        {
            pNewData = pAllocator->allocate(nNewCapacity);
        }
        catch (const std::bad_alloc&)
        {
            throw;
        }

        size_type nConstructedCount{};
        try
        {
            if constexpr (std::is_nothrow_move_constructible_v<T>)
            {
                for (; nConstructedCount < m_nCount; ++nConstructedCount)
                {
                    std::construct_at(
                        std::addressof(pNewData[nConstructedCount]),
                        std::move(m_pData[nConstructedCount])
                    );
                }
            }
            else
            {
                for (; nConstructedCount < m_nCount; ++nConstructedCount)
                {
                    std::construct_at(
                        std::addressof(pNewData[nConstructedCount]),
                        m_pData[nConstructedCount]
                    );
                }
            }

            std::construct_at(std::addressof(pNewData[m_nCount]), val);
            ++nConstructedCount;
        }
        catch (...)
        {
            std::destroy_n(pNewData, nConstructedCount);
            pAllocator->deallocate(pNewData, nNewCapacity);
            throw;
        }

        if (m_pData)
        {
            std::destroy_n(m_pData, m_nCount);
            pAllocator->deallocate(m_pData, m_nCapacity);
        }

        m_pData = pNewData;
        m_nCapacity = nNewCapacity;
        ++m_nCount;
    }

    void PushBackPOD(const T& val)
    {
        if (m_nCount >= m_nCapacity)
        {
            allocator_type* pAllocator = GetAllocator();
            ZVD_ASSERT(pAllocator);
            const size_type nNewCapacity = pAllocator->GetNewCapacity(m_nCount + 1, m_nCapacity);

            if (nNewCapacity > max_size()) {
                throw std::length_error("ZvdcDArray reallocation exceeds max_size()");
            }
            
            T* pNewData = pAllocator->reallocate(m_pData, nNewCapacity);

            if (!pNewData) {
                throw std::bad_alloc();
            }

            m_pData = pNewData;
            m_nCapacity = nNewCapacity;
        }

        memcpy(std::addressof(m_pData[m_nCount++]), std::addressof(val), sizeof(T));
    }

private:
    T* m_pData{};
    size_type   m_nCount{};
    size_type   m_nCapacity{};
};