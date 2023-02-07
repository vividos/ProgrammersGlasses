//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SectionHeader.hpp
/// \brief Section header definition
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// COFF file section header
/// \see https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers
struct SectionHeader
{
   /// section name, or when starting with '/' char, string decimal value of offset into string
   /// table
   CHAR name[8];

   /// virtual size of section
   DWORD virtualSize;

   /// virtual address; image files: address relative to the image base where to load this
   /// section. object files: address of the first byte before relovation; usually set to zero.
   DWORD virtualAddress;

   /// object files: size of the section; image files: size of initialized data on disk.
   DWORD sizeOfRawData;

   /// file offset to raw data
   DWORD pointerToRawData;

   /// file offset to relocations
   DWORD pointerToRelocations;

   /// file offset to line numbers
   DWORD pointerToLinenumbers;

   /// number of relocation entries
   WORD numberOfRelocations;

   /// number of line number entries
   WORD numberOfLinenumbers;

   /// characteristics flags
   DWORD sectionFlags;
};

#pragma pack(pop)

static_assert(sizeof(SectionHeader) == 40, "Section header must be 40 bytes long");

/// struct definition for above COFF section header
extern const StructDefinition g_definitionSectionHeader;
