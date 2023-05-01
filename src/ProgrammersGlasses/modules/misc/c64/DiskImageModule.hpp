//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImageModule.hpp
/// \brief module to display C64 disk images
//
#pragma once

#include "modules/IModule.hpp"

/// \brief C64 disk image module
/// \details C64 floppy disks are stored in various disk image formats that
/// differ since the floppy drives and disk media can store different amout of
/// data.
/// \see http://unusedino.de/ec64/technical/formats/d64.html
class DiskImageModule : public IModule
{
public:
   /// ctor
   DiskImageModule() = default;

   // Inherited via IModule
   virtual CString DisplayName() const override
   {
      return CString{ "C64 disk image module" };
   }

   virtual ModuleIconID IconID() const override
   {
      return ModuleIconID::moduleStaticLib;
   }

   virtual CString FilterStrings() const override
   {
      return CString{ "C64 disk image files (*.d64, *.d71; *.d81)|*.d64;*.d71;*.d81|" };
   }

   virtual bool IsModuleApplicableForFile(const File& file) const override;
   virtual std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
