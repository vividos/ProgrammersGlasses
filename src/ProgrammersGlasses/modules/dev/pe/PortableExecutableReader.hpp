//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PortableExecutableReader.hpp
/// \brief reader for Portable Executable files
//
#pragma once

#include "modules/IReader.hpp"

/// PE reader
class PortableExecutableReader : public IReader
{
public:
   /// determines if the given file is a valid PE file
   static bool IsPortableExecutableFile(const File& file);

   /// ctor
   explicit PortableExecutableReader(const File& file);

   // Inherited via IReader
   const CString& Filename() const override
   {
      return m_file.Filename();
   }

   std::shared_ptr<INode> RootNode() const override
   {
      return m_rootNode;
   }

   void Load() override;
   void Cleanup() override;

private:
   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
