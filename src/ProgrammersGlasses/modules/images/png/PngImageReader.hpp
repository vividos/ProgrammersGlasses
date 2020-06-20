//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PngImageModule.hpp
/// \brief module to load PNG image files
//
#pragma once

#include "modules/IReader.hpp"

/// PNG image reader
class PngImageReader : public IReader
{
public:
   /// determines if the given file is a PNG file (by checking the signature)
   static bool IsPngFile(const File& file);

   /// ctor
   explicit PngImageReader(const File& file);

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
   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
