//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file App.hpp
/// \brief application class
//
#pragma once

#include "AppOptions.hpp"

/// the application
class App
{
public:
   /// ctor
   explicit App(HINSTANCE hInstance);
   /// dtor
   ~App() noexcept;
   // copy ctor
   App(const App&) = delete;
   // copy assignment operator
   App& operator=(const App&) = delete;

   /// parses command line
   void ParseCommandLine(LPCTSTR commandLine = nullptr);

   /// runs the application
   int Run(int commandShow = SW_SHOWDEFAULT) const;

private:
   /// app options
   AppOptions m_appOptions;
};
