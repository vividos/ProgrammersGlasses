//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file CoffObjectNodeTreeBuilder.hpp
/// \brief Node tree builder for for COFF object files
//
#pragma once

#include "INode.hpp"
#include "File.hpp"
#include <map>

class CodeTextViewNode;
class StaticNode;
struct CoffHeader;

/// Node tree builder for COFF objects
class CoffObjectNodeTreeBuilder
{
public:
   /// ctor
   CoffObjectNodeTreeBuilder(const File& file, size_t fileOffset, bool isImage);

   /// adds COFF header, section and symbol tables
   std::shared_ptr<INode> BuildCoffObjectNode();

   /// returns object file summary text
   const CString& GetObjectFileSummary() const { return m_objectFileSummary; }

private:
   /// adds COFF header, section and symbol tables
   void AddCoffObjectFile(CodeTextViewNode& coffSummaryNode);

   /// adds summary text to node
   void AddCoffHeaderSummaryText(CodeTextViewNode& node) const;

   /// adds section table to node
   void AddSectionTable(StaticNode& coffSummaryNode);

   /// loads string table into map
   void LoadStringTable();

   /// adds symbol table to node
   void AddSymbolTable(StaticNode& coffSummaryNode);

   /// adds string table to node
   void AddStringTable(StaticNode& stringTableSummaryNode);

private:
   /// file to load COFF object from
   const File& m_file;

   /// file offset where COFF object starts
   size_t m_fileOffset;

   /// indicates if the COFF object is inside an image (vs. in an object file)
   bool m_isImage;

   /// COFF object header
   const CoffHeader& m_coffObjectHeader;

   /// string table mapping from string offset to text
   std::map<size_t, CString> m_offsetToStringMapping;

   /// object file summary text
   CString m_objectFileSummary;
};
