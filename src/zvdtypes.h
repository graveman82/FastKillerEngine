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

#if __cplusplus < 201703L
#error "ZVD Engine requires a C++17 compliant compiler or newer!"
#endif

#include <utility>
#include <limits>       // Для std::numeric_limits
#include <cstdint>      // Для int8_t, uint32_t и т.д.
#include <cstdio>
#include <cstdlib>


#define ZVD_UNUSED(x) ((void)x)

enum class ZvdeAxis
{
    kZVD_X,
    kZVD_Y,
    kZVD_Z,
    kZVD_W
};

using ZvdByte = uint8_t;
using ZvdReal32 = float;
using ZvdReal64 = double;

//-----------------------------------------------------------------------------
//
// Строки и символы
//
//-----------------------------------------------------------------------------
using ZvdCharA = char;
using ZvdpCharBufferA = char*;
using ZvdCStringA = const char*;

using ZvdCharW = wchar_t;
using ZvdpCharBufferW = wchar_t*;
using ZvdCStringW = const wchar_t*;


#ifdef ZVD_UNICODE
using ZvdChar = ZvdCharW;
using ZvdpCharBuffer = ZvdpCharBufferW;
using ZvdCString = ZvdCStringW;
#else
using ZvdChar = ZvdCharA;
using ZvdpCharBuffer = ZvdpCharBufferA;
using ZvdCString = ZvdCStringA;
#endif

