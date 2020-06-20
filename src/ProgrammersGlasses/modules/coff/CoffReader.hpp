//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffReader.hpp
/// \brief reader for COFF format files
//
#pragma once

#include "IReader.hpp"

class CodeTextViewNode;
struct CoffHeader;

/// COFF file format reader
class CoffReader : public IReader
{
public:
   /// checks file if it a COFF object file
   static bool IsCoffObjectFile(const File& file);

   /// checks file if it a non-COFF or anonymous object file
   static bool IsNonCoffOrAnonymousObjectFile(const File& file);

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
   /// loads COFF based object files
   void LoadCoffObjectFile();

   /// adds summary text to node
   void AddCoffHeaderSummaryText(CodeTextViewNode& node, const CoffHeader& header, bool isImage);

   /// adds section table to node
   void AddSectionTable(CodeTextViewNode& sectionSummaryNode, const CoffHeader& header);

   /// adds symbol table to node
   void AddSymbolTable(CodeTextViewNode& symbolTableSummaryNode, const CoffHeader& header);

   /// loads non-COFF or anonymous object files
   void LoadNonCoffObjectFile();

   /// loads archive library files
   void LoadArchiveLibraryFile();

   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
