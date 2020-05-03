//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
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
