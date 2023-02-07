//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffSymbolTable.hpp
/// \brief COFF symbol table definition
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// \see https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#coff-symbol-table
struct CoffSymbolTable
{
   /// symbol name
   CHAR name[8];

   /// value of the symbol, e.g. relocatable address
   DWORD value;

   /// (signed) 1-based index into the section table
   WORD sectionNumber;

   /// symbol type
   WORD type;

   /// storage class enumeration
   BYTE storageClass;

   /// the number of auxiliary symbol table entries following this struct
   BYTE numberOfAuxSymbols;
};

#pragma pack(pop)

static_assert(sizeof(CoffSymbolTable) == 18, "COFF symbol table must be 18 bytes long");

/// struct definition for above COFF symbol table
extern const StructDefinition g_definitionCoffSymbolTable;
