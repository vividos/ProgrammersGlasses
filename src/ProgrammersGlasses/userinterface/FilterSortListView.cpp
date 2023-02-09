//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file FilterSortListView.cpp
/// \brief list view that can be filtered and sorted
//
#include "stdafx.h"
#include "FilterSortListView.hpp"
#include "Helper.hpp"
#include <algorithm>

void FilterSortListView::SetFilterText(const CString& filterText)
{
   m_filterText = filterText;

   ApplyFilterAndSorting();
   RedrawItems(0, GetItemCount());
}

LRESULT FilterSortListView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT ret = DefWindowProc(uMsg, wParam, lParam);
   bHandled = TRUE;

   InitList();

   return ret;
}

LRESULT FilterSortListView::OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   if (pnmh->hwndFrom != m_hWnd) return 0;

   const LVITEM& item = ((NMLVDISPINFO*)pnmh)->item;

   if (item.mask & LVIF_TEXT)
   {
      int displayIndex = item.iItem;
      auto iter = m_displayIndexToDataIndexMapping.find(displayIndex);
      ATLASSERT(iter != m_displayIndexToDataIndexMapping.end());

      size_t dataIndex = iter->second;
      ATLASSERT(dataIndex < m_data.size());

      const auto& line = m_data[dataIndex];

      size_t columnIndex = item.iSubItem;

      CString text;
      if (columnIndex < line.size())
         text = line[columnIndex];

      ATLVERIFY(nullptr != lstrcpyn(item.pszText, text.GetString(), item.cchTextMax));
   }

   return 0;
}

LRESULT FilterSortListView::OnOwnerDataFindItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   const NMLVFINDITEM& findInfo = *(NMLVFINDITEM*)pnmh;

   if ((findInfo.lvfi.flags & LVFI_STRING) == 0)
      return -1;

   CString textToFind = findInfo.lvfi.psz;

   int startDisplayIndex = findInfo.iStart;
   if (startDisplayIndex >= GetItemCount())
      startDisplayIndex = 0;

   int currentDisplayIndex = startDisplayIndex;
   do
   {
      // find in line
      auto iter = m_displayIndexToDataIndexMapping.find(currentDisplayIndex);
      ATLASSERT(iter != m_displayIndexToDataIndexMapping.end());

      size_t dataIndex = iter->second;
      ATLASSERT(dataIndex < m_data.size());

      const auto& line = m_data[dataIndex];

      // search in all cells, not just the first
      for (const auto& cellText : line)
         if (ContainsIgnoreCase(cellText, textToFind))
            return currentDisplayIndex;

      currentDisplayIndex++;
      if (currentDisplayIndex >= GetItemCount())
         currentDisplayIndex = 0;

   } while (currentDisplayIndex != startDisplayIndex);

   // no match found
   return -1;
}

LRESULT FilterSortListView::OnHeaderItemClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   // ignore hdn.iButton; sorting can be changed with either left or right button
   const NMHEADER& hdn = *(NMHEADER*)pnmh;

   if (hdn.iItem == m_sortColumn)
      m_sortAscending = !m_sortAscending;
   else
   {
      SetColumnHeaderSortFlag(m_sortColumn, 0);

      m_sortAscending = true;
      m_sortColumn = hdn.iItem;
   }

   SetColumnHeaderSortFlag(m_sortColumn,
      m_sortAscending ? HDF_SORTUP : HDF_SORTDOWN);

   ApplyFilterAndSorting();
   RedrawItems(0, GetItemCount());

   return 0;
}

void FilterSortListView::SetColumnHeaderSortFlag(int columnIndex, UINT sortFlagToSet)
{
   ATLASSERT(sortFlagToSet == 0 ||
      (sortFlagToSet & (HDF_SORTUP | HDF_SORTDOWN)) != 0);

   CHeaderCtrl header = GetHeader();

   HDITEM headerItem = {};
   headerItem.mask = HDI_FORMAT;
   header.GetItem(columnIndex, &headerItem);

   headerItem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
   headerItem.fmt |= sortFlagToSet;

   header.SetItem(columnIndex, &headerItem);
}

void FilterSortListView::InitList()
{
   SetUnicodeFormat();

   CLogFont logFont;
   logFont.SetHeight(12);
   _tcscpy_s(logFont.lfFaceName, _T("Consolas"));
   CFontHandle codeFont = logFont.CreateFontIndirect();

   SetFont(codeFont);

   int columnIndex = 0;
   for (const auto& columnName : m_columnsList)
      InsertColumn(columnIndex++, columnName, LVCFMT_LEFT, 100);

   SetColumnHeaderSortFlag(m_sortColumn,
      m_sortAscending ? HDF_SORTUP : HDF_SORTDOWN);

   ApplyFilterAndSorting();

   for (int columnIndexForWidth = 0; columnIndexForWidth < columnIndex; columnIndexForWidth++)
      SetColumnWidth(columnIndexForWidth, LVSCW_AUTOSIZE_USEHEADER);

   SetExtendedListViewStyle(GetWndExStyle(0));
}

void FilterSortListView::ApplyFilterAndSorting()
{
   std::vector<std::pair<CString, size_t>> columnTextToDataIndexList;

   GetFilteredColumnList(columnTextToDataIndexList);
   SortColumnList(columnTextToDataIndexList);

   // fill the mapping
   m_displayIndexToDataIndexMapping.clear();

   int listViewIndex = 0;
   for (const auto& pair : columnTextToDataIndexList)
   {
      m_displayIndexToDataIndexMapping.insert(
         std::make_pair(listViewIndex++, pair.second));
   }

   CListViewCtrl::SetItemCount(listViewIndex);
}

void FilterSortListView::GetFilteredColumnList(
   std::vector<std::pair<CString, size_t>>& columnTextToDataIndexList)
{
   for (size_t dataLineIndex = 0, maxDataLineIndex = m_data.size();
      dataLineIndex < maxDataLineIndex;
      dataLineIndex++)
   {
      const auto& dataLine = m_data[dataLineIndex];

      if (!m_filterText.IsEmpty())
      {
         bool found = false;
         for (const auto& dataColumn : dataLine)
            if (ContainsIgnoreCase(dataColumn, m_filterText))
            {
               found = true;
               break;
            }

         if (!found)
            continue;
      }

      CString columnText;

      // not all columns might be filled
      if (m_sortColumn < dataLine.size())
         columnText = dataLine[m_sortColumn];

      columnTextToDataIndexList.push_back(std::make_pair(columnText, dataLineIndex));
   }
}

void FilterSortListView::SortColumnList(
   std::vector<std::pair<CString, size_t>>& columnTextToDataIndexList)
{
   bool sortAscending = m_sortAscending;

   auto pred = [sortAscending](const auto& left, const auto& right)
   {
      // use natural sort from the shell here
      int result = StrCmpLogicalW(left.first, right.first);

      return sortAscending
         ? result < 0
         : result > 0;
   };

   std::stable_sort(
      columnTextToDataIndexList.begin(),
      columnTextToDataIndexList.end(),
      pred);
}
