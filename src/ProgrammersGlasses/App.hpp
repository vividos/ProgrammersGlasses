//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file App.hpp
/// \brief application class
//
#include <vector>

/// the application
class App
{
public:
   /// ctor
   App(HINSTANCE instance);
   /// dtor
   ~App() noexcept;

   /// parses command line
   void ParseCommandLine(LPCTSTR commandLine = nullptr);

   /// runs the application
   int Run(int commandShow = SW_SHOWDEFAULT);

private:
   /// list of filenames to open at startup
   std::vector<CString> m_filenamesList;
};
