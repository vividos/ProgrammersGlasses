//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SidFileReader.cpp
/// \brief reader for SID files
//
#include "stdafx.h"
#include "SidFileReader.hpp"
#include "SidFileHeader.hpp"
#include "modules/CodeTextViewNode.hpp"
#include "modules/StructListViewNode.hpp"

bool SidFileReader::IsSidFile(const File& file)
{
   if (file.Size() < sizeof(SidFileHeader))
      return false;

   const SidFileHeader& header = *reinterpret_cast<const SidFileHeader*>(file.Data());
   return
      memcmp(header.magicId, "PSID", 4) == 0 ||
      memcmp(header.magicId, "RSID", 4) == 0;
}

void SidFileReader::Load()
{
   if (!m_file.IsAvail())
      return;

   auto rootNode = std::make_shared<CodeTextViewNode>(_T("Summary"), NodeTreeIconID::nodeTreeIconDocument);

   const SidFileHeader& header = *reinterpret_cast<const SidFileHeader*>(m_file.Data());
   AddSummaryText(header, rootNode);

   WORD version = SwapEndianness(header.version);

   const StructDefinition& structDefinition =
      version >= 2 ? g_definitionSidV2FileHeader : g_definitionSidFileHeader;

   auto sidFileHeaderNode = std::make_shared<StructListViewNode>(
      _T("SID file header"),
      NodeTreeIconID::nodeTreeIconBinary,
      structDefinition,
      &header,
      m_file.Data());

   rootNode->ChildNodes().push_back(sidFileHeaderNode);

   m_rootNode = rootNode;
}

void SidFileReader::AddSummaryText(const SidFileHeader& header,
   std::shared_ptr<CodeTextViewNode> summaryNode) const
{
   CString text;

   CString name{ header.name, 32 };
   CString author{ header.author, 32 };
   CString released{ header.released, 32 };

   text.Append(_T("SID file: ") + m_file.Filename() + _T("\n\n"));
   text.AppendFormat(_T("Title: %s\n"), name.GetString());
   text.AppendFormat(_T("Author: %s\n"), author.GetString());
   text.AppendFormat(_T("Released: %s\n"), released.GetString());
   text.Append(_T("\n"));

   WORD effectiveLoadAddress = SwapEndianness(header.loadAddress);
   if (effectiveLoadAddress == 0)
   {
      WORD dataOffset = SwapEndianness(header.dataOffset);

      const BYTE* data = reinterpret_cast<const BYTE*>(m_file.Data());

      // the load address at the data offset is stored in little-endian
      effectiveLoadAddress = (WORD(data[dataOffset + 1]) << 8) | data[dataOffset];
   }

   WORD effectiveInitAddress = SwapEndianness(header.initAddress);
   if (effectiveInitAddress == 0)
      effectiveInitAddress = effectiveLoadAddress;

   text.AppendFormat(_T("Load Address: $%04x    Number of tunes: %u\n"), effectiveLoadAddress, SwapEndianness(header.numSongs));
   text.AppendFormat(_T("Init Address: $%04x    Default tune: %u\n"), effectiveInitAddress, SwapEndianness(header.startSong));
   text.AppendFormat(_T("Play Address: $%04x    Speed: $%08x\n"), SwapEndianness(header.playAddress), SwapEndianness(header.speed));
   text.Append(_T("\n"));

   WORD version = SwapEndianness(header.version);
   WORD flags = version >= 2 ? SwapEndianness(header.flags) : 0;
   BYTE sidModel = (flags >> 4) & 3;

   LPCTSTR sidModelText =
      sidModel == 1 ? _T("MOS6581") :
      sidModel == 2 ? _T("MOS8580") :
      sidModel == 3 ? _T("MOS6581") : _T("Unknown");

   BYTE clock = (flags >> 2) & 3;
   LPCTSTR clockText =
      clock == 1 ? _T("PAL") :
      clock == 2 ? _T("NTSC") :
      clock == 3 ? _T("PAL/NTSC") : _T("Unknown");

   const DWORD* data32 = reinterpret_cast<const DWORD*>(m_file.Data());

   LPCTSTR fileFormat = GetValueFromMapOrDefault<DWORD>(
      g_mapSidMagicIdsToDisplayText, SwapEndianness(data32[0]), _T("invalid"));

   text.AppendFormat(_T("SID Model:    %s  Clock: %s\n"), sidModelText, clockText);
   text.AppendFormat(_T("File Format:  %04s     Format version: %u\n"), fileFormat, SwapEndianness(header.version));

   CString warnings;

   if (version < 1 || version > 4)
      warnings.AppendFormat(_T("Warning: Unknown SID version\n"));

   bool isRsid = memcmp(header.magicId, "RSID", 4) == 0;

   if (isRsid &&
      (version < 2 || version > 4))
      warnings.AppendFormat(_T("Warning: RSID format, but version is not between 2 and 4\n"));

   if (isRsid &&
      (header.loadAddress != 0 || header.playAddress != 0 || header.speed != 0))
      warnings.AppendFormat(_T("Warning: RSID format, but load adress, play address or speed isn't 0\n"));

   WORD dataOffset = SwapEndianness(header.dataOffset);

   if ((version == 1 && dataOffset != 0x0076) ||
      ((version == 2 || version == 3 || version == 4) && dataOffset != 0x007C))
      warnings.AppendFormat(_T("Warning: Invalid data offset\n"));

   if (isRsid &&
      effectiveLoadAddress < 0x07E8)
      warnings.AppendFormat(_T("Warning: Load address for RSID file is below $07E8\n"));

   if (isRsid &&
      effectiveInitAddress < 0x07E8 ||
      (effectiveInitAddress >= 0xA000 && effectiveInitAddress <= 0xBFFF))
      warnings.AppendFormat(_T("Warning: Init address for RSID file is below $07E8 or in Basic ROM\n"));

   if (!warnings.IsEmpty())
   {
      text.Append(_T("\n"));
      text.Append(warnings);
   }

   summaryNode->SetText(text);
}
