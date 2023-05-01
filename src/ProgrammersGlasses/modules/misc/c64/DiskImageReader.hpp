//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImageReader.hpp
/// \brief C64 disk image reader
//
#pragma once

#include "modules/IReader.hpp"
#include "DiskImage.hpp"

class StaticNode;

/// \brief C64 disk image reader
class DiskImageReader : public IReader
{
public:
   /// determines if given file is a C64 disk image file
   static bool IsDiskImageFile(const File& file);

   /// ctor
   explicit DiskImageReader(const File& file);

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
   /// adds disk image directory
   void AddDirectoryText(CString& directoryText) const;

   /// converts PETASCII to readable ASCII text
   static CString GetAsciiFromPetascii(const BYTE* data, size_t size);

   /// formats the file type byte as text
   static CString FormatFileType(BYTE fileType, bool isD81);

   /// adds the BAM (block availability map) to given node
   void AddBlockAvailabilityMap(std::shared_ptr<StaticNode> node) const;

private:
   /// raw disk image file
   File m_file;

   /// disk image file
   DiskImage m_image;

   /// the root node
   std::shared_ptr<INode> m_rootNode;
};
