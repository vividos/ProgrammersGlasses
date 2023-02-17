//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file PortableExecutableReader.cpp
/// \brief reader for Portable Executable files
//
#include "stdafx.h"
#include "PortableExecutableReader.hpp"
#include "DosMzHeader.hpp"
#include "modules/CodeTextViewNode.hpp"
#include "modules/StructListViewNode.hpp"

/// PE signature struct definition
static const StructDefinition g_definitionPeSignature = StructDefinition({
   StructField(
      0,
      2,
      1,
      true, // little-endian
      StructFieldType::text,
      _T("PE Signature"))
   });

bool PortableExecutableReader::IsPortableExecutableFile(const File& file)
{
   const DosMzHeader& header = *file.Data<DosMzHeader>();

   return header.magicNumber[0] == 'M' &&
      header.magicNumber[1] == 'Z';
}

PortableExecutableReader::PortableExecutableReader(const File& file)
   :m_file(file)
{
}

void PortableExecutableReader::Load()
{
   auto rootNode = std::make_shared<CodeTextViewNode>(
      _T("Summary"),
      NodeTreeIconID::nodeTreeIconDocument);

   m_rootNode = rootNode;

   CString summaryText;
   summaryText.Append(_T("Executable file: ") + m_file.Filename() + _T("\n\n"));
   summaryText.Append(_T("Summary:\n"));

   const DosMzHeader& dosMzHeader = *m_file.Data<DosMzHeader>();

   auto msdosHeaderNode = std::make_shared<StructListViewNode>(
      _T("MZ header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionDosMzHeader,
      &dosMzHeader,
      m_file.Data());

   rootNode->ChildNodes().push_back(msdosHeaderNode);

   // add PE signature
   if (dosMzHeader.newExecutableHeader == 0 ||
      dosMzHeader.newExecutableHeader < dosMzHeader.numHeaderParagraphs * 16 ||
      !m_file.IsValidRange(m_file.Data<BYTE>(dosMzHeader.newExecutableHeader), 4))
   {
      summaryText += _T("New executable header offset is invalid.");
      rootNode->SetText(summaryText);

      return;
   }

   const DWORD* peSignature = m_file.Data<DWORD>(dosMzHeader.newExecutableHeader);

   if (*peSignature != 0x00004550) // "PE\0\0"
   {
      summaryText += _T("PE signature not found; normal MS DOS executable.");

      rootNode->SetText(summaryText);

      return;
   }

   auto peSignatureNode = std::make_shared<StructListViewNode>(
      _T("PE Signature"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionPeSignature,
      peSignature,
      m_file.Data());

   rootNode->ChildNodes().push_back(peSignatureNode);

   // TODO add COFF file header
   // TODO add optional header

   rootNode->SetText(summaryText);

   m_rootNode = rootNode;
}

void PortableExecutableReader::Cleanup()
{
   // nothing expensive to cleanup here
}
