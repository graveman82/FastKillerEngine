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

    explicit ZvdcDArray(size_type nInitialCount);
    ZvdcDArray(size_type nInitialCount, const T& value);

    ~ZvdcDArray()
    {
        clear(); 
        GetAllocator()->deallocate(m_pData, m_nCapacity);
    }

    ZvdcDArray(const ZvdcDArray& other);
    ZvdcDArray& operator=(const ZvdcDArray& other);
    ZvdcDArray(ZvdcDArray&& other) noexcept;
    ZvdcDArray& operator=(ZvdcDArray&& other) noexcept;


    void push_back(const T& val)
    {
        size_type nNewCount = m_nCount + 1;
        if (nNewCount <= m_nCapacity)
        {
            PushBackInPlace(val);
        }
        else
        {
            PushBackReallocate(val);
        }
    }

    void push_back(T&& value);

    template<typename... TArgs>
    T& emplace_back(TArgs&&... args);

    size_type push_back_unique(const T& val);
    void pop_back();

 
    iterator erase(const_iterator pos);

    void clear() noexcept;

    void reserve(size_type nNewCapacity);
    void resize(size_type nNewCount);

   
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
    TAllocator* GetAllocator();

    void PushBackInPlace(const T& val)
    {

    }

    void PushBackReallocate(const T& val)
    {
        
    }
private:
    T* m_pData{};
    size_type   m_nCount{};
    size_type   m_nCapacity{};
};