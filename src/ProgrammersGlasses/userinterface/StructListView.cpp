//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file StructListView.cpp
/// \brief list view showing data structure fields
//
#include "stdafx.h"
#include "StructListView.hpp"
#include "modules/StructDefinition.hpp"
#include "modules/DisplayFormatHelper.hpp"
#include "StringHelper.hpp"

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

   InsertColumn(0, _T("Offset"), LVCFMT_LEFT, 200);
   InsertColumn(1, _T("Raw data"), LVCFMT_LEFT, 200);
   InsertColumn(2, _T("Value"), LVCFMT_LEFT, 200);
   InsertColumn(3, _T("Description"), LVCFMT_LEFT, 200);

   for (auto structField : m_structDefinition.FieldList())
   {
      const BYTE* address = structBase + structField.m_offset;

      CString addressText;
      size_t offset = address - fileBase;
      addressText.Format(useShortOffset ? _T("0x%08zx") : _T("0x%016zx"), offset);

      int itemIndex = InsertItem(GetItemCount(), addressText);

      CString rawDataText = DisplayFormatHelper::FormatRawData(
         address,
         structField.m_length,
         structField.m_valueSize,
         structField.m_littleEndian,
         8);

      std::vector<CString> rawDataMultilineTexts;
      if (rawDataText.Find(_T('\n')) > 0)
      {
         rawDataMultilineTexts = StringSplit(rawDataText, _T("\n"), false);
         SetItemText(itemIndex, 1, rawDataMultilineTexts[0]);
         rawDataMultilineTexts.erase(rawDataMultilineTexts.begin());
      }
      else
         SetItemText(itemIndex, 1, rawDataText);

      CString formattedValue =
         DisplayFormatHelper::FormatValue(structField, address, 8);

      std::vector<CString> formattedValueMultilineTexts;

      if (formattedValue.Find(_T('\n')) > 0)
      {
         formattedValueMultilineTexts = StringSplit(formattedValue, _T("\n"), false);
         SetItemText(itemIndex, 2, formattedValueMultilineTexts[0]);
         formattedValueMultilineTexts.erase(formattedValueMultilineTexts.begin());
      }
      else
         SetItemText(itemIndex, 2, formattedValue);

      if (structField.m_description != nullptr)
         SetItemText(itemIndex, 3, structField.m_description);

      if (!rawDataMultilineTexts.empty() ||
         !formattedValueMultilineTexts.empty())
      {
         size_t maxIndex = std::max(rawDataMultilineTexts.size(), formattedValueMultilineTexts.size());
         for (size_t index = 0; index < maxIndex; index++)
         {
            int partItemIndex = InsertItem(GetItemCount(), _T(""));

            if (index < rawDataMultilineTexts.size())
               SetItemText(partItemIndex, 1, rawDataMultilineTexts[index]);

            if (index < formattedValueMultilineTexts.size())
               SetItemText(partItemIndex, 2, formattedValueMultilineTexts[index]);
         }
      }
   }

   SetColumnWidth(0, LVSCW_AUTOSIZE);
   SetColumnWidth(1, LVSCW_AUTOSIZE);
   SetColumnWidth(2, LVSCW_AUTOSIZE);
   SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);

   SetExtendedListViewStyle(GetWndExStyle(0));
}
