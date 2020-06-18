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

#pragma pack(pop)

static_assert(sizeof(ArchiveHeader) == 8, "Archive header must be X bytes long");

/// signature text for archive header
extern LPCSTR c_archiveHeaderSignatureText;

/// struct definition for above archive header
extern StructDefinition g_definitionArchiveHeader;
