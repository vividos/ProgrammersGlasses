//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffModule.cpp
/// \brief module to load COFF files
//
#include "stdafx.h"
#include "CoffModule.hpp"
#include "CoffReader.hpp"
#include "NonCoffObjectNodeTreeBuilder.hpp"

CString CoffModule::DisplayName() const
{
   return CString{ "COFF module" };
}

ModuleIconID CoffModule::IconID() const
{
   return ModuleIconID::moduleStaticLib;
}

CString CoffModule::FilterStrings() const
{
   return CString{ "COFF library files (*.lib;*.exp;*.a)|*.lib;*.exp;*.a|COFF object files (*.obj;*.cof)|*.obj;*.cof;*.o|" };
}

bool CoffModule::IsModuleApplicableForFile(const File& file) const
{
   return CoffReader::IsCoffObjectFile(file) ||
      NonCoffObjectNodeTreeBuilder::IsNonCoffOrAnonymousObjectFile(file, 0) ||
      CoffReader::IsArLibraryFile(file);
}

std::shared_ptr<IReader> CoffModule::OpenReader(const File& file) const
{
   return std::make_shared<CoffReader>(file);
}
