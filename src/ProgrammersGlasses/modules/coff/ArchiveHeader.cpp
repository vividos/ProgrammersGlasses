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

StructDefinition g_definitionArchiveHeader = StructDefinition({
   StructField(
      offsetof(ArchiveHeader, ArchiveHeader::signature),
      sizeof(ArchiveHeader::signature),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Archive signature")),
   });
