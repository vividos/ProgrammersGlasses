//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file ModuleManager.cpp
/// \brief module manager
//
#include "stdafx.h"
#include "ModuleManager.hpp"
#include "IModule.hpp"
#include "coff/CoffModule.hpp"
#include "elf/ElfModule.hpp"
#include "images/png/PngImageModule.hpp"
#include "audio/sid/SidAudioModule.hpp"

ModuleManager::ModuleManager()
{
   m_moduleList.push_back(std::make_shared<CoffModule>());
   m_moduleList.push_back(std::make_shared<ElfModule>());
   m_moduleList.push_back(std::make_shared<PngImageModule>());
   m_moduleList.push_back(std::make_shared<SidAudioModule>());
}

CString ModuleManager::GetAllFilterStrings() const
{
   CString filter;
   for (const auto& module : m_moduleList)
      filter.Append(module->FilterStrings());

   return filter;
}

bool ModuleManager::IsModuleAvailable(const CString& filename) const
{
   CString extension = Path::ExtensionOnly(filename);

   for (auto module : m_moduleList)
      if (IsMatchingModule(*module, extension))
         return true;

   return false;
}

std::shared_ptr<IReader> ModuleManager::LoadFile(const CString& filename) const
{
   CString extension = Path::ExtensionOnly(filename);

   for (auto module : m_moduleList)
      if (IsMatchingModule(*module, extension))
      {
         File file{ filename };
         if (module->IsModuleApplicableForFile(file))
            return module->OpenReader(file);
      }

   return nullptr;
}

bool ModuleManager::IsMatchingModule(const IModule& module, const CString& extension)
{
   CString filterStrings = module.FilterStrings();

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
