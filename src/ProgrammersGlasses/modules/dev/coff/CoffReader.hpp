//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CoffReader.hpp
/// \brief reader for COFF format files
//
#pragma once

#include "IReader.hpp"

class CodeTextViewNode;
class StaticNode;

/// COFF file format reader
class CoffReader : public IReader
{
public:
   /// checks file if it a COFF object file
   static bool IsCoffObjectFile(const File& file);

   /// checks file if it an "ar" archive library file
   static bool IsArLibraryFile(const File& file);

   /// ctor
   explicit CoffReader(const File& file);

   // Inherited via IReader
   virtual const CString& Filename() const override
   {
      return m_file.Filename();
   }

   virtual std::shared_ptr<INode> RootNode() const override
   {
      return m_rootNode;
   }

   virtual void Load() override;
   virtual void Cleanup() override;

private:
   friend class PortableExecutableReader;

   /// loads COFF based object files
   void LoadCoffObjectFile();

   /// loads non-COFF (import or anonymous) object files
   void LoadNonCoffObjectFile();

   /// loads archive library files
   void LoadArchiveLibraryFile();

   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
