//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file DosMzHeader.cpp
/// \brief MS-DOS MZ header struct
//
#include "stdafx.h"
#include "DosMzHeader.hpp"

const StructDefinition g_definitionDosMzHeader = StructDefinition({
   StructField(
      offsetof(DosMzHeader, DosMzHeader::magicNumber),
      sizeof(DosMzHeader::magicNumber),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("MZ magic number")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::bytesInLastBlock),
      sizeof(DosMzHeader::bytesInLastBlock),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Bytes in last block")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::blocksInFile),
      sizeof(DosMzHeader::blocksInFile),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of blocks in file")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::numRelocs),
      sizeof(DosMzHeader::numRelocs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of relocations")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::numHeaderParagraphs),
      sizeof(DosMzHeader::numHeaderParagraphs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of header paragraphs")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::minExtraParagraphs),
      sizeof(DosMzHeader::minExtraParagraphs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Minimum number of extra paragraphs needed")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::maxExtraParagraphs),
      sizeof(DosMzHeader::maxExtraParagraphs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Maximum number of extra paragraphs needed")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialSS),
      sizeof(DosMzHeader::initialSS),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Initial SS register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialSP),
      sizeof(DosMzHeader::initialSP),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Initial SP register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::checksum),
      sizeof(DosMzHeader::checksum),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Checksum")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialIP),
      sizeof(DosMzHeader::initialIP),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Initial IP register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialCS),
      sizeof(DosMzHeader::initialCS),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Initial CS register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::relocTableOffset),
      sizeof(DosMzHeader::relocTableOffset),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Offset to relocation table")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::overlayNumber),
      sizeof(DosMzHeader::overlayNumber),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Overlay number")),
   });
