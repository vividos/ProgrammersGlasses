//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file Helper.hpp
/// \brief helper functions
//
#pragma once

#include <vector>
#include <map>

/// Splits a string by one or more split characters
std::vector<CString> StringSplit(const CString& text, LPCTSTR splitChars, bool addEmptyEntries);

/// Returns value from a std::map with given key, or a default value when the key is not found in the map
template <typename T>
inline LPCTSTR GetValueFromMapOrDefault(const std::map<T, LPCTSTR>& map, const T key, LPCTSTR defaultValue)
{
   auto iter = map.find(key);
   return iter != map.end() ? iter->second : defaultValue;
}
