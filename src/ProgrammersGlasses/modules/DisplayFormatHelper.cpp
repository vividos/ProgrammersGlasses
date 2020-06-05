//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file DisplayFormatHelper.cpp
/// \brief display format helper
//
#include "stdafx.h"
#include "DisplayFormatHelper.hpp"

CString DisplayFormatHelper::FormatDateTime(time_t time)
{
   struct tm tm = {};
   CString text;
   if (0 == ::localtime_s(&tm, &time))
   {
      text.Format(_T("%04i-%02i-%02i %02i:%02i:%02i"),
         tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
         tm.tm_hour, tm.tm_min, tm.tm_sec);
   }

   return text;
}

CString DisplayFormatHelper::FormatBitFlagsFromMap(const std::map<DWORD, LPCTSTR>& bitflagsMap, DWORD flags)
{
   CString text;
   DWORD unmappedFlags = 0;
   for (unsigned int bits = 1; bits < 32; bits++)
   {
      DWORD testFlag = 1 << bits;
      if ((flags & testFlag) == 0)
         continue;

      auto iter = bitflagsMap.find(testFlag);
      if (iter == bitflagsMap.end())
      {
         unmappedFlags |= testFlag;
         continue;
      }

      if (!text.IsEmpty())
         text += _T(" | ");
      text += iter->second;
   }

   if (unmappedFlags != 0)
   {
      if (!text.IsEmpty())
         text += _T(" | ");
      text.AppendFormat(_T("0x%08x"), unmappedFlags);
   }

   if (text.IsEmpty())
      text = _T("N/A");

   return text;
}

CString DisplayFormatHelper::FormatRawData(const BYTE* rawData, size_t length, size_t valueSize, bool littleEndian)
{
   CString text;

   size_t maxOffset = length - (length % valueSize);

   for (size_t offset = 0; offset < maxOffset; offset += valueSize)
   {
      if (offset != 0)
         text.AppendChar(_T(' '));

      DWORD value = GetBufferValueWithEndianness(rawData + offset, valueSize, littleEndian);

      switch (valueSize)
      {
      case 1:
         text.AppendFormat(_T("%02x"), value);
         break;

      case 2:
         text.AppendFormat(_T("%04x"), value);
         break;

      case 4:
         text.AppendFormat(_T("%08x"), value);
         break;

      default:
         ATLASSERT(false); // invalid valueSize value
         break;
      }
   }

   if (maxOffset < length)
   {
      text.AppendChar(_T(' '));
      text += FormatRawData(rawData + maxOffset, length - maxOffset, 1, true);
   }

   return text;
}
