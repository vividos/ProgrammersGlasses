//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PngHeader.cpp
/// \brief PNG file related header structs
//
#include "stdafx.h"
#include "PngHeader.hpp"

extern const BYTE c_pngFileHeaderSignatureBytes[8] =
{
   137, 80, 78, 71, 13, 10, 26, 10
};

StructDefinition g_definitionPngFileHeader = StructDefinition({
   StructField(
      offsetof(PngFileHeader, PngFileHeader::signature),
      sizeof(PngFileHeader::signature),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("PNG signature")),
   });

StructDefinition g_definitionPngChunkHeader = StructDefinition({
   StructField(
      offsetof(PngChunkHeader, PngChunkHeader::length),
      sizeof(PngChunkHeader::length),
      4,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Chunk data length")),

   StructField(
      offsetof(PngChunkHeader, PngChunkHeader::chunkType),
      sizeof(PngChunkHeader::chunkType),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Chunk type")),
   });

std::map<DWORD, LPCTSTR> g_mapPngColourTypeToDisplayText =
{
   { 0, _T("Greyscale")},
   { 2, _T("True colour")},
   { 3, _T("Indexed colour")},
   { 4, _T("Greyscale with alpha")},
   { 6, _T("True color with alpha")},
};

std::map<DWORD, LPCTSTR> g_mapPngCompressionMethodToDisplayText =
{
   { 0, _T("deflate")},
};

std::map<DWORD, LPCTSTR> g_mapPngFilterMethodToDisplayText =
{
   { 0, _T("adaptive filtering")},
};

std::map<DWORD, LPCTSTR> g_mapPngInterlaceMethodToDisplayText =
{
   { 0, _T("no interlace")},
   { 1, _T("Adam7 interlace")},
};

StructDefinition g_definitionPngImageHeader = StructDefinition({
   StructField(
      offsetof(PngImageHeader, PngImageHeader::width),
      sizeof(PngImageHeader::width),
      4,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Image width")),

   StructField(
      offsetof(PngImageHeader, PngImageHeader::height),
      sizeof(PngImageHeader::height),
      4,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Image height")),

   StructField(
      offsetof(PngImageHeader, PngImageHeader::bitDepth),
      sizeof(PngImageHeader::bitDepth),
      1,
      true,
      StructFieldType::unsignedInteger,
      _T("Bit depth per sample")),

   StructField(
      offsetof(PngImageHeader, PngImageHeader::colourType),
      sizeof(PngImageHeader::colourType),
      1,
      true,
      StructFieldType::valueMapping,
      g_mapPngColourTypeToDisplayText,
      _T("Colour type")),

   StructField(
      offsetof(PngImageHeader, PngImageHeader::compressionMethod),
      sizeof(PngImageHeader::compressionMethod),
      1,
      true,
      StructFieldType::valueMapping,
      g_mapPngCompressionMethodToDisplayText,
      _T("Compression method")),

   StructField(
      offsetof(PngImageHeader, PngImageHeader::filterMethod),
      sizeof(PngImageHeader::filterMethod),
      1,
      true,
      StructFieldType::valueMapping,
      g_mapPngFilterMethodToDisplayText,
      _T("Filter method")),

   StructField(
      offsetof(PngImageHeader, PngImageHeader::interlaceMethod),
      sizeof(PngImageHeader::interlaceMethod),
      1,
      true,
      StructFieldType::valueMapping,
      g_mapPngInterlaceMethodToDisplayText,
      _T("Interlace method")),
   });
