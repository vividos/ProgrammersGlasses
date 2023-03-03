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
#include "FilterSortListViewNode.hpp"
#include "CoffHeader.hpp"
#include "ImportObjectHeader.hpp"
#include "AnonymousObjectHeader.hpp"
#include "ArchiveHeader.hpp"
#include "SectionHeader.hpp"
#include "CoffSymbolTable.hpp"
#include "File.hpp"
#include "Helper.hpp"
#include "DisplayFormatHelper.hpp"
#include "SymbolsHelper.hpp"
#include "StringListIterator.hpp"
#include "StructListViewNode.hpp"
#include <map>

bool CoffReader::IsCoffObjectFile(const File& file)
{
   // COFF has no magic number, so all we can do is check a few parameters
   // from the header
   if (file.Size() < sizeof(CoffHeader))
      return false;

   const CoffHeader& header = *file.Data<CoffHeader>();

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
      *file.Data<IMPORT_OBJECT_HEADER>(fileOffset);

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
      *file.Data<ArchiveHeader>();

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
   auto rootNode = std::make_shared<CodeTextViewNode>(
      _T("Summary"),
      NodeTreeIconID::nodeTreeIconLibrary);

   CString objectFileSummary;
   AddCoffObjectFile(*rootNode, 0, false, objectFileSummary);

   rootNode->SetText(objectFileSummary);

   m_rootNode = rootNode;
}

void CoffReader::AddCoffObjectFile(CodeTextViewNode& coffSummaryNode,
   size_t fileOffset, bool isImage, CString& objectFileSummary)
{
   objectFileSummary = _T("COFF object file\n");

   const CoffHeader& header = *m_file.Data<CoffHeader>(fileOffset);

   objectFileSummary += CString{ _T("Architecture: ") } +
      GetValueFromMapOrDefault<DWORD>(
         g_mapCoffTargetMachineToDisplayText,
         (DWORD)header.targetMachine,
         _T("unknown"));
   objectFileSummary += _T("\n");

   auto coffHeaderNode = std::make_shared<StructListViewNode>(
      _T("COFF header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionCoffHeader,
      &header,
      m_file.Data());

   coffSummaryNode.ChildNodes().push_back(coffHeaderNode);

   auto sectionSummaryNode = std::make_shared<CodeTextViewNode>(
      _T("Section Table"),
      NodeTreeIconID::nodeTreeIconDocument);

   AddSectionTable(*sectionSummaryNode, header, fileOffset);
   coffSummaryNode.ChildNodes().push_back(sectionSummaryNode);

   if (header.offsetSymbolTable != 0 &&
      header.numberOfSymbols != 0)
   {
      AddSymbolTable(coffSummaryNode, header, fileOffset, objectFileSummary);
      AddStringTable(coffSummaryNode, header, fileOffset, objectFileSummary);
   }

   AddCoffHeaderSummaryText(coffSummaryNode, header, isImage, objectFileSummary);
}

void CoffReader::AddCoffHeaderSummaryText(CodeTextViewNode& node,
   const CoffHeader& header, bool isImage,
   const CString& objectFileSummary) const
{
   CString text;

   size_t fileOffset = (const BYTE*)&header - m_file.Data<BYTE>();

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

   text += _T("\n") + objectFileSummary;

   node.SetText(text);
}

void CoffReader::AddSectionTable(CodeTextViewNode& sectionSummaryNode,
   const CoffHeader& header, size_t fileOffset) const
{
   if (sizeof(header) + header.optionalHeaderSize + sizeof(SectionHeader) >= m_file.Size())
   {
      sectionSummaryNode.SetText(_T("Error: section header offset is outside of the file size!"));
      return;
   }

   CString summaryText;

   size_t maxSectionCount = static_cast<size_t>(header.numberOfSections);
   summaryText.Format(_T("Number of sections: %zu\n"), maxSectionCount);

   const BYTE* data = m_file.Data<BYTE>(fileOffset);
   for (size_t sectionIndex = 0; sectionIndex < maxSectionCount; sectionIndex++)
   {
      const BYTE* sectionStart =
         data +
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
         NodeTreeIconID::nodeTreeIconBinary,
         g_definitionSectionHeader,
         sectionStart,
         m_file.Data());

      sectionSummaryNode.ChildNodes().push_back(sectionHeaderNode);
   }

   sectionSummaryNode.SetText(summaryText);
}

void CoffReader::AddSymbolTable(StaticNode& coffSummaryNode,
   const CoffHeader& header, size_t fileOffset,
   CString& objectFileSummary) const
{
   if (sizeof(header) + header.offsetSymbolTable + sizeof(CoffSymbolTable) >= m_file.Size())
   {
      objectFileSummary += _T("Error: COFF symbol table offset is outside of the file size!\n");
      return;
   }

   std::map<size_t, CString> offsetToStringMapping;
   LoadStringTable(header, fileOffset, offsetToStringMapping);

   std::vector<std::vector<CString>> symbolTableData;
   std::vector<std::shared_ptr<INode>> symbolChildNodes;

   const BYTE* data = m_file.Data<BYTE>(fileOffset);
   const BYTE* symbolTableStart =
      data +
      header.offsetSymbolTable;

   size_t maxSymbolTableEntries = header.numberOfSymbols;

   const BYTE* symbolTableCurrent = symbolTableStart;
   for (size_t symbolTableEntry = 0; symbolTableEntry < maxSymbolTableEntries; symbolTableEntry++)
   {
      if (!m_file.IsValidRange(symbolTableCurrent, sizeof(CoffSymbolTable)))
      {
         objectFileSummary.AppendFormat(_T("Warning: File ended while scanning the symbol table\n"));
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

      CString symbolIndexText;
      symbolIndexText.Format(_T("%zu"), symbolTableEntry);

      symbolTableData.push_back(
         std::vector<CString> {
         symbolIndexText,
            symbolName,
            SymbolsHelper::UndecorateSymbol(symbolName),
      });

      auto symbolTableEntryNode = std::make_shared<StructListViewNode>(
         _T("Symbol table entry ") + symbolName,
         NodeTreeIconID::nodeTreeIconBinary,
         g_definitionCoffSymbolTable,
         symbolTableCurrent,
         m_file.Data());

      symbolChildNodes.push_back(symbolTableEntryNode);

      // advance pointer
      symbolTableCurrent += sizeof(CoffSymbolTable) + (symbolTable.numberOfAuxSymbols * sizeof(CoffSymbolTable));

      // the number of symbols also includes the aux symbols, so also add these to the index
      symbolTableEntry += symbolTable.numberOfAuxSymbols;
   }

   size_t symbolTableLength = symbolTableCurrent - symbolTableStart;

   objectFileSummary.AppendFormat(
      _T("Symbol table with %u entries, length 0x%08zx bytes.\n"),
      header.numberOfSymbols, symbolTableLength);

   static std::vector<CString> symbolTableColumnNames
   {
      _T("Index"),
      _T("Symbol"),
      _T("Undecorated symbol"),
   };

   auto symbolTableNode = std::make_shared<FilterSortListViewNode>(
      _T("Symbol Table"),
      NodeTreeIconID::nodeTreeIconTable,
      symbolTableColumnNames,
      symbolTableData,
      true);

   symbolTableNode->ChildNodes().swap(symbolChildNodes);

   coffSummaryNode.ChildNodes().push_back(symbolTableNode);
}

void CoffReader::LoadStringTable(
   const CoffHeader& header, size_t fileOffset,
   std::map<size_t, CString>& offsetToStringMapping) const
{
   size_t stringTableOffset =
      fileOffset +
      header.offsetSymbolTable +
      header.numberOfSymbols * sizeof(CoffSymbolTable);

   const DWORD* stringTableStart = m_file.Data<DWORD>(stringTableOffset);
   DWORD stringTableLength = *stringTableStart;

   stringTableOffset += 4;

   StringListIterator iter{
      m_file,
      stringTableOffset,
      m_file.Size() - stringTableOffset,
      false };

   for (DWORD stringTableIndex = 0;
      stringTableIndex < stringTableLength && !iter.IsAtEnd();
      stringTableIndex++, iter.Next())
   {
      size_t offset = iter.Offset();
      CString text = iter.Current();

      offsetToStringMapping.insert(
         std::make_pair(offset, text));
   }
}

void CoffReader::AddStringTable(StaticNode& coffSummaryNode,
   const CoffHeader& header, size_t fileOffset, CString& objectFileSummary)
{
   size_t stringTableOffset =
      fileOffset +
      header.offsetSymbolTable +
      header.numberOfSymbols * sizeof(CoffSymbolTable);

   const DWORD* stringTableStart = m_file.Data<DWORD>(stringTableOffset);
   DWORD stringTableLength = *stringTableStart;

   std::map<size_t, CString> offsetToStringMapping;
   LoadStringTable(header, fileOffset, offsetToStringMapping);

   std::vector<std::vector<CString>> stringTableData;

   DWORD stringTableIndex = 0;
   for (const auto& pair : offsetToStringMapping)
   {
      CString stringIndexText;
      stringIndexText.Format(_T("%u"), stringTableIndex);

      CString stringOffsetText;
      stringOffsetText.Format(_T("0x%08zx"), pair.first);

      stringTableData.push_back(
         std::vector<CString> {
         stringIndexText,
            stringOffsetText,
            pair.second,
            SymbolsHelper::UndecorateSymbol(pair.second),
      });

      stringTableIndex++;
   }

   objectFileSummary.AppendFormat(_T("String table with %u entries, length 0x%08x bytes.\n"),
      stringTableIndex, stringTableLength);

   static std::vector<CString> stringTableColumnNames
   {
      _T("Index"),
      _T("Offset"),
      _T("String"),
      _T("Undecorated string"),
   };

   auto stringTableNode = std::make_shared<FilterSortListViewNode>(
      _T("String Table"),
      NodeTreeIconID::nodeTreeIconTable,
      stringTableColumnNames,
      stringTableData,
      true);

   coffSummaryNode.ChildNodes().push_back(stringTableNode);
}

void CoffReader::LoadNonCoffObjectFile()
{
   auto rootNode = std::make_shared<CodeTextViewNode>(
      _T("Summary"),
      NodeTreeIconID::nodeTreeIconObject);

   CString objectFileSummary;
   AddNonCoffObjectFile(*rootNode, 0, objectFileSummary);

   rootNode->SetText(objectFileSummary);

   m_rootNode = rootNode;
}

void CoffReader::AddNonCoffObjectFile(CodeTextViewNode& nonCoffSummaryNode,
   size_t fileOffset, CString& objectFileSummary) const
{
   LPCVOID header = m_file.Data<BYTE>(fileOffset);

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
      objectFileSummary += _T("\n");

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
      objectFileSummary += _T("\n");

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

void CoffReader::AddArchiveLinkerMember(StaticNode& archiveMemberNode,
   size_t archiveMemberIndex, size_t fileOffset, size_t linkerMemberSize,
   CString& linkerMemberSummary) const
{
   if (archiveMemberIndex == 0)
   {
      AddFirstLinkerMemberNode(archiveMemberNode, fileOffset, linkerMemberSize,
         linkerMemberSummary);
   }
   else if (archiveMemberIndex == 1)
   {
      AddSecondLinkerMemberNode(archiveMemberNode, fileOffset, linkerMemberSize,
         linkerMemberSummary);
   }
}

void CoffReader::AddFirstLinkerMemberNode(StaticNode& archiveMemberNode,
   size_t fileOffset, size_t linkerMemberSize,
   CString& linkerMemberSummary) const
{
   const DWORD* firstLinkerMember =
      reinterpret_cast<const DWORD*>(
         m_file.Data<BYTE>(fileOffset));

   if (!m_file.IsValidRange(firstLinkerMember, linkerMemberSize))
   {
      linkerMemberSummary.AppendFormat(
         _T("Error: Linker member size #%zu is outside of the file size!"),
         linkerMemberSize);
      return;
   }

   std::vector<std::vector<CString>> firstArchiveMemberListData;

   DWORD numSymbolsBigEndian = *(firstLinkerMember++);
   DWORD numSymbols = SwapEndianness(numSymbolsBigEndian);

   linkerMemberSummary.AppendFormat(
      _T("First linker member, containing %u symbols"), numSymbols);

   StringListIterator iter{
      m_file,
      fileOffset + numSymbols * 4 + 4,
      linkerMemberSize - numSymbols * 4 - 4,
      false };

   for (DWORD symbolIndex = 0; symbolIndex < numSymbols; symbolIndex++)
   {
      if (iter.IsAtEnd())
      {
         linkerMemberSummary +=
            _T("Error: Symbol table ended before iterating all symbols!");
            break;
      }

      DWORD offsetBigEndian = firstLinkerMember[symbolIndex];
      DWORD offset = SwapEndianness(offsetBigEndian);

      CString symbolIndexText;
      symbolIndexText.Format(_T("%u"), symbolIndex);

      CString symbolOffsetText;
      symbolOffsetText.Format(_T("0x%08x"), offset);

      CString symbolTableText = iter.Current();

      firstArchiveMemberListData.push_back(
         std::vector<CString> {
         symbolIndexText,
            symbolOffsetText,
            symbolTableText,
            SymbolsHelper::UndecorateSymbol(symbolTableText),
      });

      iter.Next();
   }

   static std::vector<CString> firstArchiveMemberListColumnNames
   {
      _T("Index"),
      _T("Offset"),
      _T("Symbol name"),
      _T("Undecorated symbol name"),
   };

   auto firstLinkerMemberSymbolsNode = std::make_shared<FilterSortListViewNode>(
      _T("First Linker Member Symbols"),
      NodeTreeIconID::nodeTreeIconTable,
      firstArchiveMemberListColumnNames,
      firstArchiveMemberListData,
      true);

   archiveMemberNode.ChildNodes().push_back(firstLinkerMemberSymbolsNode);
}

void CoffReader::AddSecondLinkerMemberNode(StaticNode& archiveMemberNode,
   size_t fileOffset, size_t linkerMemberSize,
   CString& linkerMemberSummary) const
{
   const DWORD* secondLinkerMember =
      reinterpret_cast<const DWORD*>(
         m_file.Data<BYTE>(fileOffset));

   if (!m_file.IsValidRange(secondLinkerMember, linkerMemberSize))
   {
      linkerMemberSummary.AppendFormat(
         _T("Error: Linker member size #%zu is outside of the file size!"),
         linkerMemberSize);
      return;
   }

   // member table
   std::vector<std::vector<CString>> secondLinkerMemberTableListData;

   DWORD numMembers = *(secondLinkerMember++);
   const DWORD* memberIndexStart = secondLinkerMember;

   for (DWORD memberIndex = 0; memberIndex < numMembers; memberIndex++)
   {
      CString memberIndexText;
      memberIndexText.Format(_T("%u"), memberIndex);

      CString memberOffsetText;
      memberOffsetText.Format(_T("0x%08x"), memberIndexStart[memberIndex]);

      secondLinkerMemberTableListData.push_back(
         std::vector<CString> {
         memberIndexText,
            memberOffsetText,
      });
   }

   static std::vector<CString> secondLinkerMemberTableListColumnNames
   {
      _T("Index"),
      _T("Member offset"),
   };

   auto secondLinkerMemberTableNode = std::make_shared<FilterSortListViewNode>(
      _T("Second Linker Member Offsets"),
      NodeTreeIconID::nodeTreeIconTable,
      secondLinkerMemberTableListColumnNames,
      secondLinkerMemberTableListData,
      false);

   archiveMemberNode.ChildNodes().push_back(secondLinkerMemberTableNode);

   secondLinkerMember += numMembers;

   // symbol table
   std::vector<std::vector<CString>> secondLinkerMemberSymbolsListData;

   DWORD numSymbols = *(secondLinkerMember++);

   const WORD* mapIndexStart =
      reinterpret_cast<const WORD*>(secondLinkerMember);

   const CHAR* symbolTableText =
      reinterpret_cast<const CHAR*>(
         mapIndexStart + numSymbols);

   for (DWORD symbolIndex = 0; symbolIndex < numSymbols; symbolIndex++)
   {
      WORD mapIndex = mapIndexStart[symbolIndex];

      CString symbolIndexText;
      symbolIndexText.Format(_T("%u"), symbolIndex);

      CString mapIndexText;
      mapIndexText.Format(_T("0x%04x"), mapIndex);

      secondLinkerMemberSymbolsListData.push_back(
         std::vector<CString> {
         symbolIndexText,
            mapIndexText,
            symbolTableText,
            SymbolsHelper::UndecorateSymbol(symbolTableText),
      });

      symbolTableText += strlen(symbolTableText) + 1;
   }

   static std::vector<CString> secondLinkerMemberSymbolsListColumnNames
   {
      _T("Index"),
      _T("Map index"),
      _T("Symbol name"),
      _T("Undecorated symbol name"),
   };

   auto secondLinkerMemberSymbolsNode = std::make_shared<FilterSortListViewNode>(
      _T("Second Linker Member Symbols"),
      NodeTreeIconID::nodeTreeIconTable,
      secondLinkerMemberSymbolsListColumnNames,
      secondLinkerMemberSymbolsListData,
      true);

   archiveMemberNode.ChildNodes().push_back(secondLinkerMemberSymbolsNode);

   linkerMemberSummary.AppendFormat(
      _T("Second linker member, containing %u members and %u symbols"),
      numMembers,
      numSymbols);
}

void CoffReader::LoadArchiveLibraryFile()
{
   auto rootNode = std::make_shared<CodeTextViewNode>(
      _T("Library Summary"),
      NodeTreeIconID::nodeTreeIconLibrary);

   std::vector<std::vector<CString>> libraryArchiveMemberListData;

   const ArchiveHeader& archiveHeader =
      *m_file.Data<ArchiveHeader>();

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
         *m_file.Data<ArchiveMemberHeader>(archiveMemberOffset);

      // add to list view data
      CString archiveMemberName{ archiveMemberHeader.name, sizeof(archiveMemberHeader.name) };
      CString dateText{ archiveMemberHeader.dateText, sizeof(archiveMemberHeader.dateText) };
      CString userIDText{ archiveMemberHeader.userID, sizeof(archiveMemberHeader.userID) };
      CString groupIDText{ archiveMemberHeader.groupID, sizeof(archiveMemberHeader.groupID) };
      CString fileModeText{ archiveMemberHeader.mode, sizeof(archiveMemberHeader.mode) };
      CString sizeText{ archiveMemberHeader.sizeText, sizeof(archiveMemberHeader.sizeText) };

      CString trimmedArchiveMemberName = archiveMemberName;
      trimmedArchiveMemberName.Trim();
      userIDText.Trim();
      groupIDText.Trim();
      fileModeText.Trim();
      sizeText.Trim();

      time_t dateTime = _tcstoul(dateText, nullptr, 10);
      CString formattedDateTime = DisplayFormatHelper::FormatDateTime(dateTime);

      CString archiveMemberIndexText;
      archiveMemberIndexText.Format(_T("%zu"), archiveMemberIndex);

      libraryArchiveMemberListData.push_back(
         std::vector<CString> {
         archiveMemberIndexText,
            trimmedArchiveMemberName,
            formattedDateTime,
            userIDText,
            groupIDText,
            fileModeText,
            sizeText,
      });

      CString archiveMemberSummaryText;
      archiveMemberSummaryText.AppendFormat(
         _T("Archive member [%zu]: \"%s\"\n")
         _T("Date: %s\n")
         _T("User ID: %s\n")
         _T("Group ID: %s\n")
         _T("Mode: %s\n")
         _T("Size: %s\n\n"),
         archiveMemberIndex,
         archiveMemberName.GetString(),
         formattedDateTime.GetString(),
         userIDText.GetString(),
         groupIDText.GetString(),
         fileModeText.GetString(),
         sizeText.GetString());

      // add a node for each archive member
      auto archiveMemberNode = std::make_shared<CodeTextViewNode>(
         _T("Archive member: ") + trimmedArchiveMemberName,
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
         trimmedArchiveMemberName.GetString());

      // add archive member
      size_t archiveMemberStart = archiveMemberOffset + sizeof(ArchiveMemberHeader);
      size_t archiveMemberSize = _ttol(sizeText);

      // note: the first two are the linker members
      if (archiveMemberIndex < 2 && trimmedArchiveMemberName == _T("/"))
      {
         CString linkerMemberSummary;
         AddArchiveLinkerMember(
            *archiveMemberNode,
            archiveMemberIndex,
            archiveMemberStart,
            archiveMemberSize,
            linkerMemberSummary);

         archiveMemberSummaryText += linkerMemberSummary;

         IndentText(linkerMemberSummary, 3);
         linkerMemberSummary.TrimLeft();
         librarySummaryText += linkerMemberSummary + _T("\n");
      }
      else if (archiveMemberIndex >= 2)
      {
         // add COFF object / anonymous object
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
               false,
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
      archiveMemberOffset += sizeof(ArchiveMemberHeader) + archiveMemberSize;

      // ensure 2-byte alignment
      if ((archiveMemberOffset & 1) != 0)
         archiveMemberOffset++;

      librarySummaryText += _T("\n");
   }

   static std::vector<CString> libraryArchiveMemberListColumnNames
   {
      _T("Index"),
      _T("Archive member"),
      _T("Date"),
      _T("User ID"),
      _T("Group ID"),
      _T("Mode"),
      _T("Size"),
   };

   auto libraryArchiveMemberListNode = std::make_shared<FilterSortListViewNode>(
      _T("Library Archive Members"),
      NodeTreeIconID::nodeTreeIconTable,
      libraryArchiveMemberListColumnNames,
      libraryArchiveMemberListData,
      true);

   rootNode->ChildNodes().insert(
      rootNode->ChildNodes().begin() + 1,
      libraryArchiveMemberListNode);

   rootNode->SetText(librarySummaryText);

   m_rootNode = rootNode;
}
