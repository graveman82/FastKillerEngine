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
#include "zvddebug.h"

#include <mutex>
#include <cctype>
#include <conio.h>
#include <cstring>
#include <Windows.h>

namespace zvd
{
    // Неглобальная константа для размера буфера
    static constexpr size_t kMESSAGE_BUFSIZE = 2048;
    static ZvdCharA s_messageBuffer[kMESSAGE_BUFSIZE];
    static std::mutex s_messageBufferMutex;

    static bool s_forceBreakPoint = false;
    static std::mutex s_forceBreakPointMutex;

    static std::mutex s_stdOutputMutex;

    //-----------------------------------------------------------------------------
    // Прототипы внутренних статических функций
    //-----------------------------------------------------------------------------
    namespace debug
    {
        static void PrepareMessage(ZvdCStringA pTitle, ZvdCStringA pFileName, int nCodeLine, 
            ZvdCStringA pFormat, va_list vArgs);
        static void ConsoleAskUserToQuit(ZvdCStringA pMessage);
        static void DialogAskUserToQuit(ZvdCStringA pTitle, ZvdCStringA pMessage);
        static void FlushBufferToLog(ZvdCStringA pMessageType);
    } // namespace debug


    //=============================================================================
    //
    // Реализация публичных функций
    //
    //=============================================================================

    void ConsolePrint(ZvdCStringA pFormat, ...)
    {
        static std::mutex s_mutex;
        std::lock_guard<std::mutex> lock(s_mutex);

        va_list vArgs;
        va_start(vArgs, pFormat);
        _vsnprintf_s(s_messageBuffer, kMESSAGE_BUFSIZE, _TRUNCATE, pFormat, vArgs);
        va_end(vArgs);
        printf_s("%s\n", s_messageBuffer);
    }

    void ShowInfoDialog(ZvdCStringA pTitle, ZvdCStringA pFormat, ...)
    {
        static std::mutex s_mutex;
        std::lock_guard<std::mutex> lock(s_mutex);

        va_list vArgs;
        va_start(vArgs, pFormat);
        _vsnprintf_s(s_messageBuffer, kMESSAGE_BUFSIZE, _TRUNCATE, pFormat, vArgs);
        va_end(vArgs);

        ::MessageBoxA(nullptr, s_messageBuffer, pTitle, MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
    }

    void Log(ZvdCStringA pFormat, ...)
    {
        static std::mutex s_mutex;
        std::lock_guard<std::mutex> lock(s_mutex);

        static bool bLogInited{};
        ZvdCStringA kLogFileName = "zvd_engine_log.txt";
        FILE* pLogFile{};

        if (!bLogInited)
        {
            if (::fopen_s(&pLogFile, kLogFileName, "w") == 0 && pLogFile) {
                ::fprintf_s(pLogFile, "Log started ...\n");
                ::fclose(pLogFile);
            }
            bLogInited = true;
        }

        if (::fopen_s(&pLogFile, kLogFileName, "a") != 0 || !pLogFile) {
            return;
        }

        va_list vArgs;
        va_start(vArgs, pFormat);
        ::vfprintf_s(pLogFile, pFormat, vArgs);
        ::fprintf_s(pLogFile, "\n");
        va_end(vArgs);

        ::fclose(pLogFile);
    }

    //=============================================================================
    //
    // Реализация пространства имен debug
    //
    //=============================================================================
    namespace debug
    {
        bool ShouldForceBreak()
        {
            static std::mutex s_mutex;
            std::lock_guard<std::mutex> lock(s_mutex);
            if (s_forceBreakPoint) {
                s_forceBreakPoint = false;
                return true;
            }
            return false;
        }

        void AssertInternal(ZvdCStringA pFileName, int nCodeLine, ZvdCStringA expression, ZvdCStringA pFormat, ...)
        {
            char userMessage[kMESSAGE_BUFSIZE / 2];
            va_list vArgs;
            va_start(vArgs, pFormat);
            _vsnprintf_s(userMessage, sizeof(userMessage), _TRUNCATE, pFormat, vArgs);
            va_end(vArgs);

            {
                std::lock_guard<std::mutex> lock(s_messageBufferMutex);
                _snprintf_s(s_messageBuffer, kMESSAGE_BUFSIZE, _TRUNCATE,
                    "Expression: %s\nMessage: %s", expression, userMessage);

                PrepareMessage("ASSERT FAILED", pFileName, nCodeLine, s_messageBuffer, nullptr);
            }

            FlushBufferToLog("ASSERT FAILED");

#ifdef ZVD_DEBUG_WINDOWS_MSG
            DialogAskUserToQuit("Assert Failed", s_messageBuffer);
#else
            ConsoleAskUserToQuit(s_messageBuffer);
#endif
        }

        void ErrorInternal(ErrorType errType, ZvdCStringA pFileName, int nCodeLine, ZvdCStringA pFormat, ...)
        {
            ZvdCStringA pTitle = "Error";
            switch (errType) {
            case ErrorType::kWarning: pTitle = "Warning"; break;
            case ErrorType::kMustFix: pTitle = "Error (Must Fix)"; break;
            case ErrorType::kFatal:   pTitle = "Fatal Error"; break;
            default: break;
            }

            va_list vArgs;
            va_start(vArgs, pFormat);
            PrepareMessage(pTitle, pFileName, nCodeLine, pFormat, vArgs);
            va_end(vArgs);

            FlushBufferToLog(pTitle);

            switch (errType)
            {
            case ErrorType::kWarning:
            case ErrorType::kMustFix:
#ifdef ZVD_DEBUG_WINDOWS_MSG
                ::MessageBoxA(nullptr, s_messageBuffer, pTitle, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
#else
                printf_s("%s\n", s_messageBuffer);
#endif
                break;

            case ErrorType::kFatal:
#ifdef ZVD_DEBUG_WINDOWS_MSG
                ::MessageBoxA(nullptr, s_messageBuffer, pTitle, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
#else
                printf_s("%s\n", s_messageBuffer);
#endif
                exit(EXIT_FAILURE);
                break;

            case ErrorType::kError:
#ifdef ZVD_DEBUG_WINDOWS_MSG
                DialogAskUserToQuit(pTitle, s_messageBuffer);
#else
                ConsoleAskUserToQuit(s_messageBuffer);
#endif
                break;
            }
        }

        //-----------------------------------------------------------------------------
        // Реализация внутренних статических функций
        //-----------------------------------------------------------------------------

        static void PrepareMessage(ZvdCStringA pTitle, ZvdCStringA pFileName, int nCodeLine, ZvdCStringA pMsgFormat, va_list vArgs)
        {
            // сообщение копируем во временный буфер
            const size_t kTMP_BUFSIZE = kMESSAGE_BUFSIZE / 2;
            char tmpBuffer[kTMP_BUFSIZE];
            if (vArgs) {
                _vsnprintf_s(tmpBuffer, sizeof(tmpBuffer), _TRUNCATE, pMsgFormat, vArgs);
            }
            else {
                strcpy_s(tmpBuffer, sizeof(tmpBuffer), pMsgFormat);
            }

            // формируем финальное сообщение, добавляя заглавие и отладочную информацию
            std::lock_guard<std::mutex> lock(s_messageBufferMutex);
#ifdef ZVD_DEBUG
            _snprintf_s(s_messageBuffer, kMESSAGE_BUFSIZE, _TRUNCATE,
                "%s in %s (%d):\n%s", pTitle, pFileName, nCodeLine, tmpBuffer);
#else
            _snprintf_s(s_messageBuffer, kMESSAGE_BUFSIZE, _TRUNCATE,
                "%s:\n%s", pTitle, tmpBuffer);
#endif
        }

        static void ConsoleAskUserToQuit(ZvdCStringA pMessage)
        {
            {
                std::lock_guard<std::mutex> lock(s_stdOutputMutex);

                printf_s("\n========================================\n");
                printf_s("%s\n", pMessage);
                printf_s("========================================\n");

#ifdef ZVD_DEBUG
                printf_s("Quit (y) / Debug (x) ?");
#else
                printf_s("Quit (y) / Continue (n) ?");
#endif
            }

#ifdef ZVD_DEBUG
            const char kContinueChar = 'x';
#else
            const char kContinueChar = 'n';
#endif
            char choice;
            do 
            {
                choice = static_cast<char>(tolower(_getch()));
            } 
            while (choice != 'y' && choice != kContinueChar);

            if (choice == 'y') {
                ::exit(EXIT_FAILURE);
            }
#ifdef ZVD_DEBUG
            else if (choice == 'x') {
                std::lock_guard<std::mutex> lock(s_forceBreakPointMutex);
                s_forceBreakPoint = true;
            }
#endif
        }

        static void DialogAskUserToQuit(ZvdCStringA pTitle, ZvdCStringA pMessage)
        {
            char fullMessage[kMESSAGE_BUFSIZE];

#ifdef ZVD_DEBUG
            _snprintf_s(fullMessage, kMESSAGE_BUFSIZE, _TRUNCATE,
                "%s\nQuit (Yes) / Continue (No) / Debug (Cancel)?", pMessage);
#else
            _snprintf_s(fullMessage, kMESSAGE_BUFSIZE, _TRUNCATE,
                "%s\nQuit (Yes) / Continue (No)?", pMessage);
#endif

#ifdef ZVD_DEBUG
            const auto kButtons = MB_YESNOCANCEL;
#else
            const auto kButtons = MB_YESNO;
#endif
            int response = ::MessageBoxA(nullptr, fullMessage, pTitle, kButtons | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

            if (response == IDYES) {
                ::exit(EXIT_FAILURE);
            }
#ifdef ZVD_DEBUG
            else if (response == IDCANCEL) {
                std::lock_guard<std::mutex> lock(s_forceBreakPointMutex);
                s_forceBreakPoint = true;
            }
#endif
        }

        static void FlushBufferToLog(ZvdCStringA pMessageType)
        {
            Log("--- BEGIN %s ---", pMessageType);
            {
                std::lock_guard<std::mutex> lock(s_messageBufferMutex);
                Log(s_messageBuffer);
            }
            Log("--- END %s ---\n", pMessageType);
        }

    } // eof debug
} // eof zvd
