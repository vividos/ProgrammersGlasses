//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file StructListView.cpp
/// \brief list view showing data structure fields
//
#include "stdafx.h"
#include "StructListView.hpp"
#include "modules/StructDefinition.hpp"
#include "modules/DisplayFormatHelper.hpp"

LRESULT StructListView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT ret = DefWindowProc(uMsg, wParam, lParam);
   bHandled = TRUE;

   InitList();

   return ret;
}

void StructListView::InitList()
{
   CLogFont logFont;
   logFont.SetHeight(12);
   _tcscpy_s(logFont.lfFaceName, _T("Consolas"));
   CFontHandle codeFont = logFont.CreateFontIndirect();

   SetFont(codeFont);

   InsertColumn(0, _T("Offset"), LVCFMT_LEFT, 100);
   InsertColumn(1, _T("Raw data"), LVCFMT_LEFT, 200);
   InsertColumn(2, _T("Value"), LVCFMT_LEFT, 200);
   InsertColumn(3, _T("Description"), LVCFMT_LEFT, 200);

   const BYTE* base = reinterpret_cast<const BYTE*>(m_baseAddress);

   for (auto structField : m_structDefinition.FieldList())
   {
      const BYTE* address = base + structField.m_offset;

      CString addressText;
      addressText.Format(_T("0x%08x"), address);

      int itemIndex = InsertItem(GetItemCount(), addressText);

      CString rawDataText = FormatRawData(structField, address);
      SetItemText(itemIndex, 1, rawDataText);

      CString formattedValue = FormatValue(structField, address);
      SetItemText(itemIndex, 2, formattedValue);

      if (structField.m_description != nullptr)
         SetItemText(itemIndex, 3, structField.m_description);
   }

   SetColumnWidth(1, LVSCW_AUTOSIZE);
   SetColumnWidth(2, LVSCW_AUTOSIZE);
   SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
}

CString StructListView::FormatRawData(StructField& field, const BYTE* rawData)
{
   return DisplayFormatHelper::FormatRawData(rawData, field.m_length, field.m_valueSize);
}

CString StructListView::FormatValue(StructField& field, const BYTE* rawData)
{
   CString text;

   switch (field.m_type)
   {
   case StructFieldType::unsignedInteger:
      text = DisplayFormatHelper::FormatRawData(rawData, field.m_length, field.m_length);
      break;

   case StructFieldType::custom:
      ATLASSERT(field.m_formatter != nullptr); // field formatter must have been set
      text = field.m_formatter(rawData, field.m_length);
      break;

   case StructFieldType::flagsMapping:
   case StructFieldType::valueMapping:
      ATLASSERT(field.m_valueMapping.has_value()); // value mapping must have been set
      {
         DWORD flagsValue = 0;
         if (field.m_length == 1)
            flagsValue = rawData[0];
         else if (field.m_length == 2)
            flagsValue = *(const WORD*)rawData;
         else if (field.m_length == 4)
            flagsValue = *(const DWORD*)rawData;
         else
            ATLASSERT(false); // invalid field length; must be 1, 2 or 4

         if (field.m_type == StructFieldType::flagsMapping)
            text = DisplayFormatHelper::FormatBitFlagsFromMap(field.m_valueMapping.value(), flagsValue);
         else
            text = GetValueFromMapOrDefault<DWORD>(field.m_valueMapping.value(), flagsValue, _T("unknown"));
      }
      break;

   default:
      // TODO format more types
      ATLASSERT(false); // invalid field type
      text = _T("invalid struct field type");
      break;
   }

   return text;
}