//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file AppOptions.hpp
/// \brief application options
//
#pragma once

#include <vector>
#include <ulib/ProgramOptions.hpp>

/// App command line options
class AppOptions : public ProgramOptions
{
public:
   /// ctor
   AppOptions();

   /// returns if if console output should be used
   bool UseConsole() const { return m_useConsole; }

   /// returns the list of filenames to open
   const std::vector<CString> FilenamesList() const { return m_filenamesList; }

private:
   /// indicates if console output should be used
   bool m_useConsole = false;

   /// list of filenames to open
   std::vector<CString> m_filenamesList;
};
