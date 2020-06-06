//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file IModule.hpp
/// \brief module interface
//
#pragma once

#include "File.hpp"

class IReader;

/// Icon IDs from resources for various modules
enum class ModuleIconID : UINT
{
   moduleStaticLib = 512,  ///< module icon for a static library
   moduleDynamicLib = 513, ///< module icon for a dynamic library
   moduleApp = 514,        ///< module icon for an application
   moduleImage = 515,      ///< module icon for an image module
   moduleAudio = 516,      ///< module icon for an audio module
   moduleVideo = 517,      ///< module icon for a video module
};

/// \brief module interface
/// \details a module can provide a reader to parse a specific file format.
class IModule
{
public:
   /// dtor
   virtual ~IModule() noexcept {}

   /// Returns a displayable name for the module
   virtual CString DisplayName() const = 0;

   /// Returns an icon resource ID for the module
   virtual ModuleIconID IconID() const = 0;

   /// Returns a list of filter strings that this module can handle; the format
   /// is: "Xzy Files (*.xyz)|*.xyz|"
   virtual CString FilterStrings() const = 0;

   /// Called when a module should check an opened file if its format is correct
   /// and can be parsed with this module or not
   virtual bool IsModuleApplicableForFile(const File& file) const = 0;

   /// Gets a reader for for the given file
   virtual std::shared_ptr<IReader> OpenReader(const File& file) const = 0;
};
