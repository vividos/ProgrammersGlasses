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
struct SectionHeader
{
   /// section name, or when starting with / char, offset into string table
   CHAR name[8];
   DWORD virtualSize;
   DWORD virtualAddress;
   DWORD sizeOfRawData;
   DWORD pointerToRawData;
   DWORD pointerToRelocations;
   DWORD pointerToLinenumbers;
   WORD numberOfRelocations;
   WORD numberOfLinenumbers;
   DWORD characteristics;
};

#pragma pack(pop)

static_assert(sizeof(SectionHeader) == 40, "Section header must be 40 bytes long");

/// struct definition for above COFF section header
extern StructDefinition g_definitionSectionHeader;
