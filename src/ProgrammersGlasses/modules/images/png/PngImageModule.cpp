//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PngImageModule.cpp
/// \brief module to load PNG image files
//
#include "stdafx.h"
#include "PngImageModule.hpp"
#include "PngImageReader.hpp"

bool PngImageModule::IsModuleApplicableForFile(const File& file) const
{
   return PngImageReader::IsPngFile(file);
}

std::shared_ptr<IReader> PngImageModule::OpenReader(const File& file) const
{
   return std::make_shared<PngImageReader>(file);
}
