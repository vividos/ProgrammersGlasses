//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file ArchiveHeader.cpp
/// \brief header definition of archive files
//
#include "stdafx.h"
#include "ArchiveHeader.hpp"

LPCSTR c_archiveHeaderSignatureText = "!<arch>\n";

const StructDefinition g_definitionArchiveHeader = StructDefinition({
   StructField(
      offsetof(ArchiveHeader, ArchiveHeader::signature),
      sizeof(ArchiveHeader::signature),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Archive signature")),
   });

const StructDefinition g_definitionArchiveMemberHeader = StructDefinition({
   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::name),
      sizeof(ArchiveMemberHeader::name),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Entry name")),

   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::dateText),
      sizeof(ArchiveMemberHeader::dateText),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Date (Unix epoch)")),

   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::userID),
      sizeof(ArchiveMemberHeader::userID),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("User ID")),

   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::groupID),
      sizeof(ArchiveMemberHeader::groupID),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Group ID")),

   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::mode),
      sizeof(ArchiveMemberHeader::mode),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("File mode")),

   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::sizeText),
      sizeof(ArchiveMemberHeader::sizeText),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Size")),

   StructField(
      offsetof(ArchiveMemberHeader, ArchiveMemberHeader::endOfHeader),
      sizeof(ArchiveMemberHeader::endOfHeader),
      1,
      true, // little-endian
      StructFieldType::byteArray,
      _T("End of header")),
   });
