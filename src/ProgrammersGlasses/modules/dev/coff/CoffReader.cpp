//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CoffReader.cpp
/// \brief reader for COFF format files
//
#include "stdafx.h"
#include "CoffReader.hpp"
#include "CoffHeader.hpp"
#include "CoffObjectNodeTreeBuilder.hpp"
#include "NonCoffObjectNodeTreeBuilder.hpp"
#include "ArchiveFileNodeTreeBuilder.hpp"
#include "ArchiveHeader.hpp"
#include "File.hpp"
#include "Helper.hpp"

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


void CoffReader::LoadArchiveLibraryFile()
{
   ArchiveFileNodeTreeBuilder nodeTreeBuilder{ m_file, 0 };
   m_rootNode = nodeTreeBuilder.BuildArchiveFileNode();
}
