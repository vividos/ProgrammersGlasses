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
ModuleManager::ModuleManager()
{
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
   CString formatStrings = module.FormatStrings();

   std::vector<CString> formatStringsList = StringSplit(formatStrings, _T("|"), true);

   ATLASSERT(formatStringsList.size() % 2 == 0); // format strings list entries must be even

   for (size_t index = 0; index < formatStringsList.size(); index += 2)
   {
      std::vector<CString> extensionsList =
         StringSplit(formatStringsList[index + 1], _T(";"), false);

      for (CString extensionToCheck : extensionsList)
      {
         extensionToCheck.TrimLeft(_T("*"));
         if (extensionToCheck.CompareNoCase(extension) == 0)
            return true;
      }
   }

   return false;
}
