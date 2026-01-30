//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AttachedConsole.cpp Attached console class
//
#include "stdafx.h"
#include "AttachedConsole.hpp"
#include <io.h> // for _open_osfhandle
#include <fcntl.h> // for _O_TEXT
#include <iostream>

AttachedConsole::AttachedConsole()
{
   BOOL result = ::AttachConsole(ATTACH_PARENT_PROCESS);
   if (result == FALSE)
   {
      DWORD lastError = GetLastError();
      if (lastError == ERROR_ACCESS_DENIED)
      {
         // already have a console
         m_calledFromConsole = true;
         return;
      }
      else
      {
         m_calledFromConsole = false;
         return;
      }
   }

   m_calledFromConsole = true;

   ReadConsolePrompt(m_consolePrompt);

   SetupRedirect();
}

AttachedConsole::~AttachedConsole()
{
   Detach();
}

void AttachedConsole::Detach()
{
   if (!m_consolePrompt.IsEmpty())
   {
      _tprintf(_T("%s"), m_consolePrompt.GetString());
      m_consolePrompt.Empty();
   }

   if (m_calledFromConsole)
      ATLVERIFY(TRUE == FreeConsole());
}

void AttachedConsole::ReadConsolePrompt(CString& consolePrompt)
{
   // read console prompt
   HANDLE consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE); // don't close the handle
   ATLASSERT(consoleHandle != INVALID_HANDLE_VALUE);

   // get cursor pos
   CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
   ATLVERIFY(TRUE == GetConsoleScreenBufferInfo(consoleHandle, &consoleScreenBufferInfo));

   COORD cursorPos = {};
   cursorPos.X = consoleScreenBufferInfo.srWindow.Left;
   cursorPos.Y = consoleScreenBufferInfo.dwCursorPosition.Y;

   const unsigned int numReadChars = consoleScreenBufferInfo.dwCursorPosition.X;

   // read prompt
   DWORD numCharsRead = 0;

   ::ReadConsoleOutputCharacter(consoleHandle,
      consolePrompt.GetBuffer(numReadChars),
      numReadChars,
      cursorPos, &numCharsRead);

   consolePrompt.ReleaseBuffer(numCharsRead);

   // set cursor to first column
   ::SetConsoleCursorPosition(consoleHandle, cursorPos);

   // overwrite prompt
   CString blankText(_T(' '), numReadChars);
   ATLVERIFY(TRUE == ::WriteConsole(consoleHandle, blankText, numReadChars, nullptr, nullptr));

   // set cursor again
   ::SetConsoleCursorPosition(consoleHandle, cursorPos);
}

void AttachedConsole::SetupRedirect()
{
   //freopen("CONOUT$", "w", stdout);
   FILE* fd = nullptr;
   freopen_s(&fd, "CONOUT$", "w", stdout);
   freopen_s(&fd, "CONOUT$", "w", stderr);
   freopen_s(&fd, "CONIN$", "r", stdin);

   // redirect unbuffered STDOUT to the console
   //HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   //int stdoutFileDescriptor = _open_osfhandle((intptr_t)stdoutHandle, _O_TEXT);
   ////_dup2(stdoutFileDescriptor, 1);

   //*stdout = *_fdopen(stdoutFileDescriptor, "w");
   //setvbuf(stdout, NULL, _IONBF, 0);

   //FILE* fp = _fdopen(stdoutFileDescriptor, "w");
   //*stdout = *fp;

   // set stdout to unbuffered
   setvbuf(stdout, nullptr, _IONBF, 0);

   std::cout.clear();
   std::wcout.clear();
   std::clog.clear();
   std::wclog.clear();
   std::cerr.clear();
   std::wcerr.clear();
   std::cin.clear();
   std::wcin.clear();
}
