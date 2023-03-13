//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file NonCoffObjectNodeTreeBuilder.cpp
/// \brief Node tree builder for non-COFF object files
//
#include "stdafx.h"
#include "NonCoffObjectNodeTreeBuilder.hpp"
#include "CodeTextViewNode.hpp"
#include "StructListViewNode.hpp"
#include "CoffHeader.hpp"
#include "ImportObjectHeader.hpp"
#include "AnonymousObjectHeader.hpp"

NonCoffObjectNodeTreeBuilder::NonCoffObjectNodeTreeBuilder(
   const File& file, size_t fileOffset)
   :m_file(file),
   m_fileOffset(fileOffset)
{
}

std::shared_ptr<INode> NonCoffObjectNodeTreeBuilder::BuildNonCoffObjectNode()
{
   auto nonCoffObjectSummaryNode = std::make_shared<CodeTextViewNode>(
      _T("non-COFF Summary"),
      NodeTreeIconID::nodeTreeIconLibrary);

   AddNonCoffObjectFile(*nonCoffObjectSummaryNode);

   nonCoffObjectSummaryNode->SetText(m_objectFileSummary);

   return nonCoffObjectSummaryNode;
}

void NonCoffObjectNodeTreeBuilder::AddNonCoffObjectFile(
   CodeTextViewNode& nonCoffSummaryNode)
{
   // use the shorter IMPORT_OBJECT_HEADER to check the version field
   const ImportObjectHeader& importObjectHeader =
      *m_file.Data<ImportObjectHeader>(m_fileOffset);

   if (importObjectHeader.version == 0)
   {
      nonCoffSummaryNode.SetText(_T("Import object"));

      AddImportObjectNode(nonCoffSummaryNode, importObjectHeader);
   }
   else if (importObjectHeader.version == 1)
   {
      nonCoffSummaryNode.SetText(_T("Anonymous object"));

      const AnonymousObjectHeader& anonymousObjectHeader =
         *m_file.Data<AnonymousObjectHeader>(m_fileOffset);

      AddAnonymousObjectNode(nonCoffSummaryNode, anonymousObjectHeader);
   }
   else
   {
      nonCoffSummaryNode.SetText(_T("Invalid non-COFF object header"));

      m_objectFileSummary = _T("Invalid non-COFF object header");

      // code flow shouldn't arrive here, since IsNonCoffOrAnonymousObjectFile()
      // explicitly checked for the version field.
      ATLASSERT(false);
   }

   nonCoffSummaryNode.SetText(m_objectFileSummary);
}

void NonCoffObjectNodeTreeBuilder::AddImportObjectNode(
   CodeTextViewNode& nonCoffSummaryNode,
   const ImportObjectHeader& importObjectHeader)
{
   m_objectFileSummary = _T("Import object\n");
   m_objectFileSummary += CString{ _T("Architecture: ") } +
      GetValueFromMapOrDefault<DWORD>(
         g_mapCoffTargetMachineToDisplayText,
         (DWORD)importObjectHeader.targetMachine,
         _T("unknown"));
   m_objectFileSummary += _T("\n");

   auto importObjectHeaderNode = std::make_shared<StructListViewNode>(
      _T("Import object header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionImportObjectHeader,
      &importObjectHeader,
      m_file.Data());

   nonCoffSummaryNode.ChildNodes().push_back(importObjectHeaderNode);
}

void NonCoffObjectNodeTreeBuilder::AddAnonymousObjectNode(
   CodeTextViewNode& nonCoffSummaryNode,
   const AnonymousObjectHeader& anonymousObjectHeader)
{

   m_objectFileSummary = _T("Anonymous object\n");
   m_objectFileSummary += CString{ _T("Architecture: ") } +
      GetValueFromMapOrDefault<DWORD>(
         g_mapCoffTargetMachineToDisplayText,
         (DWORD)anonymousObjectHeader.targetMachine,
         _T("unknown"));
   m_objectFileSummary += _T("\n");

   auto anonymousObjectHeaderNode = std::make_shared<StructListViewNode>(
      _T("Anonymous object header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionAnonymousObjectHeader,
      &anonymousObjectHeader,
      m_file.Data());

   nonCoffSummaryNode.ChildNodes().push_back(anonymousObjectHeaderNode);
}
