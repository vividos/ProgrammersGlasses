//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SidAudioModule.hpp
/// \brief module to show SID file infos
//
#pragma once

#include "modules/IModule.hpp"

/// \brief Module to show SID audio file infos
/// \details The SID files store C64 music that can be played with software like Sidplay on other
/// operating systems.
/// \see https://gist.github.com/cbmeeks/2b107f0a8d36fc461ebb056e94b2f4d6
class SidAudioModule : public IModule
{
public:
   /// ctor
   SidAudioModule() = default;

   // Inherited via IModule
   virtual CString DisplayName() const override
   {
      return CString{ "SID C64 audio module" };
   }

   virtual ModuleIconID IconID() const override
   {
      return ModuleIconID::moduleAudio;
   }

   virtual CString FilterStrings() const override
   {
      return CString{ "SID files (*.sid, *.psid)|*.sid;*.psid|" };
   }

   virtual bool IsModuleApplicableForFile(const File& file) const override;
   virtual std::shared_ptr<IReader> OpenReader(const File& file) const override;
};
