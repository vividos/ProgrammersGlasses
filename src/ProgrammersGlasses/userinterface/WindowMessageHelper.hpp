//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file WindowMessageHelper.hpp
/// \brief window message helper functions
//
#pragma once

/// traces the window message
void AtlTraceMessage(
   _In_z_ LPCTSTR className,
   _In_z_ const char* pszFileName,
   _In_ int nLineNo,
   _In_ HWND hWnd,
   _In_ UINT uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam);

#ifdef DEBUG

/// macro that can be used in a message map to trace the messages being received
#define ATLTRACE_MESSAGE(theClass) \
   AtlTraceMessage(_T( #theClass ), __FILE__, __LINE__, hWnd, uMsg, wParam, lParam);

#else

#define ATLTRACE_MESSAGE(theClass) __noop;

#endif
