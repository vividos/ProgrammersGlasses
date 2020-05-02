//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file App.cpp
/// \brief application class
//
#include "stdafx.h"
#include "resource.h"
#include "App.hpp"
#include "MainFrame.hpp"
#include <ulib/CommandLineParser.hpp>

CAppModule _Module;

App::App(HINSTANCE instance)
{
   HRESULT result = ::CoInitialize(nullptr);
   ATLASSERT(SUCCEEDED(result));

   AtlInitCommonControls(ICC_BAR_CLASSES); // add flags to support other controls

   result = _Module.Init(nullptr, instance);
   ATLASSERT(SUCCEEDED(result));
}

App::~App()
{
   _Module.Term();
   ::CoUninitialize();
}

void App::ParseCommandLine(LPCTSTR commandLine)
{
   if (commandLine == nullptr)
      return;

   CommandLineParser parser{ commandLine };

   CString filename;
   while (parser.GetNext(filename))
      m_filenamesList.push_back(filename);
}

int App::Run(int commandShow)
{
   if (!RunTimeHelper::IsRibbonUIAvailable())
   {
      AtlMessageBox(NULL, L"Cannot run with this version of Windows", IDR_MAINFRAME);
      return -1;
   }

   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain{ m_filenamesList };

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
int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE /*previousInstance*/, LPTSTR commandLine, int commandShow)
{
   App app{ instance };
   app.ParseCommandLine(commandLine);
   return app.Run(commandShow);
}
