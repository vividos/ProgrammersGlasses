//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImageModule.cpp
/// \brief module to display C64 disk images
//
#include "stdafx.h"
#include "DiskImageModule.hpp"
#include "DiskImageReader.hpp"

bool DiskImageModule::IsModuleApplicableForFile(const File& file) const
{
   return DiskImageReader::IsDiskImageFile(file);
}

std::shared_ptr<IReader> DiskImageModule::OpenReader(const File& file) const
{
   return std::make_shared<DiskImageReader>(file);
}
