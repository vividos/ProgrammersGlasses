//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SidAudioModule.cpp
/// \brief module to show SID file infos
//
#include "stdafx.h"
#include "SidAudioModule.hpp"
#include "SidFileReader.hpp"

bool SidAudioModule::IsModuleApplicableForFile(const File& file) const
{
   return SidFileReader::IsSidFile(file);
}

std::shared_ptr<IReader> SidAudioModule::OpenReader(const File& file) const
{
   return std::make_shared<SidFileReader>(file);
}
