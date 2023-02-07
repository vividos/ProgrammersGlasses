//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CoffSymbolTable.cpp
/// \brief COFF symbol table definition
//
#include "stdafx.h"
#include "CoffSymbolTable.hpp"

/// COFF symbol table base type to display text mapping
std::map<DWORD, LPCTSTR> g_mapCoffSymbolTableBaseTypeToDisplayText =
{
   { 0, _T("IMAGE_SYM_TYPE_NULL (unknown)") },
   { 1, _T("IMAGE_SYM_TYPE_VOID (void* and funcs)") },
   { 2, _T("IMAGE_SYM_TYPE_CHAR (signed 1-byte int)") },
   { 3, _T("IMAGE_SYM_TYPE_SHORT  (2-byte signed int)") },
   { 4, _T("IMAGE_SYM_TYPE_INT (natural integer type)") },
   { 5, _T("IMAGE_SYM_TYPE_LONG  (4-byte signed int)") },
   { 6, _T("IMAGE_SYM_TYPE_FLOAT (4-byte float)") },
   { 7, _T("IMAGE_SYM_TYPE_DOUBLE (8-byte double)") },
   { 8, _T("IMAGE_SYM_TYPE_STRUCT (C struct)") },
   { 9, _T("IMAGE_SYM_TYPE_UNION  (C union)") },
   { 10, _T("IMAGE_SYM_TYPE_ENUM (C enum)") },
   { 11, _T("IMAGE_SYM_TYPE_MOE (enumeration member)") },
   { 12, _T("IMAGE_SYM_TYPE_BYTE (unsigned 1-byte int)") },
   { 13, _T("IMAGE_SYM_TYPE_WORD (2-byte unsigned int)") },
   { 14, _T("IMAGE_SYM_TYPE_UINT (natural unsigned integer type)") },
   { 15, _T("IMAGE_SYM_TYPE_DWORD (4-byte unsigned int)") },

   // note that Microsoft COFF symbol table only ever uses 0x00 and 0x20 for
   // the type field, so additionally define this value here.
   { 0x20, _T("MSVC: function") }
};

/// COFF symbol table complex type to display text mapping (for completeness)
std::map<DWORD, LPCTSTR> g_mapCoffSymbolTableComplexTypeToDisplayText =
{
   { 0, _T("IMAGE_SYM_DTYPE_NULL") },
   { 1, _T("IMAGE_SYM_DTYPE_POINTER") },
   { 2, _T("IMAGE_SYM_DTYPE_FUNCTION") },
   { 3, _T("IMAGE_SYM_DTYPE_ARRAY") },
};

/// COFF symbol table type bitfield
std::vector<BitfieldDescriptor> g_listCoffSymbolTableTypeBits =
{
   BitfieldDescriptor{ 0, 8, StructFieldType::valueMapping, g_mapCoffSymbolTableBaseTypeToDisplayText },
   BitfieldDescriptor{ 8, 8, StructFieldType::valueMapping, g_mapCoffSymbolTableComplexTypeToDisplayText },
};

/// COFF symbol table storage class to display text mapping
std::map<DWORD, LPCTSTR> g_mapCoffSymbolTableStorageClassToDisplayText =
{
   { 0xFF, _T("IMAGE_SYM_CLASS_END_OF_FUNCTION ") }, // -1
   { 0, _T("IMAGE_SYM_CLASS_NULL") },

   // Microsoft COFF files only use these values, so only list these (for now)
   { 2, _T("IMAGE_SYM_CLASS_EXTERNAL") },
   { 3, _T("IMAGE_SYM_CLASS_STATIC") },
   { 102, _T("IMAGE_SYM_CLASS_END_OF_STRUCT") },
   { 103, _T("IMAGE_SYM_CLASS_FILE") },
};

const StructDefinition g_definitionCoffSymbolTable = StructDefinition({
   StructField(
      offsetof(CoffSymbolTable, CoffSymbolTable::name),
      sizeof(CoffSymbolTable::name),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Symbol table name")),

   StructField(
      offsetof(CoffSymbolTable, CoffSymbolTable::value),
      sizeof(CoffSymbolTable::value),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Symbol value")),

   StructField(
      offsetof(CoffSymbolTable, CoffSymbolTable::sectionNumber),
      sizeof(CoffSymbolTable::sectionNumber),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger, // TODO rather signedInteger
      _T("Section number")),

   StructField(
      offsetof(CoffSymbolTable, CoffSymbolTable::type),
      sizeof(CoffSymbolTable::type),
      1,
      true, // little-endian
      StructFieldType::bitfieldMapping,
      g_listCoffSymbolTableTypeBits,
      _T("Symbol type")),

   StructField(
      offsetof(CoffSymbolTable, CoffSymbolTable::storageClass),
      sizeof(CoffSymbolTable::storageClass),
      1,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapCoffSymbolTableStorageClassToDisplayText,
      _T("Storage class")),

   StructField(
      offsetof(CoffSymbolTable, CoffSymbolTable::numberOfAuxSymbols),
      sizeof(CoffSymbolTable::numberOfAuxSymbols),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of aux. symbols")),
   });
