//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file App.hpp
/// \brief application class
//

/// the application
class App
{
public:
   /// ctor
   App(HINSTANCE instance);
   /// dtor
   ~App() noexcept;

   /// runs the application
   int Run(LPTSTR commandLine = nullptr, int commandShow = SW_SHOWDEFAULT);
};
