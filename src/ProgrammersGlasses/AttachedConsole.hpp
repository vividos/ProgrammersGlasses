//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AttachedConsole.hpp Attached console class
//
#pragma once

/// \brief attached console
/// \details when run as Win32 application, provides access to console when started on a console
class AttachedConsole
{
public:
   /// ctor
   AttachedConsole();
   /// dtor
   ~AttachedConsole();

   /// prints prompt and detaches console
   void Detach();

   /// indicates if this process is started from console
   bool IsCalledFromConsole() const { return m_calledFromConsole; }

private:
   /// reads command line prompt from current cursor pos
   static void ReadConsolePrompt(CString& consolePrompt);

   /// \brief sets up redirection for stdout
   /// \details code from http://www.halcyon.com/~ast/dload/guicon.htm
   static void SetupRedirect();

private:
   /// indicates if process was started from console
   bool m_calledFromConsole = false;

   /// console prompt string
   CString m_consolePrompt;
};
