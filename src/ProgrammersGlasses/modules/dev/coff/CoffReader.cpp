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
#include "CoffObjectNodeTreeBuilder.hpp"
#include "NonCoffObjectNodeTreeBuilder.hpp"
#include "ArchiveHeader.hpp"
#include "File.hpp"
#include "Helper.hpp"
#include "DisplayFormatHelper.hpp"
#include "SymbolsHelper.hpp"
#include "StringListIterator.hpp"
#include "StructListViewNode.hpp"

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
   else if (NonCoffObjectNodeTreeBuilder::IsNonCoffOrAnonymousObjectFile(m_file, 0))
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
   CoffObjectNodeTreeBuilder nodeTreeBuilder{ m_file, 0, false };
   m_rootNode = nodeTreeBuilder.BuildCoffObjectNode();
}

void CoffReader::LoadNonCoffObjectFile()
{
   NonCoffObjectNodeTreeBuilder nodeTreeBuilder{ m_file, 0 };
   m_rootNode = nodeTreeBuilder.BuildNonCoffObjectNode();
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

void CoffReader::AddArchiveLongnamesMember(StaticNode& archiveMemberNode,
   size_t fileOffset, size_t linkerMemberSize,
   CString& linkerMemberSummary) const
{
   std::vector<std::vector<CString>> longnamesMemberSymbolsListData;

   StringListIterator iter{
      m_file,
      fileOffset,
      linkerMemberSize,
      false };


   DWORD numSymbols = 0;

   for (; !iter.IsAtEnd(); iter.Next(), numSymbols++)
   {
      CString stringIndexText;
      stringIndexText.Format(_T("%u"), numSymbols);

      size_t offset = iter.Offset() - fileOffset;

      CString stringOffsetText;
      stringOffsetText.Format(_T("/%zu"), offset);

      CString stringTableText = iter.Current();

      longnamesMemberSymbolsListData.push_back(
         std::vector<CString> {
         stringIndexText,
            stringOffsetText,
            stringTableText,
      });
   }

   static std::vector<CString> longnamesMemberSymbolsListColumnNames
   {
      _T("Index"),
      _T("Offset"),
      _T("String"),
   };

   auto longnamesMemberSymbolsNode = std::make_shared<FilterSortListViewNode>(
      _T("Longnames Member Strings"),
      NodeTreeIconID::nodeTreeIconTable,
      longnamesMemberSymbolsListColumnNames,
      longnamesMemberSymbolsListData,
      true);

   archiveMemberNode.ChildNodes().push_back(longnamesMemberSymbolsNode);

   linkerMemberSummary.AppendFormat(
      _T("Longnames member, containing %u strings"),
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
         if (NonCoffObjectNodeTreeBuilder::IsNonCoffOrAnonymousObjectFile(m_file, archiveMemberStart))
         {
            NonCoffObjectNodeTreeBuilder nodeTreeBuilder{ m_file, archiveMemberStart };
            auto nonCoffSummaryNode = nodeTreeBuilder.BuildNonCoffObjectNode();
            CString objectFileSummary = nodeTreeBuilder.GetObjectFileSummary();

            archiveMemberSummaryText += objectFileSummary;

            IndentText(objectFileSummary, 3);
            objectFileSummary.TrimLeft();
            librarySummaryText += objectFileSummary + _T("\n");

            archiveMemberNode->ChildNodes().push_back(nonCoffSummaryNode);
         }
         else if (archiveMemberIndex == 2 && trimmedArchiveMemberName == _T("//"))
         {
            CString linkerMemberSummary;
            AddArchiveLongnamesMember(
               *archiveMemberNode,
               archiveMemberStart,
               archiveMemberSize,
               linkerMemberSummary);

            archiveMemberSummaryText += linkerMemberSummary;

            IndentText(linkerMemberSummary, 3);
            linkerMemberSummary.TrimLeft();
            librarySummaryText += linkerMemberSummary + _T("\n");
         }
         else
         {
            CoffObjectNodeTreeBuilder nodeTreeBuilder{
               m_file, archiveMemberStart, false };

            auto coffSummaryNode = nodeTreeBuilder.BuildCoffObjectNode();
            CString objectFileSummary = nodeTreeBuilder.GetObjectFileSummary();

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
