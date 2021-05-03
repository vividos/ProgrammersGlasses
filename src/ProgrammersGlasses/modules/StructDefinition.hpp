//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2021 Michael Fink
//
/// \file StructDefinition.hpp
/// \brief structure definition
//
#pragma once

#include <vector>
#include <functional>
#include <optional>

/// structure field type
enum class StructFieldType
{
   unsignedInteger,  ///< unsigned integer of various size (allowed are 1, 2, 4 and 8 byte values)
   byteArray,        ///< byte array with values of specific size
   text,             ///< text, with fixed length (allowed are 1 and 2 byte values), possibly not null terminated
   flagsMapping,     ///< flags value, with custom mapping
   valueMapping,     ///< value, with custom mapping
   custom,           ///< custom field that has its own formatter
};

/// describes a single structure field
struct StructField
{
   size_t m_offset;     ///< offset from struct start
   size_t m_length;     ///< length of field, in bytes
   size_t m_valueSize;  ///< value size, for the byteArray and text field type
   bool m_littleEndian; ///< indicates if the source value is litte endian or big endian
   StructFieldType m_type;   ///< field type

   /// custom formatter; used when type is StructFieldType::custom
   std::function<CString(LPCVOID, size_t)> m_formatter;

   /// flags or value mapping; used when type is StructFieldType::flagsMapping or valueMapping
   std::optional<std::reference_wrapper<const std::map<DWORD, LPCTSTR>>> m_valueMapping;

   LPCTSTR m_description;    ///< field description text

   /// ctor; initializes field without formatter
   StructField(size_t offset, size_t length, size_t valueSize, bool littleEndian, StructFieldType type, LPCTSTR description = nullptr)
      :m_offset(offset),
      m_length(length),
      m_valueSize(valueSize),
      m_littleEndian(littleEndian),
      m_type(type),
      m_description(description)
   {
      ATLASSERT(valueSize <= length); // valueSize can't be larger than length
   }

   /// ctor; initializes field with custom formatter
   StructField(size_t offset, size_t length, size_t valueSize,
      bool littleEndian,
      std::function<CString(LPCVOID, size_t)> formatter,
      LPCTSTR description = nullptr)
      :m_offset(offset),
      m_length(length),
      m_valueSize(valueSize),
      m_littleEndian(littleEndian),
      m_type(StructFieldType::custom),
      m_formatter(formatter),
      m_description(description)
   {
   }

   /// ctor; initializes field with flags or value mapping
   StructField(size_t offset, size_t length, size_t valueSize,
      bool littleEndian,
      StructFieldType type,
      const std::map<DWORD, LPCTSTR>& flagsOrValueMapping,
      LPCTSTR description = nullptr)
      :m_offset(offset),
      m_length(length),
      m_valueSize(valueSize),
      m_littleEndian(littleEndian),
      m_type(type),
      m_valueMapping(std::make_optional(std::cref(flagsOrValueMapping))),
      m_description(description)
   {
      ATLASSERT(type == StructFieldType::flagsMapping ||
         type == StructFieldType::valueMapping); // can only specify flagsMapping or valueMapping
   }
};

/// \brief Definition of a data structure
/// \details Defines all fields of a structure to display.
class StructDefinition
{
public:
   /// Default ctor; creates a struct definition with an empty field list that can be added to
   StructDefinition()
   {
   }

   /// Ctor; creates a struct definition with a field list passed as initializer list
   StructDefinition(const std::initializer_list<StructField>& fieldList)
      :m_fieldList(fieldList)
   {
   }

   /// Ctor; extends struct definition with an extra initializer list containing more fields
   StructDefinition(const StructDefinition& structDefinition, const std::initializer_list<StructField>& extraFieldList)
      :m_fieldList(structDefinition.FieldList())
   {
      for (auto field : extraFieldList)
         AddField(field);
   }

   /// Adds structure field
   void AddField(const StructField& field)
   {
      m_fieldList.push_back(field);
   }

   /// Returns the field list
   const std::vector<StructField>& FieldList() const { return m_fieldList; }

   /// Returns maximum offset of any field in the field list
   size_t GetMaxStructFieldOffset() const
   {
      size_t maxOffset = 0;
      for (const StructField& field : m_fieldList)
         maxOffset = std::max(maxOffset, field.m_offset + field.m_length);

      return maxOffset;
   }

private:
   /// Field list
   std::vector<StructField> m_fieldList;
};
