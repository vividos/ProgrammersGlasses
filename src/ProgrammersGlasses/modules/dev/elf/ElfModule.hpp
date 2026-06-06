//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file ElfModule.hpp
/// \brief module to load ELF binary files
//
#pragma once

#include "modules/IModule.hpp"

/// \brief ELF module
/// \details module to load files in the ELF format, e.g. .so files
class ElfModule : public IModule
{
public:
   /// ctor
   ElfModule() = default;

   // Inherited via IModule
   CString DisplayName() const override;
   ModuleIconID IconID() const override;
   CString FilterStrings() const override;
   bool IsModuleApplicableForFile(const File& file) const override;
   std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
