//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffModule.hpp
/// \brief module to load COFF files
//
#pragma once

#include "modules/IModule.hpp"

/// \brief COFF module
/// \details module to load files in the COFF format, e.g. .obj or .lib files
class CoffModule : public IModule
{
public:
   /// ctor
   CoffModule() {}

   // Inherited via IModule
   virtual CString DisplayName() const override;
   virtual ModuleIconID IconID() const override;
   virtual CString FilterStrings() const override;
   virtual bool IsModuleApplicableForFile(const File& file) const override;
   virtual std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
