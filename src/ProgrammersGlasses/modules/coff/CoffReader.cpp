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
#include "StructListViewNode.hpp"
#include <map>

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

   auto coffHeaderNode = std::make_shared<StructListViewNode>(
      _T("COFF header"),
      NodeTreeIconID::nodeTreeIconDocument,
      g_definitionCoffHeader,
      &header);

   rootNode->ChildNodes().push_back(coffHeaderNode);

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
      GetValueFromMapOrDefault<DWORD>(g_mapCoffTargetMachineToDisplayText, (DWORD)header.targetMachine, _T("unknown")),
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
