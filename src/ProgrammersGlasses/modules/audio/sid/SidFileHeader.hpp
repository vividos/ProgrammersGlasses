//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SidFileHeader.hpp
/// \brief SID file header
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// \brief SID file header
/// \note all fields are big-endian
/// \see https://gist.github.com/cbmeeks/2b107f0a8d36fc461ebb056e94b2f4d6
struct SidFileHeader
{
   /// magic ID; either PSID or RSID
   CHAR magicId[4];
   /// version, 1 to 4 (or 2 to 4 for RSID)
   WORD version;
   /// file offset to the binary data to load into C64
   WORD dataOffset;
   /// C64 address to load data to, or 0 when load address is specified in data
   WORD loadAddress;

   /// C64 address with routine to initialize the tune
   WORD initAddress;
   /// C64 address with routine to play the tune; when 0, the init routine sets up an IRQ handler
   /// that plays the tune
   WORD playAddress;

   /// number of songs; must be >= 1
   WORD numSongs;

   /// number of start song to play initially
   WORD startSong;

   /// speed bit flags for the first 32 songs
   DWORD speed;

   /// name of tune, as ASCII string, maybe not zero-terminated
   CHAR name[32];

   /// author of tune
   CHAR author[32];

   /// released info
   CHAR released[32];

   // SID version 2, 3 and 4 only from here on

   /// flags value
   WORD flags;

   /// start page (high byte) of the first free block that isn't touched by the SID code
   BYTE startPage;

   /// length, in blocks, of the area that isn't touchbed by the SID code
   BYTE pageLength;

   /// address middle bytes ($Dxx0) of a second SID chip
   BYTE secondSidAddress;

   /// address middle bytes ($Dxx0) of a third SID chip
   BYTE thirdSidAddress;
};

#pragma pack(pop)

static_assert(sizeof(SidFileHeader) == 0x7C, "SID file header must be 0x76 bytes long");

/// magic IDs for SID files
extern const std::map<DWORD, LPCTSTR> g_mapSidMagicIdsToDisplayText;

/// structure definition for SID file header
extern const const StructDefinition g_definitionSidFileHeader;

/// structure definition for the extra fields in a SID v2, v3 or v4 file header
extern const StructDefinition g_definitionSidV2FileHeader;
