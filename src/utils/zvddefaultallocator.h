/*
-----------------
 Persistent info
-----------------

This pFileName is part of the "Zv3D" project.

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
Purpose: blank pFileName for headers.

----------------------
 For developers notes
----------------------

*/
#pragma once

#pragma once

#include "zvdtypes.h"
#include <new>


extern "C"
{
    void* ZvdfMemAllocateA16(size_t nSize);
    void* ZvdfMemReallocateA16(void* pBlock, size_t nNewSize);
    void  ZvdfMemFreeA16(void* pBlock);
}


template<typename T>
struct ZvdcDefaultAllocator
{
    using value_type = T;
    using size_type = size_t;

   
    static constexpr bool CanBeCreatedOnStack() noexcept { return true; }

    static constexpr bool IsSingleton() noexcept { return false; }

    static constexpr bool IsEngineService() noexcept { return false; }

    static constexpr size_type GetInstanceSize() noexcept
    {
        return sizeof(ZvdcDefaultAllocator<T>);
    }

 
    static ZvdcDefaultAllocator<T>* CreateOnStack(void* pStackMem, size_type nStackMemSize)
    {
        ZVD_UNUSED(nStackMemSize); // Проверка размера должна быть в вызывающем коде
        // Просто используем placement new, чтобы создать наш экземпляр
        return new (pStackMem) ZvdcDefaultAllocator<T>();
    }


    ZvdcDefaultAllocator() noexcept = default;
    

    [[nodiscard]] T* allocate(size_type nNumObjects)
    {
        if (nNumObjects == 0) return nullptr;
        return static_cast<T*>(ZvdfMemAllocateA16(nNumObjects * sizeof(T)));
    }

    void deallocate(T* pMemory, size_type nNumObjects) noexcept
    {
        ZVD_UNUSED(nNumObjects);
        ZvdfMemFreeA16(pMemory);
    }

    [[nodiscard]] T* reallocate(T* pMemory, size_type nNewNumObjects)
    {
        return static_cast<T*>(ZvdfMemReallocateA16(pMemory, nNewNumObjects * sizeof(T)));
    }

    static constexpr size_type GetNewCapacity(size_type nRequiredSize, size_type nOldCapacity) noexcept
    {
        size_type nNewCapacity = nOldCapacity;
        if (nNewCapacity < 16)
            nNewCapacity = 16;
        while (nNewCapacity < nRequiredSize)
        {
            nNewCapacity += (nNewCapacity >> 1);
        }
        return nNewCapacity;
    }
};