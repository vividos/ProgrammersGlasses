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

bool CoffReader::IsCoffFileFormat(const File& file)
{
   // TODO implement
   file;
   return true;
}

CoffReader::CoffReader(const File& file)
   :m_file(file)
{
}

void CoffReader::Load()
{
   // TODO implement
   m_rootNode.reset(new CoffNode(_T("Summary"), NodeTreeIconID::nodeTreeIconLibrary));
}

void CoffReader::Cleanup()
{
   // nothing expensive to cleanup here
}
