//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
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

   const BYTE* fileBase = reinterpret_cast<const BYTE*>(m_fileBaseAddress);
   const BYTE* structBase = reinterpret_cast<const BYTE*>(m_structBaseAddress);

   size_t maxOffset = (structBase - fileBase) + m_structDefinition.GetMaxStructFieldOffset();
   bool useShortOffset = maxOffset < 0xFFFFFFFF;

   InsertColumn(0, _T("Offset"), LVCFMT_LEFT, useShortOffset ? 110 : 180);
   InsertColumn(1, _T("Raw data"), LVCFMT_LEFT, 200);
   InsertColumn(2, _T("Value"), LVCFMT_LEFT, 200);
   InsertColumn(3, _T("Description"), LVCFMT_LEFT, 200);

   for (auto structField : m_structDefinition.FieldList())
   {
      const BYTE* address = structBase + structField.m_offset;

      CString addressText;
      auto offset = address - fileBase;
      addressText.Format(useShortOffset ? _T("0x%08x") : _T("0x%016x"), offset);

      int itemIndex = InsertItem(GetItemCount(), addressText);

      CString rawDataText = DisplayFormatHelper::FormatRawData(
         address,
         structField.m_length,
         structField.m_valueSize,
         structField.m_littleEndian);
      SetItemText(itemIndex, 1, rawDataText);

      CString formattedValue = DisplayFormatHelper::FormatValue(structField, address);
      SetItemText(itemIndex, 2, formattedValue);

      if (structField.m_description != nullptr)
         SetItemText(itemIndex, 3, structField.m_description);
   }

   SetColumnWidth(1, LVSCW_AUTOSIZE);
   SetColumnWidth(2, LVSCW_AUTOSIZE);
   SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
}
