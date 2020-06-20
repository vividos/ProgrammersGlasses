//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffReader.cpp
/// \brief reader for COFF format files
//
#include "stdafx.h"
#include "CoffReader.hpp"
#include "CodeTextViewNode.hpp"
#include "CoffHeader.hpp"
#include "ArchiveHeader.hpp"
#include "SectionHeader.hpp"
#include "File.hpp"
#include "DisplayFormatHelper.hpp"
#include "StructListViewNode.hpp"
#include <map>

bool CoffReader::IsCoffObjectFile(const File& file)
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

bool CoffReader::IsNonCoffOrAnonymousObjectFile(const File& file)
{
   if (file.Size() < sizeof(IMPORT_OBJECT_HEADER))
      return false;

   const IMPORT_OBJECT_HEADER& header =
      *reinterpret_cast<const IMPORT_OBJECT_HEADER*>(file.Data());

   return
      header.Sig1 == IMAGE_FILE_MACHINE_UNKNOWN &&
      header.Sig2 == IMPORT_OBJECT_HDR_SIG2 &&
      (header.Version == 0 || header.Version == 1);
}

bool CoffReader::IsArLibraryFile(const File& file)
{
   if (file.Size() < sizeof(ArchiveHeader))
      return false;

   const ArchiveHeader& archiveHeader =
      *reinterpret_cast<const ArchiveHeader*>(file.Data());

   CString signatureText{ archiveHeader.signature, sizeof(archiveHeader.signature) };

   return signatureText == c_archiveHeaderSignatureText;
}

CoffReader::CoffReader(const File& file)
   :m_file(file)
{
}

void CoffReader::Load()
{
   if (IsCoffObjectFile(m_file))
      LoadCoffObjectFile();
   else if (IsNonCoffOrAnonymousObjectFile(m_file))
      LoadNonCoffObjectFile();
   else if (IsArLibraryFile(m_file))
      LoadArchiveLibraryFile();
   else
      ATLASSERT(false);
}

void CoffReader::Cleanup()
{
   // nothing expensive to cleanup here
}

void CoffReader::LoadCoffObjectFile()
{
   const CoffHeader& header = *reinterpret_cast<const CoffHeader*>(m_file.Data());

   auto rootNode = new CodeTextViewNode(_T("Summary"), NodeTreeIconID::nodeTreeIconLibrary);
   AddCoffHeaderSummaryText(*rootNode, header, false);

   auto coffHeaderNode = std::make_shared<StructListViewNode>(
      _T("COFF header"),
      NodeTreeIconID::nodeTreeIconDocument,
      g_definitionCoffHeader,
      &header);

   rootNode->ChildNodes().push_back(coffHeaderNode);

   auto sectionSummaryNode = std::make_shared<CodeTextViewNode>(_T("Section Table"), NodeTreeIconID::nodeTreeIconDocument);
   AddSectionTable(*sectionSummaryNode, header);
   rootNode->ChildNodes().push_back(sectionSummaryNode);

   m_rootNode.reset(rootNode);
}

void CoffReader::AddCoffHeaderSummaryText(CodeTextViewNode& node, const CoffHeader& header, bool isImage)
{
   CString text;

   text.Append(_T("COFF file: ") + m_file.Filename() + _T("\n\n"));
   text.Append(_T("COFF Header\n"));

   text.AppendFormat(_T("Target machine: %s (0x%04x)\n"),
      GetValueFromMapOrDefault<DWORD>(g_mapCoffTargetMachineToDisplayText, (DWORD)header.targetMachine, _T("unknown")),
      header.targetMachine);

   text.AppendFormat(_T("Number of sections: %u\n"), header.numberOfSections);

   CString time = DisplayFormatHelper::FormatDateTime(header.timeStamp);
   text += time.IsEmpty()
      ? _T("Warning: Error while formatting date/time")
      : _T("Creation date/time: ") + time + _T("\n");

   text.AppendFormat(_T("Symbol table offset: 0x%08x\n"), header.offsetSymbolTable);
   text.AppendFormat(_T("Symbol table length: %u\n"), header.numberOfSymbols);
   if (isImage &&
      header.offsetSymbolTable != 0 ||
      header.numberOfSymbols != 0)
      text.Append(_T("Warning: COFF symbol table for images is deprecated\n"));

   text.AppendFormat(_T("Optional header size: %u\n"), header.optionalHeaderSize);

   text.AppendFormat(_T("Characteristics flags: 0x%08x (%s)\n"),
      header.characteristicsFlags,
      DisplayFormatHelper::FormatBitFlagsFromMap(
         g_mapCoffCharacteristicsBitsToDisplayText,
         header.characteristicsFlags).GetString());

   node.SetText(text);
}

void CoffReader::AddSectionTable(CodeTextViewNode& sectionSummaryNode, const CoffHeader& header)
{
   LPCVOID data = m_file.Data();
   if (sizeof(header) + header.optionalHeaderSize + sizeof(SectionHeader) >= m_file.Size())
   {
      sectionSummaryNode.SetText(_T("Error: section header offset is outside of the file size!"));
      return;
   }

   CString summaryText;

   size_t maxSectionCount = static_cast<size_t>(header.numberOfSections);
   summaryText.Format(_T("Number of sections: %zu\n"), maxSectionCount);

   for (size_t sectionIndex = 0; sectionIndex < maxSectionCount; sectionIndex++)
   {
      const BYTE* sectionStart =
         (const BYTE*)data +
         sizeof(header) +
         header.optionalHeaderSize +
         sizeof(SectionHeader) * sectionIndex;

      const SectionHeader& sectionHeader = *reinterpret_cast<const SectionHeader*>(sectionStart);

      CString sectionName{ sectionHeader.name, sizeof(sectionHeader.name) };

      summaryText.AppendFormat(_T("Section %zu: %s at 0x%08x (size 0x%08x)\n"),
         sectionIndex + 1,
         sectionName.GetString(),
         sectionHeader.virtualAddress,
         sectionHeader.virtualSize);

      auto sectionHeaderNode = std::make_shared<StructListViewNode>(
         _T("Section header ") + sectionName,
         NodeTreeIconID::nodeTreeIconDocument,
         g_definitionSectionHeader,
         sectionStart);

      sectionSummaryNode.ChildNodes().push_back(sectionHeaderNode);
   }

   sectionSummaryNode.SetText(summaryText);
}

void CoffReader::LoadNonCoffObjectFile()
{
   // TODO implement
   auto rootNode = new CodeTextViewNode(_T("Summary"), NodeTreeIconID::nodeTreeIconLibrary);
   m_rootNode.reset(rootNode);
}

void CoffReader::LoadArchiveLibraryFile()
{
   auto rootNode = new CodeTextViewNode(_T("Summary"), NodeTreeIconID::nodeTreeIconLibrary);

   const ArchiveHeader& archiveHeader =
      *reinterpret_cast<const ArchiveHeader*>(m_file.Data());

   auto archiveHeaderNode = std::make_shared<StructListViewNode>(
      _T("Archive header"),
      NodeTreeIconID::nodeTreeIconDocument,
      g_definitionArchiveHeader,
      &archiveHeader);

   rootNode->ChildNodes().push_back(archiveHeaderNode);

   // TODO implement loading archive content
   size_t archiveMemberOffset = sizeof(ArchiveHeader);

   for (; archiveMemberOffset < m_file.Size();)
   {
      const ArchiveMemberHeader& archiveMemberHeader =
         *reinterpret_cast<const ArchiveMemberHeader*>(
            (const BYTE*)m_file.Data() + archiveMemberOffset);

      auto archiveMemberNode = std::make_shared<StructListViewNode>(
         _T("Archive member"),
         NodeTreeIconID::nodeTreeIconDocument,
         g_definitionArchiveMemberHeader,
         &archiveMemberHeader);

      rootNode->ChildNodes().push_back(archiveMemberNode);

      if (archiveMemberHeader.endOfHeader[0] != 0x60 &&
         archiveMemberHeader.endOfHeader[1] != 0x0a)
         break;

      CString sizeText{ archiveMemberHeader.sizeText, sizeof(archiveMemberHeader.sizeText) };
      size_t archiveMemberSize = _ttol(sizeText);

      archiveMemberOffset += sizeof(ArchiveMemberHeader) + archiveMemberSize;

      // ensure 2-byte alignment
      if ((archiveMemberOffset & 1) != 0)
         archiveMemberOffset++;
   }

   m_rootNode.reset(rootNode);
}
