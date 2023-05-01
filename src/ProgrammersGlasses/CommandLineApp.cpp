//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CommandLineApp.cpp
/// \brief command line application class
//
#include "stdafx.h"
#include "CommandLineApp.hpp"
#include "IReader.hpp"
#include "INode.hpp"
#include "Helper.hpp"
#include "CodeTextViewNode.hpp"
#include <ulib/Timer.hpp>

CommandLineApp::CommandLineApp(const std::vector<CString>& filenamesList)
   :m_filenamesList(filenamesList)
{
   _tprintf(_T("Programmer's Glasses - a developer's file content viewer\n\n"));
}

int CommandLineApp::Run() const
{
   for (const CString& filename : m_filenamesList)
   {
      OutputFile(filename);
   }

   return 0;
}

void CommandLineApp::OutputFile(const CString& filename) const
{
   _tprintf(_T("Dumping file: %s\n"), filename.GetString());

   if (!Path::FileExists(filename))
   {
      _tprintf(_T("Error: Couldn't open file: %s\n\n"), filename.GetString());
      return;
   }

   if (!m_moduleManager.IsModuleAvailable(filename))
   {
      _tprintf(_T("Error: No suitable module found for file: %s\n\n"),
         filename.GetString());
      return;
   }

   auto reader = m_moduleManager.LoadFile(filename);

   if (reader == nullptr)
   {
      _tprintf(_T("Error: The module couldn't initialize a reader for the file: %s\n\n"),
         filename.GetString());
      return;
   }

   Timer loadTimer;
   loadTimer.Start();
   reader->Load();
   loadTimer.Stop();

   _tprintf(_T("Loading file took %u ms.\n"),
      int(loadTimer.TotalElapsed() * 1000));

   std::shared_ptr<INode> rootNode = reader->RootNode();

   DumpNodeRecursively(rootNode);

   reader->Cleanup();

   _tprintf(_T("\n"));
}

void CommandLineApp::DumpNodeRecursively(std::shared_ptr<INode> node) const
{
   _tprintf(_T("Node name: %s\n"), node->DisplayName().GetString());

   std::shared_ptr<CodeTextViewNode> codeTextViewNode =
      std::dynamic_pointer_cast<CodeTextViewNode>(node);

   if (codeTextViewNode != nullptr)
   {
      CString codeText = codeTextViewNode->GetText();

      IndentText(codeText, 3);
      _tprintf(_T("%s\n\n"), codeText.GetString());
   }

   for (auto& childNode : node->ChildNodes())
   {
      DumpNodeRecursively(childNode);
   }
}
