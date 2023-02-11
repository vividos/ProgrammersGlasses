//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
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

   /// returns const pointer to a array of given type, in the memory mapped file
   template <typename T>
   const T* Data(size_t fileOffset = 0) const
   {
      return reinterpret_cast<const T*>(
         reinterpret_cast<const BYTE*>(m_data.get()) + fileOffset);
   }

   /// returns size of memory mapped file
   size_t Size() const { return m_size; }

   /// checks if given pointer lies inside the mapped file
   bool IsValidPointer(const void* ptr) const;

   /// checks if given area (by pointer and byte size) lies completely inside
   /// the mapped file
   bool IsValidRange(const void* ptr, size_t size) const;

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
