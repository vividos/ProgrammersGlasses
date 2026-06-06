//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PortableExecutableModule.hpp
/// \brief module to load Portable Executable binary files
//
#pragma once

#include "modules/IModule.hpp"

/// \brief PE module
/// \details module to load portable executable .exe and .dll files
class PortableExecutableModule : public IModule
{
public:
   /// ctor
   PortableExecutableModule() = default;

   // Inherited via IModule
   CString DisplayName() const override
   {
      return CString{ "PE binary module" };
   }

   ModuleIconID IconID() const override
   {
      return ModuleIconID::moduleApp;
   }

   CString FilterStrings() const override
   {
      return CString{ "PE files (*.exe;*.dll;*.ocx;*.sys;*.ovl)|*.exe;*.dll;*.ocx;*.sys;*.ovl|" };
   }

   bool IsModuleApplicableForFile(const File& file) const override;
   std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
