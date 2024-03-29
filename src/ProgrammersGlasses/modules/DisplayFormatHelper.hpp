//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file DisplayFormatHelper.hpp
/// \brief display format helper
//
#pragma once

#include <ctime>

struct StructField;

/// Helper class to format display text from data
class DisplayFormatHelper
{
public:
   /// formats local date/time
   static CString FormatDateTime(time_t time);

   /// formats a 32-bit flags value with single bits mapped to texts
   static CString FormatBitFlagsFromMap(const std::map<DWORD, LPCTSTR>& bitflagsMap, DWORD flags);

   /// formats raw data as hex values
   static CString FormatRawData(const BYTE* rawData, size_t length, size_t valueSize, bool littleEndian);

   /// formats GUID as human-readable text
   static CString FormatGUID(const BYTE* rawData, size_t length);

   /// formats value using struct field definition
   static CString FormatValue(const StructField& field, const BYTE* rawData);

private:
   /// formats struct field bitfield value
   static CString FormatBitfieldValue(const StructField& field, const BYTE* rawData);
};
