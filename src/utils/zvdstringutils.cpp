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

#include "utils/zvdstringutils.h"

#include <vector>
#include <mutex>
#include <windows.h>



std::string ZvdfConsoleStr(const char* utf8Str)
{
    static std::mutex g_consoleStrMutex;
    // current console's code page (e.g., 866 for OEM, 65001 for UTF-8).
    const UINT consoleCP = ::GetConsoleOutputCP();

    // already in UTF-8, no conversion is needed.
    if (consoleCP == CP_UTF8) {
        return utf8Str;
    }

    // Convert the input UTF-8 string to a wide string (UTF-16),
    //    which is the native format for modern Windows APIs.
    int nWideLen = ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
    if (nWideLen == 0) return ""; // Conversion error

    std::lock_guard<std::mutex> lock(g_consoleStrMutex);
    static std::vector<wchar_t> wideBuf;
    wideBuf.resize(nWideLen);
    ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wideBuf.data(), nWideLen);

    // 3. Convert the wide string (UTF-16) to the console's code page.
    int nResultLen = ::WideCharToMultiByte(consoleCP, 0, wideBuf.data(), -1, nullptr, 0, nullptr, nullptr);
    if (nResultLen == 0) return ""; // Conversion error

    static std::vector<char> resultBuf;
    resultBuf.resize(nResultLen);
    ::WideCharToMultiByte(consoleCP, 0, wideBuf.data(), -1, resultBuf.data(), nResultLen, nullptr, nullptr);

    return resultBuf.data();
}

