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
   virtual CString DisplayName() const override;
   virtual ModuleIconID IconID() const override;
   virtual CString FilterStrings() const override;
   virtual bool IsModuleApplicableForFile(const File& file) const override;
   virtual std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
