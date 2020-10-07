//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file ElfModule.cpp
/// \brief module to load ELF binary files
//
#include "stdafx.h"
#include "ElfModule.hpp"

CString ElfModule::DisplayName() const
{
   return CString{ "ELF binary module" };
}

ModuleIconID ElfModule::IconID() const
{
   return ModuleIconID::moduleDynamicLib;
}

CString ElfModule::FilterStrings() const
{
   return CString{ "ELF shared library files (*.so)|*.so|" };
}

bool ElfModule::IsModuleApplicableForFile(const File& file) const
{
   if (file.Size() < 4)
      return false;

   const BYTE* data = reinterpret_cast<const BYTE*>(file.Data());
   return
      data[0] == 0x7F &&
      data[1] == 'E' &&
      data[2] == 'L' &&
      data[3] == 'F';
}

std::shared_ptr<IReader> ElfModule::OpenReader(const File& file) const
{
   (void)file;
   return nullptr;
}
