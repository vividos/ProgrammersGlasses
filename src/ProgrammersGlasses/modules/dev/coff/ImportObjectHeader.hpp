//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file ImportObjectHeader.hpp
/// \brief header definition of import object files
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// \brief Import object header
/// \see https://www.geoffchappell.com/studies/msvc/link/dump/infiles/obj.htm
/// The header corresponds with winnt.h's IMPORT_OBJECT_HEADER struct.
/// The haeder is followed by two zero-terminated strings, specifying the
/// import's name and the DLL name.
struct ImportObjectHeader
{
   WORD signature1;        ///< must be IMAGE_FILE_MACHINE_UNKNOWN
   WORD signature2;        ///< must be IMPORT_OBJECT_HDR_SIG2 (0xFFFF)
   WORD version;           ///< version; 0 for import object
   WORD targetMachine;     ///< type of target machine

   DWORD timeStamp;        ///< file creation time stamp
   DWORD sizeOfData;       ///< size of data following header

   WORD ordinalOrHint;     ///< ordinal value (IMPORT_OBJECT_ORDINAL sets) or hint
   WORD flags;             ///< bitfield flags
};

#pragma pack(pop)

static_assert(sizeof(ImportObjectHeader) == 20,
   "Import object header must be 20 bytes long");

static_assert(sizeof(ImportObjectHeader) == sizeof(IMPORT_OBJECT_HEADER),
   "Import object header must have same size as IMPORT_OBJECT_HEADER");

/// signature 1 values to display text mapping
extern const std::map<DWORD, LPCTSTR> g_mapNonCoffHeaderSignature1ToDisplayText;

/// signature 2 values to display text mapping
extern const std::map<DWORD, LPCTSTR> g_mapNonCoffHeaderSignature2ToDisplayText;

/// version values to display text mapping
extern const std::map<DWORD, LPCTSTR> g_mapNonCoffHeaderVersionToDisplayText;

/// struct definition for above Import object header
extern const StructDefinition g_definitionImportObjectHeader;
