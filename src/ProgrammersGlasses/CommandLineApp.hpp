//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CommandLineApp.hpp
/// \brief command line application class
//
#pragma once

#include "ModuleManager.hpp"

class INode;

/// \brief command line application
/// The command line app loads all files specified on the command line and
/// dumps the collected nodes on the command line.
/// Mainly used to run tests and collect coverage.
class CommandLineApp
{
public:
   /// ctor
   CommandLineApp(const std::vector<CString>& filenamesList);

   /// runs command line app
   int Run() const;

private:
   /// loads a file and outputs its node tree
   void OutputFile(const CString& filename) const;

   /// dumps a single node; called recursively
   void DumpNodeRecursively(std::shared_ptr<INode> node) const;

private:
   /// list of filenames to load and dump
   std::vector<CString> m_filenamesList;

   /// module manager
   ModuleManager m_moduleManager;
};
