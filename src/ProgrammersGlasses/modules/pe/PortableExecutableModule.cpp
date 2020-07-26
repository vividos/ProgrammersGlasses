//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PortableExecutableModule.cpp
/// \brief module to load Portable Executable binary files
//
#include "stdafx.h"
#include "PortableExecutableModule.hpp"
#include "PortableExecutableReader.hpp"

bool PortableExecutableModule::IsModuleApplicableForFile(const File& file) const
{
   return PortableExecutableReader::IsPortableExecutableFile(file);
}

std::shared_ptr<IReader> PortableExecutableModule::OpenReader(const File& file) const
{
   return std::make_shared<PortableExecutableReader>(file);
}
