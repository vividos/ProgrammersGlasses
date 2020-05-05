//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file File.hpp
/// \brief memory mapped file
//
#pragma once

/// \brief Memory mapped file
/// \details Maps an existing file into virtual memory to access the file data.
/// The file is mapped read-only and can't be modified.
class File
{
public:
   /// opens file and maps it into memory
   explicit File(const CString& filename);

   /// returns filename of mapped file
   const CString& Filename() const { return m_filename; }

   /// returns if the file is available at all
   bool IsAvail() const;

   /// returns pointer to the memory mapped file
   const void* Data() const { return m_data.get(); }

   /// returns size of memory mapped file
   size_t Size() const { return m_size; }

private:
   /// file name of mapped file
   CString m_filename;

   /// file handle
   std::shared_ptr<void> m_file;

   /// mapping handle
   std::shared_ptr<void> m_mapping;

   /// pointer to the memory mapped file
   std::shared_ptr<const void> m_data;

   /// size of memory mapped file
   SIZE_T m_size;
};
