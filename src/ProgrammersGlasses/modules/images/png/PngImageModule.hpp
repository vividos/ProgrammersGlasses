//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PngImageModule.hpp
/// \brief module to load PNG image files
//
#pragma once

#include "modules/IModule.hpp"

/// \brief PNG image module
/// \details module to display PNG (portable network graphics) image structure and details
class PngImageModule : public IModule
{
public:
   /// ctor
   PngImageModule() {}

   // Inherited via IModule
   virtual CString DisplayName() const override
   {
      return CString{ "PNG image module" };
   }

   virtual ModuleIconID IconID() const override
   {
      return ModuleIconID::moduleImage;
   }

   virtual CString FilterStrings() const override
   {
      return CString{ "PNG Images (*.png)|*.png|" };
   }

   virtual bool IsModuleApplicableForFile(const File& file) const override;
   virtual std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
