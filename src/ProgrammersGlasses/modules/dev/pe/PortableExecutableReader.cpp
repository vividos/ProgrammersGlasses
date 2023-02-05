//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PortableExecutableReader.cpp
/// \brief reader for Portable Executable files
//
#include "stdafx.h"
#include "PortableExecutableReader.hpp"
#include "DosMzHeader.hpp"
#include "modules/CodeTextViewNode.hpp"
#include "modules/StructListViewNode.hpp"

bool PortableExecutableReader::IsPortableExecutableFile(const File& file)
{
   const DosMzHeader& header = *reinterpret_cast<const DosMzHeader*>(file.Data());

   return header.magicNumber[0] == 'M' &&
      header.magicNumber[1] == 'Z';
}

PortableExecutableReader::PortableExecutableReader(const File& file)
   :m_file(file)
{
}

void PortableExecutableReader::Load()
{
   auto rootNode = new CodeTextViewNode(_T("Summary"), NodeTreeIconID::nodeTreeIconDocument);

   CString summaryText;
   summaryText.Append(_T("PE file: ") + m_file.Filename() + _T("\n\n"));
   summaryText.Append(_T("Summary:\n"));

   const DosMzHeader& dosMzHeader = *reinterpret_cast<const DosMzHeader*>(m_file.Data());

   auto pngHeaderNode = std::make_shared<StructListViewNode>(
      _T("MZ header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionDosMzHeader,
      &dosMzHeader,
      m_file.Data());

   rootNode->ChildNodes().push_back(pngHeaderNode);

   // TODO add PE signature
   // TODO add COFF file header
   // TODO add optional header

   rootNode->SetText(summaryText);

   m_rootNode.reset(rootNode);
}

void PortableExecutableReader::Cleanup()
{
   // nothing expensive to cleanup here
}
