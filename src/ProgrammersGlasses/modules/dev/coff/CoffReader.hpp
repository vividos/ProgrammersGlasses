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
   static bool IsNonCoffOrAnonymousObjectFile(const File& file, size_t fileOffset);

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

   /// adds COFF header, section and symbol tables
   void AddCoffObjectFile(CodeTextViewNode& coffSummaryNode,
      size_t fileOffset, CString& objectFileSummary);

   /// adds summary text to node
   void AddCoffHeaderSummaryText(CodeTextViewNode& node,
      const CoffHeader& header, bool isImage) const;

   /// adds section table to node
   void AddSectionTable(CodeTextViewNode& sectionSummaryNode,
      const CoffHeader& header, size_t fileOffset) const;

   /// adds symbol table to node
   void AddSymbolTable(CodeTextViewNode& symbolTableSummaryNode,
      const CoffHeader& header, size_t fileOffset) const;

   /// loads string table into map
   void LoadStringTable(
      const CoffHeader& header, size_t fileOffset,
      std::map<size_t, CString>& offsetToStringMapping) const;

   /// adds string table to node
   void AddStringTable(CodeTextViewNode& stringTableSummaryNode,
      const CoffHeader& header, size_t fileOffset);

   /// loads non-COFF (import or anonymous) object files
   void LoadNonCoffObjectFile();

   /// adds a non-COFF (import or anonymous) object file to node
   void AddNonCoffObjectFile(CodeTextViewNode& nonCoffSummaryNode,
      size_t fileOffset, CString& objectFileSummary) const;

   /// adds archive linker member to node
   void AddArchiveLinkerMember(CodeTextViewNode& linkerMemberSummaryNode,
      size_t archiveMemberIndex, size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary) const;

   /// adds first linker member node
   void AddFirstLinkerMemberNode(
      size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary, CString& linkerMemberDetails) const;

   /// adds second linker member node
   void AddSecondLinkerMemberNode(
      size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary, CString& linkerMemberDetails) const;

   /// loads archive library files
   void LoadArchiveLibraryFile();

   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
