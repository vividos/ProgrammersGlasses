//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
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
      _T("e_magic: MZ magic number")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::bytesInLastBlock),
      sizeof(DosMzHeader::bytesInLastBlock),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_cblp: Bytes in last page")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::blocksInFile),
      sizeof(DosMzHeader::blocksInFile),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_cp: Number of pages in file")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::numRelocs),
      sizeof(DosMzHeader::numRelocs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_crlc: Number of relocations")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::numHeaderParagraphs),
      sizeof(DosMzHeader::numHeaderParagraphs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_cparhdr: Size of header in paragraphs")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::minExtraParagraphs),
      sizeof(DosMzHeader::minExtraParagraphs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_minalloc: Minimum number of extra paragraphs needed")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::maxExtraParagraphs),
      sizeof(DosMzHeader::maxExtraParagraphs),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_maxalloc: Maximum number of extra paragraphs needed")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialSS),
      sizeof(DosMzHeader::initialSS),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_ss: Initial SS register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialSP),
      sizeof(DosMzHeader::initialSP),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_sp: Initial SP register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::checksum),
      sizeof(DosMzHeader::checksum),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_csum: Checksum")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialIP),
      sizeof(DosMzHeader::initialIP),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_ip: Initial IP register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::initialCS),
      sizeof(DosMzHeader::initialCS),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_cs: Initial CS register")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::relocTableOffset),
      sizeof(DosMzHeader::relocTableOffset),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_lfarlc: Offset to relocation table")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::overlayNumber),
      sizeof(DosMzHeader::overlayNumber),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_ovno: Overlay number")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::reserved),
      sizeof(DosMzHeader::reserved),
      2,
      true, // little-endian
      StructFieldType::byteArray,
      _T("e_res: Reserved")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::oemID),
      sizeof(DosMzHeader::oemID),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_oemid: OEM Identifier")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::oemInfo),
      sizeof(DosMzHeader::oemInfo),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_oeminfo: OEM Info")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::reserved2),
      sizeof(DosMzHeader::reserved2),
      2,
      true, // little-endian
      StructFieldType::byteArray,
      _T("e_res2: Reserved 2")),

   StructField(
      offsetof(DosMzHeader, DosMzHeader::newExecutableHeader),
      sizeof(DosMzHeader::newExecutableHeader),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("e_lfanew: Offset of new exe header")),
   });
