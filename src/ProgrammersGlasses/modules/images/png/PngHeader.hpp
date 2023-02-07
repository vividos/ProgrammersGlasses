//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PngHeader.hpp
/// \brief PNG file related header structs
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// PNG file header
/// \see https://www.w3.org/TR/PNG/
struct PngFileHeader
{
   /// signature bytes
   CHAR signature[8];
};

static_assert(sizeof(PngFileHeader) == 8, "PNG file header must be 8 bytes long");

/// Single chunk header
struct PngChunkHeader
{
   /// length of the chunk, in, bytes, of the data that follows the chunk
   /// header, not counting the following CRC
   DWORD length;

   /// chunk type (or name)
   CHAR chunkType[4];
};

static_assert(sizeof(PngChunkHeader) == 8, "PNG chunk header must be 8 bytes long");

/// PNG IHDR image header
struct PngImageHeader
{
   DWORD width;
   DWORD height;
   BYTE bitDepth;
   BYTE colourType;
   BYTE compressionMethod;
   BYTE filterMethod;
   BYTE interlaceMethod;
};

static_assert(sizeof(PngChunkHeader) == 8, "PNG image header (IHDR) must be 13 bytes long");

#pragma pack(pop)

/// signature bytes for PNG file header (8 bytes long)
extern const BYTE c_pngFileHeaderSignatureBytes[8];

/// structure definition for PNG file header
extern const StructDefinition g_definitionPngFileHeader;

/// structure definition for PNG chunk header
extern const StructDefinition g_definitionPngChunkHeader;

/// structure definition for PNG image header
extern const StructDefinition g_definitionPngImageHeader;

/// @brief mapping of PNG colour type to display text
extern const std::map<DWORD, LPCTSTR> g_mapPngColourTypeToDisplayText;
