//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CoffReader.cpp
/// \brief reader for COFF format files
//
#include "stdafx.h"
#include "CoffReader.hpp"
#include "CodeTextViewNode.hpp"
#include "CoffHeader.hpp"
#include "ImportObjectHeader.hpp"
#include "AnonymousObjectHeader.hpp"
#include "ArchiveHeader.hpp"
#include "SectionHeader.hpp"
#include "CoffSymbolTable.hpp"
#include "File.hpp"
#include "Helper.hpp"
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

bool CoffReader::IsNonCoffOrAnonymousObjectFile(const File& file, size_t fileOffset)
{
   if (file.Size() < fileOffset + sizeof(IMPORT_OBJECT_HEADER))
      return false;

   const IMPORT_OBJECT_HEADER& header =
      *reinterpret_cast<const IMPORT_OBJECT_HEADER*>(
         (const BYTE*)file.Data() + fileOffset);

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
   else if (IsNonCoffOrAnonymousObjectFile(m_file, 0))
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
   auto rootNode = new CodeTextViewNode(_T("Summary"), NodeTreeIconID::nodeTreeIconLibrary);

   CString objectFileSummary;
   AddCoffObjectFile(*rootNode, 0, objectFileSummary);

   rootNode->SetText(objectFileSummary);

   m_rootNode.reset(rootNode);
}

void CoffReader::AddCoffObjectFile(CodeTextViewNode& coffSummaryNode,
   size_t fileOffset, CString& objectFileSummary)
{
   objectFileSummary = _T("COFF object file\n");

   LPCVOID data = (const BYTE*)m_file.Data() + fileOffset;
   const CoffHeader& header = *reinterpret_cast<const CoffHeader*>(data);

   AddCoffHeaderSummaryText(coffSummaryNode, header, false);

   auto coffHeaderNode = std::make_shared<StructListViewNode>(
      _T("COFF header"),
      NodeTreeIconID::nodeTreeIconDocument,
      g_definitionCoffHeader,
      &header,
      m_file.Data());

   coffSummaryNode.ChildNodes().push_back(coffHeaderNode);

   auto sectionSummaryNode = std::make_shared<CodeTextViewNode>(_T("Section Table"), NodeTreeIconID::nodeTreeIconDocument);
   AddSectionTable(*sectionSummaryNode, header, fileOffset);
   coffSummaryNode.ChildNodes().push_back(sectionSummaryNode);

   if (header.offsetSymbolTable != 0 &&
      header.numberOfSymbols != 0)
   {
      auto symbolTableNode = std::make_shared<CodeTextViewNode>(
         _T("Symbol Table"),
         NodeTreeIconID::nodeTreeIconDocument);

      AddSymbolTable(*symbolTableNode, header, fileOffset);

      coffSummaryNode.ChildNodes().push_back(symbolTableNode);

      auto stringTableNode = std::make_shared<CodeTextViewNode>(
         _T("String Table"),
         NodeTreeIconID::nodeTreeIconDocument);

      AddStringTable(*stringTableNode, header, fileOffset);

      coffSummaryNode.ChildNodes().push_back(stringTableNode);
   }
}

void CoffReader::AddCoffHeaderSummaryText(CodeTextViewNode& node,
   const CoffHeader& header, bool isImage) const
{
   CString text;

   size_t fileOffset = (const BYTE*)&header - (const BYTE*)m_file.Data();

   text.Append(_T("COFF file: ") + m_file.Filename());

   if (fileOffset > 0)
      text.AppendFormat(_T(" at offset 0x%08zx"), fileOffset);

   text.Append(_T("\n\n"));
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
      (header.offsetSymbolTable != 0 ||
         header.numberOfSymbols != 0))
      text.Append(_T("Warning: COFF symbol table for images is deprecated\n"));

   text.AppendFormat(_T("Optional header size: %u\n"), header.optionalHeaderSize);

   text.AppendFormat(_T("Characteristics flags: 0x%08x (%s)\n"),
      header.characteristicsFlags,
      DisplayFormatHelper::FormatBitFlagsFromMap(
         g_mapCoffCharacteristicsBitsToDisplayText,
         header.characteristicsFlags).GetString());

   node.SetText(text);
}

void CoffReader::AddSectionTable(CodeTextViewNode& sectionSummaryNode,
   const CoffHeader& header, size_t fileOffset) const
{
   LPCVOID data = (const BYTE*)m_file.Data() + fileOffset;
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

      if (!m_file.IsValidRange(sectionStart, sizeof(SectionHeader)))
      {
         summaryText.AppendFormat(_T("Error: Section header #%zu is outside of the file size!"), sectionIndex);
         break;
      }

      const SectionHeader& sectionHeader = *reinterpret_cast<const SectionHeader*>(sectionStart);

      CString sectionName{ sectionHeader.name, sizeof(sectionHeader.name) };

      summaryText.AppendFormat(_T("Section %zu: %s at offset 0x%08x (size 0x%08x)\n"),
         sectionIndex + 1,
         sectionName.GetString(),
         sectionHeader.pointerToRawData,
         sectionHeader.sizeOfRawData);

      auto sectionHeaderNode = std::make_shared<StructListViewNode>(
         _T("Section header ") + sectionName,
         NodeTreeIconID::nodeTreeIconDocument,
         g_definitionSectionHeader,
         sectionStart,
         m_file.Data());

      sectionSummaryNode.ChildNodes().push_back(sectionHeaderNode);
   }

   sectionSummaryNode.SetText(summaryText);
}

void CoffReader::AddSymbolTable(CodeTextViewNode& symbolTableSummaryNode,
   const CoffHeader& header, size_t fileOffset) const
{
   LPCVOID data = (const BYTE*)m_file.Data() + fileOffset;
   if (sizeof(header) + header.offsetSymbolTable + sizeof(CoffSymbolTable) >= m_file.Size())
   {
      symbolTableSummaryNode.SetText(_T("Error: COFF symbol table offset is outside of the file size!"));
      return;
   }

   std::map<size_t, CString> offsetToStringMapping;
   LoadStringTable(header, fileOffset, offsetToStringMapping);

   const BYTE* symbolTableStart =
      (const BYTE*)data +
      header.offsetSymbolTable;

   CString summaryText;

   size_t maxSymbolTableEntries = header.numberOfSymbols;

   const BYTE* symbolTableCurrent = symbolTableStart;
   for (size_t symbolTableEntry = 0; symbolTableEntry < maxSymbolTableEntries; symbolTableEntry++)
   {
      if (!m_file.IsValidRange(symbolTableCurrent, sizeof(CoffSymbolTable)))
      {
         summaryText.AppendFormat(_T("Warning: File ended while scanning the symbol table"));
         break;
      }

      const CoffSymbolTable& symbolTable = *reinterpret_cast<const CoffSymbolTable*>(symbolTableCurrent);

      CString symbolName;
      if (symbolTable.name[0] == 0 &&
         symbolTable.name[1] == 0 &&
         symbolTable.name[2] == 0 &&
         symbolTable.name[3] == 0)
      {
         DWORD offset = *reinterpret_cast<const DWORD*>(&symbolTable.name[4]);

         auto iter = offsetToStringMapping.find(offset);
         if (iter != offsetToStringMapping.end())
            symbolName = iter->second;
         else
            symbolName.Format(_T("offset 0x%08x"), offset);
      }
      else
         symbolName = CString{ symbolTable.name, sizeof(symbolTable.name) };

      summaryText.AppendFormat(_T("Symbol: %s\n"), symbolName.GetString());

      auto symbolTableEntryNode = std::make_shared<StructListViewNode>(
         _T("Symbol table entry ") + symbolName,
         NodeTreeIconID::nodeTreeIconDocument,
         g_definitionCoffSymbolTable,
         symbolTableCurrent,
         m_file.Data());

      symbolTableSummaryNode.ChildNodes().push_back(symbolTableEntryNode);

      // advance pointer
      symbolTableCurrent += sizeof(CoffSymbolTable) + (symbolTable.numberOfAuxSymbols * sizeof(CoffSymbolTable));

      // the number of symbols also includes the aux symbols, so also add these to the index
      symbolTableEntry += symbolTable.numberOfAuxSymbols;
   }

   symbolTableSummaryNode.SetText(summaryText);
}

void CoffReader::LoadStringTable(
   const CoffHeader& header, size_t fileOffset,
   std::map<size_t, CString>& offsetToStringMapping) const
{
   LPCVOID data = (const BYTE*)m_file.Data() + fileOffset;

   const BYTE* symbolTableStart =
      (const BYTE*)data +
      header.offsetSymbolTable;

   const BYTE* stringTableStart =
      symbolTableStart + header.numberOfSymbols * sizeof(CoffSymbolTable);

   DWORD stringTableLength = *reinterpret_cast<const DWORD*>(stringTableStart);
   const BYTE* stringTableEnd = stringTableStart + stringTableLength;

   CString summaryText;
   const BYTE* stringTableText = stringTableStart + 4;

   DWORD stringTableIndex = 0;
   for (; stringTableText < stringTableEnd; stringTableIndex++)
   {
      size_t offset = stringTableText - stringTableStart;
      CString text{ stringTableText };

      offsetToStringMapping.insert(
         std::make_pair(offset, text));

      stringTableText += strlen(
         reinterpret_cast<const CHAR*>(stringTableText)) + 1;
   }
}

void CoffReader::AddStringTable(CodeTextViewNode& symbolTableSummaryNode,
   const CoffHeader& header, size_t fileOffset)
{
   LPCVOID data = (const BYTE*)m_file.Data() + fileOffset;

   const BYTE* symbolTableStart =
      (const BYTE*)data +
      header.offsetSymbolTable;

   const BYTE* stringTableStart =
      symbolTableStart + header.numberOfSymbols * sizeof(CoffSymbolTable);

   DWORD stringTableLength = *reinterpret_cast<const DWORD*>(stringTableStart);
   const BYTE* stringTableEnd = stringTableStart + stringTableLength;

   CString summaryText;
   const BYTE* stringTableText = stringTableStart + 4;

   DWORD stringTableIndex = 0;
   for (; stringTableText < stringTableEnd; stringTableIndex++)
   {
      if (!m_file.IsValidRange(stringTableText, stringTableEnd - stringTableText))
      {
         summaryText.AppendFormat(_T("Warning: File ended while scanning the string table"));
         break;
      }

      summaryText.AppendFormat(
         _T("0x%08zx: %hs\n"),
         stringTableText - stringTableStart,
         stringTableText);

      stringTableText += strlen(
         reinterpret_cast<const CHAR*>(stringTableText)) + 1;
   }

   CString headerText;
   headerText.AppendFormat(_T("String table with %u entries, length 0x%08x bytes.\n\n"),
      stringTableIndex, stringTableLength);

   summaryText.Insert(0, headerText);

   symbolTableSummaryNode.SetText(summaryText);
}

void CoffReader::LoadNonCoffObjectFile()
{
   auto rootNode = new CodeTextViewNode(
      _T("Summary"),
      NodeTreeIconID::nodeTreeIconObject);

   CString objectFileSummary;
   AddNonCoffObjectFile(*rootNode, 0, objectFileSummary);

   rootNode->SetText(objectFileSummary);

   m_rootNode.reset(rootNode);
}

void CoffReader::AddNonCoffObjectFile(CodeTextViewNode& nonCoffSummaryNode,
   size_t fileOffset, CString& objectFileSummary) const
{
   LPCVOID header = (const BYTE*)m_file.Data() + fileOffset;

   // use the shorter IMPORT_OBJECT_HEADER to check the version field
   const ImportObjectHeader& importObjectHeader =
      *reinterpret_cast<const ImportObjectHeader*>(header);

   if (importObjectHeader.version == 0)
   {
      nonCoffSummaryNode.SetText(_T("Import object"));

      objectFileSummary = _T("Import object\n");
      objectFileSummary += CString{ _T("Architecture: ") } +
         GetValueFromMapOrDefault<DWORD>(
            g_mapCoffTargetMachineToDisplayText,
            (DWORD)importObjectHeader.targetMachine,
            _T("unknown"));

      auto importObjectHeaderNode = std::make_shared<StructListViewNode>(
         _T("Import object header"),
         NodeTreeIconID::nodeTreeIconBinary,
         g_definitionImportObjectHeader,
         header,
         m_file.Data());

      nonCoffSummaryNode.ChildNodes().push_back(importObjectHeaderNode);
   }
   else if (importObjectHeader.version == 1)
   {
      nonCoffSummaryNode.SetText(_T("Anonymous object"));

      const AnonymousObjectHeader& anonymousObjectHeader =
         *reinterpret_cast<const AnonymousObjectHeader*>(header);

      objectFileSummary = _T("Anonymous object\n");
      objectFileSummary += CString{ _T("Architecture: ") } +
         GetValueFromMapOrDefault<DWORD>(
            g_mapCoffTargetMachineToDisplayText,
            (DWORD)anonymousObjectHeader.targetMachine,
            _T("unknown"));

      auto anonymousObjectHeaderNode = std::make_shared<StructListViewNode>(
         _T("Anonymous object header"),
         NodeTreeIconID::nodeTreeIconBinary,
         g_definitionAnonymousObjectHeader,
         header,
         m_file.Data());

      nonCoffSummaryNode.ChildNodes().push_back(anonymousObjectHeaderNode);
   }
   else
   {
      nonCoffSummaryNode.SetText(_T("Invalid non-COFF object header"));

      objectFileSummary = _T("Invalid non-COFF object header");

      // code flow shouldn't arrive here, since IsNonCoffOrAnonymousObjectFile()
      // explicitly checked for the version field.
      ATLASSERT(false);
   }

   nonCoffSummaryNode.SetText(objectFileSummary);
}

void CoffReader::LoadArchiveLibraryFile()
{
   auto rootNode = new CodeTextViewNode(_T("Library Summary"), NodeTreeIconID::nodeTreeIconLibrary);

   const ArchiveHeader& archiveHeader =
      *reinterpret_cast<const ArchiveHeader*>(m_file.Data());

   auto archiveHeaderNode = std::make_shared<StructListViewNode>(
      _T("Archive header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionArchiveHeader,
      &archiveHeader,
      m_file.Data());

   rootNode->ChildNodes().push_back(archiveHeaderNode);

   CString librarySummaryText;
   librarySummaryText.Append(_T("COFF library file: ") + m_file.Filename());
   librarySummaryText += _T("\n\n");

   size_t archiveMemberOffset = sizeof(ArchiveHeader);

   for (size_t archiveMemberIndex = 0; archiveMemberOffset < m_file.Size(); archiveMemberIndex++)
   {
      const ArchiveMemberHeader& archiveMemberHeader =
         *reinterpret_cast<const ArchiveMemberHeader*>(
            (const BYTE*)m_file.Data() + archiveMemberOffset);

      CString archiveMemberName{ archiveMemberHeader.name, sizeof(archiveMemberHeader.name) };

      CString archiveMemberSummaryText;
      archiveMemberSummaryText.AppendFormat(_T("Archive member [%u]: \"%s\"\n\n"),
         archiveMemberIndex,
         archiveMemberName.GetString());

      auto archiveMemberNode = std::make_shared<CodeTextViewNode>(
         _T("Archive member: ") + archiveMemberName,
         NodeTreeIconID::nodeTreeIconDocument);

      rootNode->ChildNodes().push_back(archiveMemberNode);

      auto archiveMemberHeaderNode = std::make_shared<StructListViewNode>(
         _T("Archive member header"),
         NodeTreeIconID::nodeTreeIconBinary,
         g_definitionArchiveMemberHeader,
         &archiveMemberHeader,
         m_file.Data());

      archiveMemberNode->ChildNodes().push_back(archiveMemberHeaderNode);

      if (archiveMemberHeader.endOfHeader[0] != 0x60 &&
         archiveMemberHeader.endOfHeader[1] != 0x0a)
         break;

      librarySummaryText.AppendFormat(_T("%s: "),
         archiveMemberName.GetString());

      // add COFF object / anonymous object
      size_t archiveMemberStart = archiveMemberOffset + sizeof(ArchiveMemberHeader);

      // note: the first two are the linker members
      if (archiveMemberIndex >= 2)
      {
         if (IsNonCoffOrAnonymousObjectFile(m_file, archiveMemberStart))
         {
            auto nonCoffSummaryNode = std::make_shared<CodeTextViewNode>(
               _T("non-COFF Summary"),
               NodeTreeIconID::nodeTreeIconDocument);

            CString objectFileSummary;
            AddNonCoffObjectFile(
               *nonCoffSummaryNode,
               archiveMemberStart,
               objectFileSummary);

            archiveMemberSummaryText += objectFileSummary;

            IndentText(objectFileSummary, 3);
            objectFileSummary.TrimLeft();
            librarySummaryText += objectFileSummary + _T("\n");

            archiveMemberNode->ChildNodes().push_back(nonCoffSummaryNode);
         }
         else
         {
            auto coffSummaryNode = std::make_shared<CodeTextViewNode>(
               _T("COFF Summary"),
               NodeTreeIconID::nodeTreeIconDocument);

            CString objectFileSummary;
            AddCoffObjectFile(
               *coffSummaryNode,
               archiveMemberStart,
               objectFileSummary);

            archiveMemberSummaryText += objectFileSummary;

            IndentText(objectFileSummary, 3);
            objectFileSummary.TrimLeft();
            librarySummaryText += objectFileSummary + _T("\n");

            archiveMemberNode->ChildNodes().push_back(coffSummaryNode);
         }
      }

      archiveMemberNode->SetText(archiveMemberSummaryText);

      // advance to next header
      CString sizeText{ archiveMemberHeader.sizeText, sizeof(archiveMemberHeader.sizeText) };
      size_t archiveMemberSize = _ttol(sizeText);

      archiveMemberOffset += sizeof(ArchiveMemberHeader) + archiveMemberSize;

      // ensure 2-byte alignment
      if ((archiveMemberOffset & 1) != 0)
         archiveMemberOffset++;

      librarySummaryText += _T("\n");
   }

   rootNode->SetText(librarySummaryText);

   m_rootNode.reset(rootNode);
}
