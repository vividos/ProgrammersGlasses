//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file DataHelper.cpp
/// \brief data helper functions
//
#include "stdafx.h"
#include "DataHelper.hpp"

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
