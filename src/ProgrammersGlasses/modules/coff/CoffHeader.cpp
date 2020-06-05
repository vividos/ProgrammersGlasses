//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffHeader.cpp
/// \brief header definition of COFF files
//
#include "stdafx.h"
#include "CoffHeader.hpp"
#include "DisplayFormatHelper.hpp"

std::map<DWORD, LPCTSTR> g_mapCoffTargetMachineToDisplayText =
{
   { 0x0, _T("IMAGE_FILE_MACHINE_UNKNOWN")},
   { 0x1d3, _T("IMAGE_FILE_MACHINE_AM33 (Matsushita AM33)") },
   { 0x8664, _T("IMAGE_FILE_MACHINE_AMD64 (x64)") },
   { 0x1c0, _T("IMAGE_FILE_MACHINE_ARM (ARM little endian)") },
   { 0x1c4, _T("IMAGE_FILE_MACHINE_ARMV7 (ARMv7(or higher) Thumb mode only)") },
   { 0xebc, _T("IMAGE_FILE_MACHINE_EBC (EFI byte code)") },
   { 0x14c, _T("IMAGE_FILE_MACHINE_I386 (Intel 386 or later processors and compatible processors)") },
   { 0x200, _T("IMAGE_FILE_MACHINE_IA64 (Intel Itanium processor family)") },
   { 0x9041, _T("IMAGE_FILE_MACHINE_M32R (Mitsubishi M32R little endian)") },
   { 0x266, _T("IMAGE_FILE_MACHINE_MIPS16 (MIPS16)") },
   { 0x366, _T("IMAGE_FILE_MACHINE_MIPSFPU (MIPS with FPU)") },
   { 0x466, _T("IMAGE_FILE_MACHINE_MIPSFPU16 (MIPS16 with FPU)") },
   { 0x1f0, _T("IMAGE_FILE_MACHINE_POWERPC (Power PC little endian)") },
   { 0x1f1, _T("IMAGE_FILE_MACHINE_POWERPCFP (Power PC with floating point support)") },
   { 0x166, _T("IMAGE_FILE_MACHINE_R4000 (MIPS little endian)") },
   { 0x1a2, _T("IMAGE_FILE_MACHINE_SH3 (Hitachi SH3)") },
   { 0x1a3, _T("IMAGE_FILE_MACHINE_SH3DSP (Hitachi SH3 DSP)") },
   { 0x1a6, _T("IMAGE_FILE_MACHINE_SH4 (Hitachi SH4)") },
   { 0x1a8, _T("IMAGE_FILE_MACHINE_SH5 (Hitachi SH5)") },
   { 0x1c2, _T("IMAGE_FILE_MACHINE_THUMB (ARM or Thumb(\"interworking\"))") },
   { 0x169, _T("IMAGE_FILE_MACHINE_WCEMIPSV2 (MIPS little - endian WCE v2)") },
};

std::map<DWORD, LPCTSTR> g_mapCoffCharacteristicsBitsToDisplayText =
{
   { 0x00001, _T("IMAGE_FILE_RELOCS_STRIPPED") },
   { 0x00002, _T("IMAGE_FILE_EXECUTABLE_IMAGE") },
   { 0x00004, _T("IMAGE_FILE_LINE_NUMS_STRIPPED") },
   { 0x00008, _T("IMAGE_FILE_LOCAL_SYMS_STRIPPED") },
   { 0x00010, _T("IMAGE_FILE_AGGRESSIVE_WS_TRIM") },
   { 0x00020, _T("IMAGE_FILE_LARGE_ADDRESS_ AWARE") },
   { 0x00080, _T("IMAGE_FILE_BYTES_REVERSED_LO") },
   { 0x00100, _T("IMAGE_FILE_32BIT_MACHINE") },
   { 0x00200, _T("IMAGE_FILE_DEBUG_STRIPPED") },
   { 0x00400, _T("IMAGE_FILE_REMOVABLE_RUN_ FROM_SWAP") },
   { 0x00800, _T("IMAGE_FILE_NET_RUN_FROM_SWAP") },
   { 0x01000, _T("IMAGE_FILE_SYSTEM") },
   { 0x02000, _T("IMAGE_FILE_DLL") },
   { 0x04000, _T("IMAGE_FILE_UP_SYSTEM_ONLY") },
   { 0x08000, _T("IMAGE_FILE_BYTES_REVERSED_HI") },
};

StructDefinition g_definitionCoffHeader = StructDefinition({
   StructField(
      offsetof(CoffHeader, CoffHeader::targetMachine),
      sizeof(CoffHeader::targetMachine),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapCoffTargetMachineToDisplayText,
      _T("Target machine")),

   StructField(
      offsetof(CoffHeader, CoffHeader::numberOfSections),
      sizeof(CoffHeader::numberOfSections),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of sections")),

   StructField(
      offsetof(CoffHeader, CoffHeader::timeStamp),
      sizeof(CoffHeader::timeStamp),
      4,
      true, // little-endian
      [](LPCVOID data, size_t)
      {
         time_t time = *reinterpret_cast<const DWORD*>(data);
         return DisplayFormatHelper::FormatDateTime(time);
      },
      _T("Time stamp")),

   StructField(
      offsetof(CoffHeader, CoffHeader::offsetSymbolTable),
      sizeof(CoffHeader::offsetSymbolTable),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Offset to symbol table")),

   StructField(
      offsetof(CoffHeader, CoffHeader::numberOfSymbols),
      sizeof(CoffHeader::numberOfSymbols),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of symbols")),

   StructField(
      offsetof(CoffHeader, CoffHeader::optionalHeaderSize),
      sizeof(CoffHeader::optionalHeaderSize),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Optional header size")),


   StructField(
      offsetof(CoffHeader, CoffHeader::characteristicsFlags),
      sizeof(CoffHeader::characteristicsFlags),
      2,
      true, // little-endian
      StructFieldType::flagsMapping,
      g_mapCoffCharacteristicsBitsToDisplayText,
      _T("Characteristics flags")),
});
