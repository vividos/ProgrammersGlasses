//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file StringHelper.cpp
/// \brief string helper functions
//
#include "stdafx.h"
#include "StringHelper.hpp"

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

CString EscapeText(CString text)
{
   CString replacement;
   for (int pos = 0; pos < text.GetLength(); pos++)
   {
      TCHAR ch = text[pos];

      // technically, 0x7f is a printable character, but it doesn't display
      // anything
      if (ch < 0x20 || ch >= 0x7f)
      {
         text.Delete(pos, 1);

         replacement.Format(_T("<%02x>"), ch);
         text.Insert(pos, replacement);
         pos += 3;
      }
   }

   return text;
}
