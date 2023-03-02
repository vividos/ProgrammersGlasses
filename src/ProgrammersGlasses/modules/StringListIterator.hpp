//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file StringListIterator.hpp
/// \brief Iterator helper class for string lists
//
#pragma once

#include "File.hpp"

/// \brief Iterator for string lists in memory
/// The class helps in iterating lists of strings that are null terminated
/// and may end with a double-null character or at a specific offset.
class StringListIterator
{
public:
   /// ctor
   StringListIterator(const File& file,
      size_t startOffset, size_t maximumSize, bool endsWithDoubleNull)
      :m_file(file),
      m_currentOffset(startOffset),
      m_endOffset(startOffset + maximumSize),
      m_endsWithDoubleNull(endsWithDoubleNull)
   {
   }

   /// returns offset of current string
   size_t Offset() const
   {
      return m_currentOffset;
   }

   /// returns the currently pointed to string
   CString Current() const
   {
      const CHAR* text = m_file.Data<CHAR>(m_currentOffset);
      size_t leftSize = m_endOffset - m_currentOffset;
      size_t length = strnlen(text, leftSize);

      return CString(text, static_cast<int>(length));
   }

   /// returns if iterator is at the end
   bool IsAtEnd() const
   {
      if (m_currentOffset >= m_endOffset)
         return true;

      const CHAR* text = m_file.Data<CHAR>(m_currentOffset);
      if (m_endsWithDoubleNull &&
         text[0] == 0)
      {
         return true;
      }

      return false;
   }

   /// advances to the next string
   bool Next()
   {
      if (IsAtEnd())
         return false;

      const CHAR* text = m_file.Data<CHAR>(m_currentOffset);

      size_t leftSize = m_endOffset - m_currentOffset;
      size_t length = strnlen(text, leftSize);

      m_currentOffset = length + 1;
      return true;
   }

private:
   /// file to get string from
   const File& m_file;

   /// current file offset
   size_t m_currentOffset;

   /// block end offset
   size_t m_endOffset;

   /// when true, the string list ends with a double null character
   bool m_endsWithDoubleNull;
};
