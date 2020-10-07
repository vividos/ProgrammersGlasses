//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffHeader.hpp
/// \brief header definition of COFF files
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// \brief COFF header
/// \see https://web.archive.org/web/20061216043713/http://support.microsoft.com/?id=121460
/// \see https://web.archive.org/web/20100314142533/http://www.microsoft.com/whdc/system/platform/firmware/PECOFFdwn.mspx
/// The header corresponds with winnt.h's IMAGE_FILE_HEADER struct.
/// The header is immediately followed by the section table. The string table is immediately
/// following the symbol table, if there is any.
struct CoffHeader
{
   WORD targetMachine;        ///< type of target machine
   WORD numberOfSections;     ///< number of sections in the section table

   DWORD timeStamp;           ///< file creation time stamp
   DWORD offsetSymbolTable;   ///< offset of the symbol table in the file
   DWORD numberOfSymbols;     ///< number of symbols in the symbol table

   WORD optionalHeaderSize;   ///< size of the optional header
   WORD characteristicsFlags; ///< attribute flags
};

#pragma pack(pop)

static_assert(sizeof(CoffHeader) == 20, "COFF header must be 20 bytes long");

/// mapping of COFF target machine to display text
extern std::map<DWORD, LPCTSTR> g_mapCoffTargetMachineToDisplayText;

/// mapping of COFF characteristics bits to display text
extern std::map<DWORD, LPCTSTR> g_mapCoffCharacteristicsBitsToDisplayText;

/// struct definition for above COFF header
extern StructDefinition g_definitionCoffHeader;
