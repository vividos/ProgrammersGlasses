//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file ModuleManager.hpp
/// \brief module manager
//
#pragma once

#include <vector>
#include <memory>

class IModule;
class IReader;

/// \brief module manager
/// \details manages all modules that can be used to open files
class ModuleManager
{
public:
   /// ctor
   ModuleManager();

   /// checks if there's a module available for the file
   bool IsModuleAvailable(const CString& filename) const;

   /// returns module list
   const std::vector<std::shared_ptr<IModule>>& GetModuleList() const
   {
      return m_moduleList;
   }

   /// loads file and returns a reader
   std::shared_ptr<IReader> LoadFile(const CString& filename) const;

private:
   /// checks if the given module matches the file extension
   static bool IsMatchingModule(const IModule& module, const CString& extension);

   /// list of all available modules
   std::vector<std::shared_ptr<IModule>> m_moduleList;
};
