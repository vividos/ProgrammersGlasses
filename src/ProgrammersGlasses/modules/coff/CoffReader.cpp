//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffReader.cpp
/// \brief reader for COFF format files
//
#include "stdafx.h"
#include "CoffReader.hpp"
#include "CoffNode.hpp"
#include "CoffHeader.hpp"
#include "File.hpp"
#include "DisplayFormatHelper.hpp"
#include <map>

/// mapping of target machine to display text
static std::map<WORD, LPCTSTR> g_mapTargetMachineToDisplayText =
{
   { (WORD)0x0, _T("IMAGE_FILE_MACHINE_UNKNOWN")},
   { (WORD)0x1d3, _T("IMAGE_FILE_MACHINE_AM33 (Matsushita AM33)") },
   { (WORD)0x8664, _T("IMAGE_FILE_MACHINE_AMD64 (x64)") },
   { (WORD)0x1c0, _T("IMAGE_FILE_MACHINE_ARM (ARM little endian)") },
   { (WORD)0x1c4, _T("IMAGE_FILE_MACHINE_ARMV7 (ARMv7(or higher) Thumb mode only)") },
   { (WORD)0xebc, _T("IMAGE_FILE_MACHINE_EBC (EFI byte code)") },
   { (WORD)0x14c, _T("IMAGE_FILE_MACHINE_I386 (Intel 386 or later processors and compatible processors)") },
   { (WORD)0x200, _T("IMAGE_FILE_MACHINE_IA64 (Intel Itanium processor family)") },
   { (WORD)0x9041, _T("IMAGE_FILE_MACHINE_M32R (Mitsubishi M32R little endian)") },
   { (WORD)0x266, _T("IMAGE_FILE_MACHINE_MIPS16 (MIPS16)") },
   { (WORD)0x366, _T("IMAGE_FILE_MACHINE_MIPSFPU (MIPS with FPU)") },
   { (WORD)0x466, _T("IMAGE_FILE_MACHINE_MIPSFPU16 (MIPS16 with FPU)") },
   { (WORD)0x1f0, _T("IMAGE_FILE_MACHINE_POWERPC (Power PC little endian)") },
   { (WORD)0x1f1, _T("IMAGE_FILE_MACHINE_POWERPCFP (Power PC with floating point support)") },
   { (WORD)0x166, _T("IMAGE_FILE_MACHINE_R4000 (MIPS little endian)") },
   { (WORD)0x1a2, _T("IMAGE_FILE_MACHINE_SH3 (Hitachi SH3)") },
   { (WORD)0x1a3, _T("IMAGE_FILE_MACHINE_SH3DSP (Hitachi SH3 DSP)") },
   { (WORD)0x1a6, _T("IMAGE_FILE_MACHINE_SH4 (Hitachi SH4)") },
   { (WORD)0x1a8, _T("IMAGE_FILE_MACHINE_SH5 (Hitachi SH5)") },
   { (WORD)0x1c2, _T("IMAGE_FILE_MACHINE_THUMB (ARM or Thumb(\"interworking\"))") },
   { (WORD)0x169, _T("IMAGE_FILE_MACHINE_WCEMIPSV2 (MIPS little - endian WCE v2)") },
};

/// mapping of COFF characteristics bits to display text
static std::map<DWORD, LPCTSTR> g_mapCoffCharacteristicsBitsToDisplayText =
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

bool CoffReader::IsCoffFileFormat(const File& file)
{
   // COFF has no magic number, so all we can do is check a few parameters
   // from the header
   if (file.Size() < sizeof(CoffHeader))
      return false;

   const CoffHeader& header = *reinterpret_cast<const CoffHeader*>(file.Data());

   if (header.offsetSymbolTable >= file.Size())
      return false;

   // might be a COFF file
   return true;
}

CoffReader::CoffReader(const File& file)
   :m_file(file)
{
}

void CoffReader::Load()
{
   // TODO implement
   auto rootNode = new CoffNode(_T("Summary"), NodeTreeIconID::nodeTreeIconLibrary);

   const CoffHeader& header = *reinterpret_cast<const CoffHeader*>(m_file.Data());

   AddSummaryText(*rootNode, header);

   m_rootNode.reset(rootNode);
}

void CoffReader::Cleanup()
{
   // nothing expensive to cleanup here
}

void CoffReader::AddSummaryText(CoffNode& node, const CoffHeader& header)
{
   CString text;

   text.Append(_T("COFF file: ") + m_file.Filename() + _T("\n\n"));
   text.Append(_T("COFF Header\n"));

   text.AppendFormat(_T("Target machine: %s (0x%04x)\n"),
      GetValueFromMapOrDefault<WORD>(g_mapTargetMachineToDisplayText, header.targetMachine, _T("unknown")),
      header.targetMachine);

   text.AppendFormat(_T("Number of sections: %u\n"), header.numberOfSections);

   CString time = DisplayFormatHelper::FormatDateTime(header.timeStamp);
   text += time.IsEmpty()
      ? _T("Warning: Error while formatting date/time")
      : _T("Creation date/time: ") + time + _T("\n");

   text.AppendFormat(_T("Symbol table offset: 0x%08x\n"), header.offsetSymbolTable);
   text.AppendFormat(_T("Symbol table length: %u\n"), header.numberOfSymbols);
   if (header.offsetSymbolTable != 0 ||
      header.numberOfSymbols != 0)
      text.Append(_T("Warning: COFF symbol table is deprecated\n"));

   text.AppendFormat(_T("Optional header size: %u\n"), header.optionalHeaderSize);

   text.AppendFormat(_T("Characteristics flags: 0x%08x (%s)\n"),
      header.characteristicsFlags,
      DisplayFormatHelper::FormatBitFlagsFromMap(
         g_mapCoffCharacteristicsBitsToDisplayText,
         header.characteristicsFlags).GetString());

   node.SetText(text);
}
