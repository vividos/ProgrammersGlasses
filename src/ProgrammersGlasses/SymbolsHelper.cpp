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

std::map<CString, CString> SymbolsHelper::m_symbolCache;

std::recursive_mutex SymbolsHelper::m_cacheLock;

unsigned int SymbolsHelper::m_cacheHits = 0;

unsigned int SymbolsHelper::m_cacheMiss = 0;

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
   {
      std::scoped_lock lock{ m_cacheLock };

      auto iter = m_symbolCache.find(symbolName);
      if (iter != m_symbolCache.end())
      {
         //ATLTRACE(_T("Symbol cache: hits: %u, misses: %u\n"),
         //   ++m_cacheHits, m_cacheMiss);

         return iter->second;
      }
   }

   if (symbolName.Find(_T('?')) == 0 ||
      symbolName.Find(_T('$')) == 0 ||
      symbolName.Find(_T("__imp_?")) == 0)
      return UndecorateMsvcSymbol(symbolName);

   // TODO implement gcc demangle

   return symbolName;
}

CString SymbolsHelper::UndecorateMsvcSymbol(const CString& symbolName)
{
   if (!Init())
      return symbolName + _T(" - dbghelp.dll SymInitialize failed!");

   if (symbolName.Find(_T("__imp_?")) == 0)
   {
      return _T("import: ") + UndecorateMsvcSymbol(symbolName.Mid(6));
   }

   //ATLTRACE(_T("Symbol cache: hits: %u, misses: %u\n"),
   //   m_cacheHits, ++m_cacheMiss);

   CString undecoratedName;

   DWORD result;
   {
      // since there's no way to ask UnDecorateSymbolName() for the buffer size,
      // just use a big enough buffer
      const DWORD length = 1024;

      std::scoped_lock dbgHelpLock{ m_dbgHelpLock };
      result = UnDecorateSymbolName(
         symbolName,
         undecoratedName.GetBuffer(length + 1),
         length,
         UNDNAME_COMPLETE);

      undecoratedName.ReleaseBuffer(result);
   }

   {
      std::scoped_lock cacheLock{ m_cacheLock };
      m_symbolCache.insert(std::make_pair(
         symbolName,
         result > 0
         ? undecoratedName
         : symbolName));
   }

   return result > 0
      ? undecoratedName
      : symbolName;
}
