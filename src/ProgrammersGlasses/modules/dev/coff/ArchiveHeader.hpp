//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file ArchiveHeader.hpp
/// \brief header definition of archive files
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// \brief archive file header
/// \details the archive format is borrowed from the Unix "ar" (archiver) tool.
struct ArchiveHeader
{
   /// signature
   CHAR signature[8];
};

/// \brief header for one archive member (or entry)
/// \details The member header is always 2-byte aligned (e.g. when the previous entry had an odd
/// length.
struct ArchiveMemberHeader
{
   /// archive member name
   CHAR name[16];

   /// the ASCII decimal representation of the date since Unix epoch (1970-01-01 00:00:00)
   CHAR dateText[12];

   /// user ID; not used in Windows
   CHAR userID[6];

   /// group ID; not used in Windows
   CHAR groupID[6];

   /// file mode as ASCII octagonal value see ST_MODE enum
   CHAR mode[8];

   /// total size of the archive member payload immediately following the header, not including
   /// this header
   CHAR sizeText[10];

   /// end of header characters; must be 0x60 0x0a
   CHAR endOfHeader[2];
};

#pragma pack(pop)

static_assert(sizeof(ArchiveHeader) == 8, "Archive header must be 8 bytes long");

static_assert(sizeof(ArchiveMemberHeader) == 60, "Archive member header must be 60 bytes long");

/// signature text for archive header
extern const LPCSTR c_archiveHeaderSignatureText;

/// struct definition for above archive header
extern const StructDefinition g_definitionArchiveHeader;

/// struct definition for above archive member header
extern const StructDefinition g_definitionArchiveMemberHeader;
