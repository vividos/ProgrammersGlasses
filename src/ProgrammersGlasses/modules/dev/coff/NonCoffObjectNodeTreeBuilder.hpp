//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file NonCoffObjectNodeTreeBuilder.hpp
/// \brief Node tree builder for non-COFF object files
//
#pragma once

#include "INode.hpp"
#include "File.hpp"

class CodeTextViewNode;
struct ImportObjectHeader;
struct AnonymousObjectHeader;

/// Node tree builder for non-COFF objects
class NonCoffObjectNodeTreeBuilder
{
public:
   /// checks file if it a non-COFF or anonymous object file
   static bool IsNonCoffOrAnonymousObjectFile(const File& file, size_t fileOffset);

   /// ctor
   NonCoffObjectNodeTreeBuilder(const File& file, size_t fileOffset);

   /// adds non-COFF object infos
   std::shared_ptr<INode> BuildNonCoffObjectNode();

   /// returns object file summary text
   const CString& GetObjectFileSummary() const { return m_objectFileSummary; }

private:
   /// adds a non-COFF (import or anonymous) object file to node
   void AddNonCoffObjectFile(CodeTextViewNode& nonCoffSummaryNode);

   /// adds an import object file to node
   void AddImportObjectNode(CodeTextViewNode& nonCoffSummaryNode,
      const ImportObjectHeader& importObjectHeader);

   /// adds an anonymous object file to node
   void AddAnonymousObjectNode(CodeTextViewNode& nonCoffSummaryNode,
      const AnonymousObjectHeader& anonymousObjectHeader);

private:
   /// file to load non-COFF object from
   const File& m_file;

   /// file offset where non-COFF object starts
   size_t m_fileOffset;

   /// object file summary text
   CString m_objectFileSummary;
};
