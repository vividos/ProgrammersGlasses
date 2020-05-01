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

int App::Run(LPTSTR /*commandLine*/, int commandShow)
{
   if (!RunTimeHelper::IsRibbonUIAvailable())
   {
      AtlMessageBox(NULL, L"Cannot run with this version of Windows", IDR_MAINFRAME);
      return -1;
   }

   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain;

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
   return app.Run(commandLine, commandShow);
}
