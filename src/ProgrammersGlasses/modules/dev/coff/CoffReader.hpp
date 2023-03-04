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
   friend class PortableExecutableReader;

   /// loads COFF based object files
   void LoadCoffObjectFile();

   /// loads non-COFF (import or anonymous) object files
   void LoadNonCoffObjectFile();

   /// adds a non-COFF (import or anonymous) object file to node
   void AddNonCoffObjectFile(CodeTextViewNode& nonCoffSummaryNode,
      size_t fileOffset, CString& objectFileSummary) const;

   /// adds archive linker member to node
   void AddArchiveLinkerMember(StaticNode& archiveMemberNode,
      size_t archiveMemberIndex, size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary) const;

   /// adds first linker member node
   void AddFirstLinkerMemberNode(StaticNode& archiveMemberNode,
      size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary) const;

   /// adds second linker member node
   void AddSecondLinkerMemberNode(StaticNode& archiveMemberNode,
      size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary) const;

   /// adds longnames linker member node
   void AddArchiveLongnamesMember(StaticNode& archiveMemberNode,
      size_t fileOffset, size_t linkerMemberSize,
      CString& linkerMemberSummary) const;

   /// loads archive library files
   void LoadArchiveLibraryFile();

   /// file to read from
   File m_file;

   /// root node
   std::shared_ptr<INode> m_rootNode;
};
