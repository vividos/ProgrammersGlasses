//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file App.cpp
/// \brief application class
//
#include "stdafx.h"
#include "resource.h"
#include "res/Ribbon.h"
#include "App.hpp"
#include "MainFrame.hpp"
#include "CommandLineApp.hpp"

CAppModule _Module;

App::App(HINSTANCE instance)
{
   HRESULT result = ::CoInitialize(nullptr);
   ATLVERIFY(SUCCEEDED(result));

   AtlInitCommonControls(ICC_BAR_CLASSES); // add flags to support other controls

   result = _Module.Init(nullptr, instance);
   ATLVERIFY(SUCCEEDED(result));
}

App::~App() noexcept
{
   _Module.Term();
   ::CoUninitialize();
}

void App::ParseCommandLine(LPCTSTR commandLine)
{
   if (commandLine == nullptr)
      return;

   // need to prepend with .exe filename, since first argument is skipped
   m_appOptions.Parse(_T("ProgrammersGlasses.exe ") + CString{ commandLine });
}

int App::Run(int commandShow) const
{
   if (m_appOptions.UseConsole())
   {
      CommandLineApp commandLineApp{ m_appOptions.FilenamesList() };
      return commandLineApp.Run();
   }

   if (!RunTimeHelper::IsRibbonUIAvailable())
   {
      AtlMessageBox(nullptr, L"Cannot run with this version of Windows", IDR_MAINFRAME);
      return -1;
   }

   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain{ m_appOptions.FilenamesList() };

   if (wndMain.CreateEx() == nullptr)
   {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }

   wndMain.ShowWindow(commandShow);

   int ret = theLoop.Run();

   _Module.RemoveMessageLoop();
   return ret;
}

/// main function
int WINAPI _tWinMain(_In_ HINSTANCE instance,
   _In_opt_ HINSTANCE /*previousInstance*/,
   _In_ LPTSTR commandLine, _In_ int commandShow)
{
   App app{ instance };
   app.ParseCommandLine(commandLine);
   return app.Run(commandShow);
}
