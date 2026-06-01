//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file AnonymousObjectHeaderBigObj.hpp
/// \brief header definition of BigObj anonymous object files
//
#pragma once

#include "StructDefinition.hpp"
#include "AnonymousObjectHeader.hpp"

#pragma pack(push, 1)

/// \brief Big Object anonymous object header
/// \see https://peter0x44.github.io/posts/bigobj_format_explained/
struct AnonymousObjectHeaderBigObj : AnonymousObjectHeader
{
   DWORD flags;               ///< flags; 0x1: contains metadata
   DWORD metadataSize;        ///< CLR metadata size
   DWORD metadataOffset;      ///< CLR metadata offset
   DWORD numberOfSections;    ///< 32-bit field (16-bit in regular COFF)
   DWORD pointerToSymbolTable;///< offset to symbol table
   DWORD numberOfSymbols;     ///< number of symbols
};

#pragma pack(pop)

static_assert(sizeof(AnonymousObjectHeaderBigObj) == 56,
   "BigObj Anonymous object header must be 56 bytes long");

static_assert(sizeof(AnonymousObjectHeaderBigObj) == sizeof(ANON_OBJECT_HEADER_BIGOBJ),
   "BigObj Anonymous object header must have same size as ANON_OBJECT_HEADER");

/// struct definition for BigObj Anonymous object header
extern const StructDefinition g_definitionAnonymousObjectHeaderBigObj;
