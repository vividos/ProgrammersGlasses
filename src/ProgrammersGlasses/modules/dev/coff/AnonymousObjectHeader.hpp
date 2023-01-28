//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file AnonymousObjectHeader.hpp
/// \brief header definition of anonymous object files
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// \brief Anonymous object header
/// \see https://www.geoffchappell.com/studies/msvc/link/dump/infiles/obj.htm
/// The header corresponds with winnt.h's ANON_OBJECT_HEADER struct.
/// The haeder is followed by TODO what.
struct AnonymousObjectHeader
{
   WORD signature1;        ///< must be IMAGE_FILE_MACHINE_UNKNOWN
   WORD signature2;        ///< must be IMPORT_OBJECT_HDR_SIG2 (0xFFFF)
   WORD version;           ///< version; 1 for anonymous object
   WORD targetMachine;     ///< type of target machine

   DWORD timeStamp;        ///< file creation time stamp

   CLSID classID;          ///< class ID of COM object
   DWORD sizeOfData;       ///< size of data following header
};

#pragma pack(pop)

static_assert(sizeof(AnonymousObjectHeader) == 32,
   "Anonymous object header must be 32 bytes long");

static_assert(sizeof(AnonymousObjectHeader) == sizeof(ANON_OBJECT_HEADER),
   "Anonymous object header must have same size as ANON_OBJECT_HEADER");

/// struct definition for above Anonymous object header
extern StructDefinition g_definitionAnonymousObjectHeader;
