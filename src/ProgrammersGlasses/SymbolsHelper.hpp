//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file SymbolsHelper.hpp
/// \brief helper for symbols
//
#pragma once

#include <mutex>

/// Helper class for symbols
class SymbolsHelper
{
public:
   /// initializes symbols helper (once) and returns if services are available
   static bool Init();

   /// undecorates symbol to a developer readable name
   static CString UndecorateSymbol(const CString& symbolName);

private:
   /// undecorates MSVC based symbols that start with a question mark
   static CString UndecorateMsvcSymbol(const CString& symbolName);

   /// lock for single-threaded access to DbgHelp functions
   static std::recursive_mutex m_dbgHelpLock;

   /// symbol cache
   static std::map<CString, CString> m_symbolCache;

   /// lock for symbol cache
   static std::recursive_mutex m_cacheLock;

   /// count of cache hits
   static unsigned int m_cacheHits;

   /// count of cache misses
   static unsigned int m_cacheMiss;
};
