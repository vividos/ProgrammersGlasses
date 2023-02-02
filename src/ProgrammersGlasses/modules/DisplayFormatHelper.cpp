//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file DisplayFormatHelper.cpp
/// \brief display format helper
//
#include "stdafx.h"
#include "DisplayFormatHelper.hpp"
#include "StructDefinition.hpp"

CString DisplayFormatHelper::FormatDateTime(time_t time)
{
   struct tm tm = {};
   CString text;
   if (0 == ::localtime_s(&tm, &time))
   {
      text.Format(_T("%04i-%02i-%02i %02i:%02i:%02i"),
         tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
         tm.tm_hour, tm.tm_min, tm.tm_sec);
   }

   return text;
}

CString DisplayFormatHelper::FormatBitFlagsFromMap(const std::map<DWORD, LPCTSTR>& bitflagsMap, DWORD flags)
{
   CString text;
   DWORD unmappedFlags = 0;
   for (unsigned int bits = 1; bits < 32; bits++)
   {
      DWORD testFlag = 1 << bits;
      if ((flags & testFlag) == 0)
         continue;

      auto iter = bitflagsMap.find(testFlag);
      if (iter == bitflagsMap.end())
      {
         unmappedFlags |= testFlag;
         continue;
      }

      if (!text.IsEmpty())
         text += _T(" | ");
      text += iter->second;
   }

   if (unmappedFlags != 0)
   {
      if (!text.IsEmpty())
         text += _T(" | ");
      text.AppendFormat(_T("0x%08x"), unmappedFlags);
   }

   if (text.IsEmpty())
      text = _T("N/A");

   return text;
}

CString DisplayFormatHelper::FormatRawData(const BYTE* rawData, size_t length, size_t valueSize, bool littleEndian)
{
   CString text;

   size_t maxOffset = length - (length % valueSize);

   for (size_t offset = 0; offset < maxOffset; offset += valueSize)
   {
      if (offset != 0)
         text.AppendChar(_T(' '));

      DWORD value = GetBufferValueWithEndianness(rawData + offset, valueSize, littleEndian);

      switch (valueSize)
      {
      case 1:
         text.AppendFormat(_T("%02x"), value);
         break;

      case 2:
         text.AppendFormat(_T("%04x"), value);
         break;

      case 4:
         text.AppendFormat(_T("%08x"), value);
         break;

      default:
         ATLASSERT(false); // invalid valueSize value
         break;
      }
   }

   if (maxOffset < length)
   {
      text.AppendChar(_T(' '));
      text += FormatRawData(rawData + maxOffset, length - maxOffset, 1, true);
   }

   return text;
}

CString DisplayFormatHelper::FormatGUID(const BYTE* rawData, size_t length)
{
   if (length != sizeof(GUID))
      return _T("Invalid GUID data length");

   const GUID* guid = reinterpret_cast<const GUID*>(rawData);

   OLECHAR* guidString = nullptr;
   HRESULT hr = ::StringFromCLSID(*guid, &guidString);
   if (FAILED(hr))
      return _T("???");

   CString text{ guidString };

   ::CoTaskMemFree(guidString);

   return text;
}

CString DisplayFormatHelper::FormatValue(const StructField& field, const BYTE* rawData)
{
   CString text;

   switch (field.m_type)
   {
   case StructFieldType::unsignedInteger:
      text = FormatRawData(
         rawData,
         field.m_length,
         field.m_length,
         field.m_littleEndian);
      break;

   case StructFieldType::byteArray:
      text = FormatRawData(
         rawData,
         field.m_length,
         field.m_valueSize,
         field.m_littleEndian);
      break;

   case StructFieldType::text:
      if (field.m_valueSize == 1)
      {
         text = CString{ reinterpret_cast<const CHAR*>(rawData),
            static_cast<int>(field.m_length) };
      }
      else if (field.m_valueSize == 2)
      {
         text = CString{ reinterpret_cast<const WCHAR*>(rawData),
            static_cast<int>(field.m_length / 2) };
      }
      else
      {
         ATLASSERT(false); // invalid valueSize value
         text = _T("invalid");
      }
      break;

   case StructFieldType::flagsMapping:
   case StructFieldType::valueMapping:
      ATLASSERT(field.m_valueMapping.has_value()); // value mapping must have been set
      {
         DWORD flagsValue = GetBufferValueWithEndianness(
            rawData,
            field.m_length,
            field.m_littleEndian);

         if (field.m_type == StructFieldType::flagsMapping)
            text = FormatBitFlagsFromMap(
               field.m_valueMapping.value(),
               flagsValue);
         else
            text = GetValueFromMapOrDefault<DWORD>(
               field.m_valueMapping.value(),
               flagsValue,
               _T("unknown"));
      }
      break;

   case StructFieldType::bitfieldMapping:
      // bitfield mapping must have been set
      ATLASSERT(field.m_bitfieldMapping.has_value());
      text = FormatBitfieldValue(field, rawData);
      break;

   case StructFieldType::custom:
      ATLASSERT(field.m_formatter != nullptr); // field formatter must have been set
      text = field.m_formatter(rawData, field.m_length);
      break;

   default:
      ATLASSERT(false); // invalid field type
      text = _T("invalid struct field type");
      break;
   }

   return text;
}

CString DisplayFormatHelper::FormatBitfieldValue(
   const StructField& field, const BYTE* rawData)
{
   DWORD value = GetBufferValueWithEndianness(
      rawData,
      field.m_length,
      field.m_littleEndian);

   CString text;
   for (const auto& bitfieldDescriptor : field.m_bitfieldMapping.value().get())
   {
      if (!text.IsEmpty())
      {
         text += ", ";
      }

      DWORD bitValue = GetBits(value,
         bitfieldDescriptor.m_startBit,
         bitfieldDescriptor.m_bitCount);

      text.AppendFormat(_T("bits %zu..%zu: "),
         bitfieldDescriptor.m_startBit,
         bitfieldDescriptor.m_startBit + bitfieldDescriptor.m_bitCount - 1);

      if (bitfieldDescriptor.m_type == StructFieldType::flagsMapping)
      {
         text += FormatBitFlagsFromMap(
            bitfieldDescriptor.m_flagsOrValueMapping.value(),
            bitValue);
      }
      else if (bitfieldDescriptor.m_type == StructFieldType::valueMapping)
      {
         text += GetValueFromMapOrDefault<DWORD>(
            bitfieldDescriptor.m_flagsOrValueMapping.value(),
            bitValue,
            _T("unknown"));
      }
      else
      {
         if (bitValue == 0)
            text += _T("0x0");
         else
         {
            CString unsignedValueText;
            unsignedValueText.Format(_T("%08x"), bitValue);
            unsignedValueText.TrimLeft(_T('0'));

            text += _T("0x") + unsignedValueText;
         }
      }
   }

   return text;
}
