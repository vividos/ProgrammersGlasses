//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SectionHeader.cpp
/// \brief Section header definition
//
#include "stdafx.h"
#include "SectionHeader.hpp"
#include "DisplayFormatHelper.hpp"


/// section flags bits to display text mapping
std::map<DWORD, LPCTSTR> g_mapSectionFlagsBitsToDisplayText =
{
   { 0x00000000, _T("Reserved (IMAGE_SCN_TYPE_REG)") },
   { 0x00000001, _T("Reserved bit 0 (IMAGE_SCN_TYPE_DSECT)") },
   { 0x00000002, _T("Reserved bit 1 (IMAGE_SCN_TYPE_NOLOAD)") },
   { 0x00000004, _T("Reserved bit 2 (IMAGE_SCN_TYPE_GROUP)") },
   { 0x00000008, _T("IMAGE_SCN_TYPE_NO_PAD") },
   { 0x00000010, _T("Reserved bit 4 (IMAGE_SCN_TYPE_COPY)") },
   { 0x00000020, _T("IMAGE_SCN_CNT_CODE") },
   { 0x00000040, _T("IMAGE_SCN_CNT_INITIALIZED_DATA") },
   { 0x00000080, _T("IMAGE_SCN_CNT_UNINITIALIZED_DATA") },
   { 0x00000100, _T("IMAGE_SCN_LNK_OTHER (reserved)") },
   { 0x00000200, _T("IMAGE_SCN_LNK_INFO") },
   { 0x00000400, _T("Reserved bit 10 (IMAGE_SCN_TYPE_OVER)") },
   { 0x00000800, _T("IMAGE_SCN_LNK_REMOVE") },
   { 0x00001000, _T("IMAGE_SCN_LNK_COMDAT") },
   { 0x00002000, _T("Reserved bit 13") },
   { 0x00004000, _T("IMAGE_SCN_NO_DEFER_SPEC_EXC (Obsolete IMAGE_SCN_MEM_PROTECTED)") },
   { 0x00008000, _T("IMAGE_SCN_GPREL") },
   { 0x00010000, _T("Obsolete IMAGE_SCN_MEM_SYSHEAP") },
   { 0x00020000, _T("Reserved bit 17 (IMAGE_SCN_MEM_PURGEABLE or IMAGE_SCN_MEM_16BIT)") },
   { 0x00040000, _T("IMAGE_SCN_MEM_LOCKED") },
   { 0x00080000, _T("IMAGE_SCN_MEM_PRELOAD") },
   // alignment flags omitted, since they are not flags but enum values
   { 0x01000000, _T("IMAGE_SCN_LNK_NRELOC_OVFL") },
   { 0x02000000, _T("IMAGE_SCN_MEM_DISCARDABLE") },
   { 0x04000000, _T("IMAGE_SCN_MEM_NOT_CACHED") },
   { 0x08000000, _T("IMAGE_SCN_MEM_NOT_PAGED") },
   { 0x10000000, _T("IMAGE_SCN_MEM_SHARED") },
   { 0x20000000, _T("IMAGE_SCN_MEM_EXECUTE") },
   { 0x40000000, _T("IMAGE_SCN_MEM_READ") },
   { 0x80000000, _T("IMAGE_SCN_MEM_WRITE") },
};

StructDefinition g_definitionSectionHeader = StructDefinition({
   StructField(
      offsetof(SectionHeader, SectionHeader::name),
      sizeof(SectionHeader::name),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Section name")),

   StructField(
      offsetof(SectionHeader, SectionHeader::virtualSize),
      sizeof(SectionHeader::virtualSize),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Virtual Size")),

   StructField(
      offsetof(SectionHeader, SectionHeader::virtualAddress),
      sizeof(SectionHeader::virtualAddress),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Virtual Address")),

   StructField(
      offsetof(SectionHeader, SectionHeader::sizeOfRawData),
      sizeof(SectionHeader::sizeOfRawData),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Size of raw data")),

   StructField(
      offsetof(SectionHeader, SectionHeader::pointerToRawData),
      sizeof(SectionHeader::pointerToRawData),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("File offset to raw data")),

   StructField(
      offsetof(SectionHeader, SectionHeader::pointerToRelocations),
      sizeof(SectionHeader::pointerToRelocations),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("File offset to relocations")),

   StructField(
      offsetof(SectionHeader, SectionHeader::pointerToLinenumbers),
      sizeof(SectionHeader::pointerToLinenumbers),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("File offset to line numbers")),

   StructField(
      offsetof(SectionHeader, SectionHeader::numberOfRelocations),
      sizeof(SectionHeader::numberOfRelocations),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of relocations")),

   StructField(
      offsetof(SectionHeader, SectionHeader::numberOfLinenumbers),
      sizeof(SectionHeader::numberOfLinenumbers),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of line numbers")),

   StructField(
      offsetof(SectionHeader, SectionHeader::sectionFlags),
      sizeof(SectionHeader::sectionFlags),
      4,
      true, // little-endian
      StructFieldType::flagsMapping,
      g_mapSectionFlagsBitsToDisplayText,
      _T("Section flags")),
   });
