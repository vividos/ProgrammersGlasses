//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffReader.hpp
/// \brief reader for COFF format files
//
#pragma once

#include "IReader.hpp"

class CoffNode;
struct CoffHeader;

/// COFF file format reader
class CoffReader : public IReader
{
public:
   /// checks file if it a COFF file
   static bool IsCoffFileFormat(const File& file);

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
   /// adds summary text to node
   void AddSummaryText(CoffNode& node, const CoffHeader& header);

   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
