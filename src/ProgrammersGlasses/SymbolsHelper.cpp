//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file SymbolsHelper.cpp
/// \brief helper for symbols
//
#include "stdafx.h"
#include "SymbolsHelper.hpp"

#define DBGHELP_TRANSLATE_TCHAR
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

std::recursive_mutex SymbolsHelper::m_dbgHelpLock;

bool SymbolsHelper::Init()
{
   static std::once_flag s_symInit;
   static bool s_initResult = false;

   std::call_once(
      s_symInit,
      []()
      {
         SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
         if (!SymInitialize(GetCurrentProcess(), nullptr, true))
         {
            ATLTRACE(_T("SymbolsHelper: SymInitialize failed!"));
            s_initResult = false;
         }
         else
            s_initResult = true;
      });

   return s_initResult;
}

CString SymbolsHelper::UndecorateSymbol(const CString& symbolName)
{
   if (symbolName.Find(_T('?')) == 0 ||
      symbolName.Find(_T('$')) == 0)
      return UndecorateMsvcSymbol(symbolName);

   // TODO implement gcc demangle

   return symbolName;
}

CString SymbolsHelper::UndecorateMsvcSymbol(const CString& symbolName)
{
   if (!Init())
      return symbolName + _T(" - dbghelp.dll SymInitialize failed!");

   std::scoped_lock lock{ m_dbgHelpLock };

   // since there's no way to ask UnDecorateSymbolName() for the buffer size,
   // just use a big enough buffer
   DWORD length = 1024;

   CString undecoratedName;
   DWORD result = UnDecorateSymbolName(
      symbolName,
      undecoratedName.GetBuffer(length + 1),
      length,
      UNDNAME_COMPLETE);

   undecoratedName.ReleaseBuffer(result);

   return result > 0
      ? undecoratedName
      : symbolName;
}
