//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SidFileReader.hpp
/// \brief reader for SID files
//
#include "stdafx.h"
#include "modules/IReader.hpp"

struct SidFileHeader;
class CodeTextViewNode;

/// \brief SID file reader
class SidFileReader : public IReader
{
public:
   /// ctor
   explicit SidFileReader(const File& file)
      :m_file(file)
   {
   }

   /// determines if given file is a valid SID file
   static bool IsSidFile(const File& file);

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

   virtual void Cleanup() override
   {
      // nothing expensive to clean up
   }

private:
   /// adds summary text from header to node
   void AddSummaryText(const SidFileHeader& header, std::shared_ptr<CodeTextViewNode> summaryNode);

   /// the SID file
   File m_file;

   /// the root node
   std::shared_ptr<INode> m_rootNode;
};
