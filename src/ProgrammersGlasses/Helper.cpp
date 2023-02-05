//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file Helper.cpp
/// \brief helper functions
//
#include "stdafx.h"
#include "Helper.hpp"

std::vector<CString> StringSplit(const CString& text, LPCTSTR splitChars, bool addEmptyEntries)
{
   std::vector<CString> elements;

   int tokenPos = 0;

   CString token = text.Tokenize(splitChars, tokenPos);
   do
   {
      if (!token.IsEmpty() || addEmptyEntries)
         elements.push_back(token);

      token = text.Tokenize(splitChars, tokenPos);
   } while (tokenPos != -1);

   return elements;
}

void IndentText(CString& text, int numberOfSpaces)
{
   CString indentText(_T(' '), numberOfSpaces);

   text.Replace(_T("\n"), _T("\n") + indentText);
   text.Insert(0, indentText);
   text.TrimRight();
}

bool ContainsIgnoreCase(const CString& textToSearch, const CString& textToFind)
{
   return CString(textToSearch).MakeLower()
      .Find(CString(textToFind).MakeLower()) != -1;
}

DWORD GetBufferValueWithEndianness(const BYTE* buffer, size_t valueSize, bool littleEndian)
{
   switch (valueSize)
   {
   case 1:
      return buffer[0];

   case 2:
   {
      WORD value16 = *reinterpret_cast<const WORD*>(buffer);
      if (!littleEndian)
         value16 = SwapEndianness(value16);
      return value16;
   }
   break;

   case 4:
   {
      DWORD value32 = *reinterpret_cast<const DWORD*>(buffer);
      if (!littleEndian)
         value32 = SwapEndianness(value32);
      return value32;
   }
   break;

   default:
      ATLASSERT(false); // invalid valueSize; must be 1, 2 or 4
      break;
   }

   return 0;
}
