//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file FilterSortListView.hpp
/// \brief list view that can be filtered and sorted
//
#pragma once

#include "modules/IContentView.hpp"

/// window traits for the filter sort list view
typedef CWinTraitsOR<LVS_REPORT | LVS_OWNERDATA, LVS_EX_FULLROWSELECT> FilterSortListViewWinTraits;

/// \brief List view that can be sorted and filtered by text
/// \details Content of files often contain long lists of data where the
/// programmer can't immediately find the necessarry infos. Therefore this
/// list view provides sorting and filtering of tabular data. Changing sorting
/// is directly handled in the list view, but the filter text can be set from
/// outside, e.g. when an CEdit control is updated.
/// Note that while the data is a 2D array (actually, vector of vector), the
/// data must not strictly a fully filled 2D array, but may be a jagged 2D
/// array. Data for which there is no column specified isn't displayed.
class FilterSortListView :
   public CWindowImpl<FilterSortListView, CListViewCtrl, FilterSortListViewWinTraits>,
   public IContentView
{
   /// base class
   typedef CWindowImpl<FilterSortListView, CListViewCtrl, FilterSortListViewWinTraits> baseClass;

public:
   /// ctor
   FilterSortListView(std::vector<CString> columnsList,
      std::vector<std::vector<CString>> data)
      :m_columnsList(columnsList),
      m_data(data)
   {
   }

   DECLARE_WND_SUPERCLASS(nullptr, CListViewCtrl::GetWndClassName())

   /// called when the view is subclassed instead of created
   BOOL SubclassWindow(HWND hWnd);

   // Inherited via IContentView
   virtual HWND CreateView(HWND parent) override
   {
      return Create(parent, rcDefault);
   }

   virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override
   {
      return FilterSortListView::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0UL);
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

   /// sets new filter text
   void SetFilterText(const CString& filterText);

private:
   BEGIN_MSG_MAP(FilterSortListView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
      NOTIFY_CODE_HANDLER(LVN_ODFINDITEM, OnOwnerDataFindItem)
      NOTIFY_CODE_HANDLER(HDN_ITEMCLICK, OnHeaderItemClick)
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_ODFINDITEM, OnOwnerDataFindItem)
      REFLECTED_NOTIFY_CODE_HANDLER(HDN_ITEMCLICK, OnHeaderItemClick)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when the window is about to be created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when virtual list view needs a list view cell's content
   LRESULT OnGetDispInfo(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   /// called when the virtual list view tries to find an item by text
   LRESULT OnOwnerDataFindItem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   /// called when the user clciked on a header item, in order to change the sorting
   LRESULT OnHeaderItemClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   /// sets the sort flag for a column
   void SetColumnHeaderSortFlag(int columnIndex, UINT sortFlagToSet);

   /// initializes list with struct definition and content
   void InitList();

   /// applies filtering and sorting by recalculating the mapping
   void ApplyFilterAndSorting();

   /// returns a list of column texts and their data indices, already filtered
   /// by the filter text
   void GetFilteredColumnList(
      std::vector<std::pair<CString, size_t>>& columnTextToDataIndexList);

   /// sorts column list
   void SortColumnList(
      std::vector<std::pair<CString, size_t>>& columnTextToDataIndexList);

private:
   /// list of column names
   std::vector<CString> m_columnsList;

   /// all data to display, first by line, then by column
   std::vector<std::vector<CString>> m_data;

   /// filter text; must appear somewhere in any column
   CString m_filterText;

   /// number of sort column
   int m_sortColumn = 0;

   /// true when sorting in ascending order, or false when descending order
   bool m_sortAscending = true;

   /// mapping from list view display index to data index
   std::map<int, size_t> m_displayIndexToDataIndexMapping;
};
