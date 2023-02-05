//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file Helper.hpp
/// \brief helper functions
//
#pragma once

#include <vector>
#include <map>

/// Splits a string by one or more split characters
std::vector<CString> StringSplit(const CString& text, LPCTSTR splitChars, bool addEmptyEntries);

/// indents a multiline string with space characters
void IndentText(CString& text, int numberOfSpaces);

/// checks if the text to search contains the text to find somewhere, using
/// case independent comparison
bool ContainsIgnoreCase(const CString& textToSearch, const CString& textToFind);

/// Returns value from a std::map with given key, or a default value when the key is not found in the map
template <typename T>
inline LPCTSTR GetValueFromMapOrDefault(const std::map<T, LPCTSTR>& map, const T key, LPCTSTR defaultValue)
{
   auto iter = map.find(key);
   return iter != map.end() ? iter->second : defaultValue;
}

/// gets a BYTE, WORD or DWORD value from a byte buffer, with endianness
DWORD GetBufferValueWithEndianness(const BYTE* buffer, size_t valueSize, bool littleEndian);

/// template function to swap endianness
template <typename T>
T SwapEndianness(T);

/// swaps 16-bit words from/to LE to BE
template <>
inline WORD SwapEndianness(WORD value)
{
   return ((value >> 8) & 0xFF) | ((value & 0xFF) << 8);
}

/// swaps 32-bit dwords from/to LE to BE
template <>
inline DWORD SwapEndianness(DWORD value)
{
   return ((value & 0xff000000) >> 24) |
      ((value & 0x00ff0000) >> 8) |
      ((value & 0x0000ff00) << 8) |
      ((value & 0x000000ff) << 24);
}

/// returns bitfield bits from given value
template <typename T>
inline T GetBits(T value, size_t start, size_t count)
{
   ATLASSERT(
      start < sizeof(T) * 8 &&
      start + count <= sizeof(T) * 8);

   return (value >> start) & ((1UL << count) - 1);
}
