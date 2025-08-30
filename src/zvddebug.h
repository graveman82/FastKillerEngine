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
#include <cstdarg>   // Для variadic functions
#include <intrin.h>

//-----------------------------------------------------------------------------
// Определяем, находимся ли мы в режиме отладки
//-----------------------------------------------------------------------------
#if defined(_MSC_VER) && defined(_DEBUG)
#define ZVD_DEBUG
#elif defined(__GNUC__) && !defined(NDEBUG)
#define ZVD_DEBUG
#endif

//-----------------------------------------------------------------------------
// Точка останова (Breakpoint)
//-----------------------------------------------------------------------------
#ifdef ZVD_DEBUG
#if defined(_MSC_VER)
#define ZVD_DEBUG_BP() __debugbreak()
#elif defined(__GNUC__)
#define ZVD_DEBUG_BP() do { __asm__ ("int $3\n\t"); } while(0)
#else
#define ZVD_DEBUG_BP() // Для других компиляторов пока пусто
#endif
#else
#define ZVD_DEBUG_BP() do {} while(0)
#endif

 /// Типы ошибок
enum class ZvdeErrorType
{
    kError,      // Обычная ошибка с предложением выбора (продолжить/выйти/отладить)
    kWarning,    // Предупреждение, выполнение не прерывается
    kMustFix,    // Ошибка, которую надо исправить, но программа не падает
    kFatal       // Фатальная ошибка, после которой приложение закрывается
};

namespace zvd
{
    using ErrorType = ZvdeErrorType;

    // Эти функции не должны вызываться напрямую, используйте макросы ниже!
    namespace debug
    {
        // Внутренние реализации, вызываемые макросами
        void AssertInternal(ZvdCStringA pFile, int nLine, ZvdCStringA pExpression, ZvdCStringA pFormat, ...);
        void ErrorInternal(ErrorType errType, ZvdCStringA pFile, int nLine, ZvdCStringA pFormat, ...);
    } // eof debug

    // Вывод в консоль
    void ConsolePrint(ZvdCStringA pFormat, ...);

    // Показать диалоговое окно с информацией
    void ShowInfoDialog(ZvdCStringA pTitle, ZvdCStringA pFormat, ...);

    // Запись в лог-файл
    void Log(ZvdCStringA pFormat, ...);

} // eof zvd


//=============================================================================
//
// Макросы для использования в коде движка (вот это - главный инструмент)
//
//=============================================================================

#ifdef ZVD_DEBUG
// Проверка утверждения. Если условие ложно, сработает assert.
// ZVD_ASSERT(ptr != nullptr, "Указатель не должен быть нулевым!");
#define ZVD_ASSERT(cond, pFormat, ...) \
do { \
    if (!(cond)) { \
        zvd::debug::AssertInternal(__FILE__, __LINE__, #cond, pFormat, ##__VA_ARGS__); \
        ZVD_DEBUG_BP(); \
    } \
} while(0)
#else
// Отключен в релизной версии
#define ZVD_ASSERT(cond, pFormat, ...) ((void)0)
#endif

// Генерация ошибки с указанным типом
// ZVD_ERROR(ZvdErrorType::kFatal, "Не удалось загрузить модель: %s", modelName);
#define ZVD_ERROR(errorType, pFormat, ...) \
    do { \
        zvd::debug::ErrorInternal(errorType, __FILE__, __LINE__, pFormat, ##__VA_ARGS__); \
        if (zvd::debug::ShouldForceBreak()) ZVD_DEBUG_BP(); \
    } while(0)

