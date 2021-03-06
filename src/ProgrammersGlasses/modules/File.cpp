//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file File.cpp
/// \brief memory mapped file
//
#include "stdafx.h"
#include "File.hpp"
#include <algorithm>

File::File(const CString& filename)
   :m_filename(filename),
   m_data(nullptr),
   m_size(0)
{
   HANDLE file = CreateFile(filename,
      GENERIC_READ,
      FILE_SHARE_READ, // only allow read for other processes
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

   if (file == INVALID_HANDLE_VALUE)
      return;

   m_file.reset(file, CloseHandle);

   HANDLE mapping = CreateFileMapping(file,
      nullptr,
      PAGE_READONLY,
      0, 0, // max. size
      nullptr);

   if (mapping == nullptr)
      return;

   m_mapping.reset(mapping, CloseHandle);

   LPVOID ptr = MapViewOfFile(mapping,
      FILE_MAP_READ,
      0, 0, // start offset
      0); // map all bytes

   MEMORY_BASIC_INFORMATION buffer = {};
   VirtualQuery(ptr, &buffer, sizeof(buffer));
   m_size = buffer.RegionSize;

   // restrict file size further by getting file size
   LARGE_INTEGER fileSize = {};
   if (GetFileSizeEx(file, &fileSize))
      m_size = std::min(m_size, static_cast<size_t>(fileSize.QuadPart));

   m_data.reset(ptr, UnmapViewOfFile);
}

bool File::IsAvail() const
{
   return m_file != nullptr &&
      m_mapping != nullptr &&
      m_data != nullptr;
}

bool File::IsValidPointer(const void* ptr) const
{
   const BYTE* endFilePtr = reinterpret_cast<const BYTE*>(m_data.get()) + m_size;
   const BYTE* bytePtr = reinterpret_cast<const BYTE*>(ptr);

   return
      bytePtr >= m_data.get() &&
      bytePtr < endFilePtr;
}

bool File::IsValidRange(const void* ptr, size_t size) const
{
   const BYTE* endPtr = reinterpret_cast<const BYTE*>(ptr) + size - 1;

   return
      IsValidPointer(endPtr) &&
      IsValidPointer(ptr);
}
