//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file FilterSortListViewForm.hpp
/// \brief form with filter edit box and filterable and sortable list view
//
#pragma once

#include "resource.h"
#include "modules/IContentView.hpp"
#include "FilterSortListView.hpp"

/// Dialog form that shows a filter edit box and the filter sort list view
/// \details This form combines an edit box and the list view to filter the
/// table for data.
class FilterSortListViewForm :
   public CDialogImpl<FilterSortListViewForm>,
   public CWinDataExchange<FilterSortListViewForm>,
   public CDialogResize<FilterSortListViewForm>,
   public IContentView
{
public:
   /// ctor
   FilterSortListViewForm(const std::vector<CString>& columnsList,
      const std::vector<std::vector<CString>>& data);

   /// dialog ID
   enum { IDD = IDD_FILTER_SORT_LISTVIEW_FORM };

   /// called to translate dialog messages
   BOOL PreTranslateMessage(MSG* msg)
   {
      return IsDialogMessage(msg);
   }

   // Inherited via IContentView
   virtual HWND CreateView(HWND parent) override
   {
      return Create(parent, rcDefault);
   }

   virtual BOOL ProcessWindowMessage(HWND /*hWnd*/, UINT /*uMsg*/,
      WPARAM /*wParam*/, LPARAM /*lParam*/, LRESULT& /*lResult*/) override
   {
      return false;
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   friend CDialogResize<FilterSortListViewForm>;

   BEGIN_DDX_MAP(FilterSortListViewForm)
      DDX_CONTROL_HANDLE(IDC_EDIT_FILTERTEXT, m_filterEditBox)
      DDX_CONTROL(IDC_LISTVIEW_FILTER, m_listViewData)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(FilterSortListViewForm)
      DLGRESIZE_CONTROL(IDC_EDIT_FILTERTEXT, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_LISTVIEW_FILTER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_MSG_MAP(FilterSortListViewForm)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_HANDLER(IDC_EDIT_FILTERTEXT, EN_CHANGE, OnChangeEditFilterText)
      CHAIN_MSG_MAP(CDialogResize<FilterSortListViewForm>)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when dialog is initialized
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the user changed the filter text
   LRESULT OnChangeEditFilterText(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
   /// edit box to filter for text
   CEdit m_filterEditBox;

   /// list view with data to use for filtering
   FilterSortListView m_listViewData;
};
