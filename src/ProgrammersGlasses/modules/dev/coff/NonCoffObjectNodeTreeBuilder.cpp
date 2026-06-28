//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file NonCoffObjectNodeTreeBuilder.cpp
/// \brief Node tree builder for non-COFF object files
//
#include "stdafx.h"
#include "NonCoffObjectNodeTreeBuilder.hpp"
#include "CodeTextViewNode.hpp"
#include "StructListViewNode.hpp"
#include "HexDataViewNode.hpp"
#include "CoffHeader.hpp"
#include "ImportObjectHeader.hpp"
#include "AnonymousObjectHeader.hpp"
#include "AnonymousObjectHeaderBigObj.hpp"

bool NonCoffObjectNodeTreeBuilder::IsNonCoffOrAnonymousObjectFile(
   const File& file, size_t fileOffset)
{
   if (file.Size() < fileOffset + sizeof(IMPORT_OBJECT_HEADER))
      return false;

   const IMPORT_OBJECT_HEADER& header =
      *file.Data<IMPORT_OBJECT_HEADER>(fileOffset);

   return
      header.Sig1 == IMAGE_FILE_MACHINE_UNKNOWN &&
      header.Sig2 == IMPORT_OBJECT_HDR_SIG2 &&
      (header.Version == 0 || header.Version == 1 || header.Version == 2);
}

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
   else if (importObjectHeader.version == 2)
   {
      nonCoffSummaryNode.SetText(_T("BigObj anonymous object"));

      const AnonymousObjectHeaderBigObj& bigObjAnonymousObjectHeader =
         *m_file.Data<AnonymousObjectHeaderBigObj>(m_fileOffset);

      AddBigObjAnonymousObjectNode(nonCoffSummaryNode, bigObjAnonymousObjectHeader);
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

   if (importObjectHeader.sizeOfData > 0)
   {
      size_t startOffset = m_fileOffset + sizeof(ImportObjectHeader);

      auto hexDataNode = std::make_shared<HexDataViewNode>(
         _T("Import object raw data"),
         NodeTreeIconID::nodeTreeIconBinary,
         m_file,
         startOffset,
         importObjectHeader.sizeOfData);

      nonCoffSummaryNode.ChildNodes().push_back(hexDataNode);
   }
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

   if (anonymousObjectHeader.sizeOfData)
   {
      size_t startOffset = m_fileOffset + sizeof(AnonymousObjectHeader);

      auto hexDataNode = std::make_shared<HexDataViewNode>(
         _T("Anonyous object raw data"),
         NodeTreeIconID::nodeTreeIconBinary,
         m_file,
         startOffset,
         anonymousObjectHeader.sizeOfData);

      nonCoffSummaryNode.ChildNodes().push_back(hexDataNode);
   }
}

void NonCoffObjectNodeTreeBuilder::AddBigObjAnonymousObjectNode(
   CodeTextViewNode& nonCoffSummaryNode,
   const AnonymousObjectHeaderBigObj& bigObjAnonymousObjectHeader)
{
   m_objectFileSummary = _T("BigObj anonymous object\n");
   m_objectFileSummary += CString{ _T("Architecture: ") } +
      GetValueFromMapOrDefault<DWORD>(
         g_mapCoffTargetMachineToDisplayText,
         (DWORD)bigObjAnonymousObjectHeader.targetMachine,
         _T("unknown"));
   m_objectFileSummary += _T("\n");

   auto bigObjAnonymousObjectHeaderNode = std::make_shared<StructListViewNode>(
      _T("BigObj anonymous object header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionAnonymousObjectHeaderBigObj,
      &bigObjAnonymousObjectHeader,
      m_file.Data());

   nonCoffSummaryNode.ChildNodes().push_back(bigObjAnonymousObjectHeaderNode);

   if (bigObjAnonymousObjectHeader.metadataSize > 0 &&
      bigObjAnonymousObjectHeader.metadataOffset > 0)
   {
      auto hexDataNode = std::make_shared<HexDataViewNode>(
         _T("CLR metadata"),
         NodeTreeIconID::nodeTreeIconBinary,
         m_file,
         bigObjAnonymousObjectHeader.metadataOffset,
         bigObjAnonymousObjectHeader.metadataSize);

      nonCoffSummaryNode.ChildNodes().push_back(hexDataNode);
   }

   if (bigObjAnonymousObjectHeader.sizeOfData > 0)
   {
      size_t startOffset = m_fileOffset + sizeof(AnonymousObjectHeaderBigObj);

      auto hexDataNode = std::make_shared<HexDataViewNode>(
         _T("BigObj anonyous object raw data"),
         NodeTreeIconID::nodeTreeIconBinary,
         m_file,
         startOffset,
         bigObjAnonymousObjectHeader.sizeOfData);

      nonCoffSummaryNode.ChildNodes().push_back(hexDataNode);
   }
}
