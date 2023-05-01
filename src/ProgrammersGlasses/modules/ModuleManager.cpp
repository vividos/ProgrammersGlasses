//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file ModuleManager.cpp
/// \brief module manager
//
#include "stdafx.h"
#include "ModuleManager.hpp"
#include "IModule.hpp"
#include "dev/coff/CoffModule.hpp"
#include "dev/pe/PortableExecutableModule.hpp"
#include "dev/elf/ElfModule.hpp"
#include "images/png/PngImageModule.hpp"
#include "audio/sid/SidAudioModule.hpp"
#include "misc/c64/DiskImageModule.hpp"

ModuleManager::ModuleManager()
{
   m_moduleList.push_back(std::make_shared<CoffModule>());
   m_moduleList.push_back(std::make_shared<PortableExecutableModule>());
   m_moduleList.push_back(std::make_shared<ElfModule>());
   m_moduleList.push_back(std::make_shared<PngImageModule>());
   m_moduleList.push_back(std::make_shared<SidAudioModule>());
   m_moduleList.push_back(std::make_shared<DiskImageModule>());
}

CString ModuleManager::GetAllFilterStrings() const
{
   CString filter;
   for (const auto& theModule : m_moduleList)
      filter.Append(theModule->FilterStrings());

   return filter;
}

bool ModuleManager::IsModuleAvailable(const CString& filename) const
{
   CString extension = Path::ExtensionOnly(filename);

   for (auto theModule : m_moduleList)
      if (IsMatchingModule(*theModule, extension))
         return true;

   return false;
}

std::shared_ptr<IReader> ModuleManager::LoadFile(const CString& filename) const
{
   CString extension = Path::ExtensionOnly(filename);

   for (auto theModule : m_moduleList)
      if (IsMatchingModule(*theModule, extension))
      {
         File file{ filename };
         if (theModule->IsModuleApplicableForFile(file))
            return theModule->OpenReader(file);
      }

   return nullptr;
}

bool ModuleManager::IsMatchingModule(const IModule& theModule, const CString& extension)
{
   CString filterStrings = theModule.FilterStrings();

   std::vector<CString> filterStringsList = StringSplit(filterStrings, _T("|"), true);

   ATLASSERT(filterStringsList.size() % 2 == 0); // filter strings list entries must be even

   for (size_t index = 0; index < filterStringsList.size(); index += 2)
   {
      std::vector<CString> extensionsList =
         StringSplit(filterStringsList[index + 1], _T(";"), false);

      for (CString extensionToCheck : extensionsList)
      {
         extensionToCheck.TrimLeft(_T("*"));
         if (extensionToCheck.CompareNoCase(extension) == 0)
            return true;
      }
   }

   return false;
}
