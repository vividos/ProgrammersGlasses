//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file ArchiveFileNodeTreeBuilder.hpp
/// \brief Node tree builder for archive files
//
#pragma once

#include "INode.hpp"
#include "File.hpp"

class StaticNode;

/// Node tree builder for archive files
class ArchiveFileNodeTreeBuilder
{
public:
   /// ctor
   ArchiveFileNodeTreeBuilder(const File& file, size_t fileOffset);

   /// builds archive file node
   std::shared_ptr<INode> BuildArchiveFileNode();

   /// returns object file summary text
   const CString& GetObjectFileSummary() const { return m_objectFileSummary; }

private:
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

private:
   /// file to load archive file from
   const File& m_file;

   /// file offset where archive file starts
   size_t m_fileOffset;

   /// object file summary text
   CString m_objectFileSummary;
};
