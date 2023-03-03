//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CoffObjectNodeTreeBuilder.cpp
/// \brief Node tree builder for for COFF object files
//
#include "stdafx.h"
#include "CoffObjectNodeTreeBuilder.hpp"
#include "CodeTextViewNode.hpp"
#include "FilterSortListViewNode.hpp"
#include "CoffHeader.hpp"
#include "SectionHeader.hpp"
#include "CoffSymbolTable.hpp"
#include "DisplayFormatHelper.hpp"
#include "SymbolsHelper.hpp"
#include "StringListIterator.hpp"
#include "StructListViewNode.hpp"

CoffObjectNodeTreeBuilder::CoffObjectNodeTreeBuilder(
   const File& file, size_t fileOffset, bool isImage)
:m_file(file),
m_fileOffset(fileOffset),
m_coffObjectHeader(*m_file.Data<CoffHeader>(fileOffset)),
m_isImage(isImage)
{
}

std::shared_ptr<INode> CoffObjectNodeTreeBuilder::BuildCoffObjectNode()
{
   auto coffObjectSummaryNode = std::make_shared<CodeTextViewNode>(
      _T("COFF Summary"),
      NodeTreeIconID::nodeTreeIconLibrary);

   AddCoffObjectFile(*coffObjectSummaryNode);

   coffObjectSummaryNode->SetText(m_objectFileSummary);

   return coffObjectSummaryNode;
}

void CoffObjectNodeTreeBuilder::AddCoffObjectFile(
   CodeTextViewNode& coffSummaryNode)
{
   m_objectFileSummary = _T("COFF object file\n");

   m_objectFileSummary += CString{ _T("Architecture: ") } +
      GetValueFromMapOrDefault<DWORD>(
         g_mapCoffTargetMachineToDisplayText,
         (DWORD)m_coffObjectHeader.targetMachine,
         _T("unknown"));
   m_objectFileSummary += _T("\n");

   auto coffHeaderNode = std::make_shared<StructListViewNode>(
      _T("COFF header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionCoffHeader,
      &m_coffObjectHeader,
      m_file.Data());

   coffSummaryNode.ChildNodes().push_back(coffHeaderNode);

   auto sectionSummaryNode = std::make_shared<CodeTextViewNode>(
      _T("Section Table"),
      NodeTreeIconID::nodeTreeIconDocument);

   AddSectionTable(*sectionSummaryNode);
   coffSummaryNode.ChildNodes().push_back(sectionSummaryNode);

   if (m_coffObjectHeader.offsetSymbolTable != 0 &&
      m_coffObjectHeader.numberOfSymbols != 0)
   {
      LoadStringTable();

      AddSymbolTable(coffSummaryNode);
      AddStringTable(coffSummaryNode);
   }

   AddCoffHeaderSummaryText(coffSummaryNode);
}

void CoffObjectNodeTreeBuilder::AddCoffHeaderSummaryText(
   CodeTextViewNode& node) const
{
   CString text;

   const CoffHeader& header = m_coffObjectHeader;

   text.Append(_T("COFF file: ") + m_file.Filename());

   if (m_fileOffset > 0)
      text.AppendFormat(_T(" at offset 0x%08zx"), m_fileOffset);

   text.Append(_T("\n\n"));
   text.Append(_T("COFF Header\n"));

   text.AppendFormat(_T("Target machine: %s (0x%04x)\n"),
      GetValueFromMapOrDefault<DWORD>(
         g_mapCoffTargetMachineToDisplayText,
         (DWORD)m_coffObjectHeader.targetMachine,
         _T("unknown")),
      m_coffObjectHeader.targetMachine);

   text.AppendFormat(_T("Number of sections: %u\n"), header.numberOfSections);

   CString time = DisplayFormatHelper::FormatDateTime(header.timeStamp);
   text += time.IsEmpty()
      ? _T("Warning: Error while formatting date/time")
      : _T("Creation date/time: ") + time + _T("\n");

   text.AppendFormat(_T("Symbol table offset: 0x%08x\n"), header.offsetSymbolTable);
   text.AppendFormat(_T("Symbol table length: %u\n"), header.numberOfSymbols);
   if (m_isImage &&
      (header.offsetSymbolTable != 0 ||
         header.numberOfSymbols != 0))
      text.Append(_T("Warning: COFF symbol table for images is deprecated\n"));

   text.AppendFormat(_T("Optional header size: %u\n"), header.optionalHeaderSize);

   text.AppendFormat(_T("Characteristics flags: 0x%08x (%s)\n"),
      header.characteristicsFlags,
      DisplayFormatHelper::FormatBitFlagsFromMap(
         g_mapCoffCharacteristicsBitsToDisplayText,
         header.characteristicsFlags).GetString());

   text += _T("\n") + m_objectFileSummary;

   node.SetText(text);
}

void CoffObjectNodeTreeBuilder::AddSectionTable(
   CodeTextViewNode& sectionSummaryNode) const
{
   if (sizeof(m_coffObjectHeader) + m_coffObjectHeader.optionalHeaderSize + sizeof(SectionHeader) >= m_file.Size())
   {
      sectionSummaryNode.SetText(_T("Error: section header offset is outside of the file size!"));
      return;
   }

   CString summaryText;

   size_t maxSectionCount = static_cast<size_t>(m_coffObjectHeader.numberOfSections);
   summaryText.Format(_T("Number of sections: %zu\n"), maxSectionCount);

   const BYTE* data = m_file.Data<BYTE>(m_fileOffset);
   for (size_t sectionIndex = 0; sectionIndex < maxSectionCount; sectionIndex++)
   {
      const BYTE* sectionStart =
         data +
         sizeof(m_coffObjectHeader) +
         m_coffObjectHeader.optionalHeaderSize +
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

void CoffObjectNodeTreeBuilder::LoadStringTable()
{
   size_t stringTableOffset =
      m_fileOffset +
      m_coffObjectHeader.offsetSymbolTable +
      m_coffObjectHeader.numberOfSymbols * sizeof(CoffSymbolTable);

   const DWORD* stringTableStart = m_file.Data<DWORD>(stringTableOffset);
   DWORD stringTableLength = *stringTableStart;

   stringTableOffset += 4;

   StringListIterator iter{
      m_file,
      stringTableOffset,
      stringTableLength - 4,
      false };

   for (DWORD stringTableIndex = 0;
      stringTableIndex < stringTableLength && !iter.IsAtEnd();
      stringTableIndex++, iter.Next())
   {
      size_t offset = iter.Offset() - stringTableOffset + 4;
      CString text = iter.Current();

      m_offsetToStringMapping.insert(
         std::make_pair(offset, text));
   }
}

void CoffObjectNodeTreeBuilder::AddSymbolTable(
   StaticNode& coffSummaryNode)
{
   if (sizeof(m_coffObjectHeader) + m_coffObjectHeader.offsetSymbolTable + sizeof(CoffSymbolTable) >= m_file.Size())
   {
      m_objectFileSummary += _T("Error: COFF symbol table offset is outside of the file size!\n");
      return;
   }

   std::vector<std::vector<CString>> symbolTableData;
   std::vector<std::shared_ptr<INode>> symbolChildNodes;

   const BYTE* data = m_file.Data<BYTE>(m_fileOffset);
   const BYTE* symbolTableStart =
      data +
      m_coffObjectHeader.offsetSymbolTable;

   size_t maxSymbolTableEntries = m_coffObjectHeader.numberOfSymbols;

   const BYTE* symbolTableCurrent = symbolTableStart;
   for (size_t symbolTableEntry = 0; symbolTableEntry < maxSymbolTableEntries; symbolTableEntry++)
   {
      if (!m_file.IsValidRange(symbolTableCurrent, sizeof(CoffSymbolTable)))
      {
         m_objectFileSummary.AppendFormat(_T("Warning: File ended while scanning the symbol table\n"));
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

         auto iter = m_offsetToStringMapping.find(offset);
         if (iter != m_offsetToStringMapping.end())
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

   m_objectFileSummary.AppendFormat(
      _T("Symbol table with %u entries, length 0x%08zx bytes.\n"),
      m_coffObjectHeader.numberOfSymbols, symbolTableLength);

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

void CoffObjectNodeTreeBuilder::AddStringTable(StaticNode& coffSummaryNode)
{
   size_t stringTableOffset =
      m_fileOffset +
      m_coffObjectHeader.offsetSymbolTable +
      m_coffObjectHeader.numberOfSymbols * sizeof(CoffSymbolTable);

   const DWORD* stringTableStart = m_file.Data<DWORD>(stringTableOffset);
   DWORD stringTableLength = *stringTableStart;

   std::vector<std::vector<CString>> stringTableData;

   DWORD stringTableIndex = 0;
   for (const auto& pair : m_offsetToStringMapping)
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

   m_objectFileSummary.AppendFormat(_T("String table with %u entries, length 0x%08x bytes.\n"),
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
