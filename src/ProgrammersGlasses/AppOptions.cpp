//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file AppOptions.cpp
/// \brief application options
//
#include "stdafx.h"
#include "AppOptions.hpp"

AppOptions::AppOptions()
{
   RegisterOutputHandler(&ProgramOptions::OutputConsole);
   RegisterHelpOption();

   RegisterOption(
      _T("c"),
      _T("console"),
      _T("Shows file infos on the console, not in the Windows application"),
      std::ref(m_useConsole));

   RegisterParameterHandler(
      [&](const CString& filename) -> bool
      {
         m_filenamesList.push_back(filename);
         return true;
      });
}
