//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file FilterSortListViewForm.cpp
/// \brief form with filter edit box and filterable and sortable list view
//
#include "stdafx.h"
#include "FilterSortListViewForm.hpp"

FilterSortListViewForm::FilterSortListViewForm(const std::vector<CString>& columnsList,
   const std::vector<std::vector<CString>>& data)
   :m_listViewData(columnsList, data)
{
}

LRESULT FilterSortListViewForm::OnInitDialog(
   UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);
   DlgResize_Init();

   return FALSE; // don't steal focus from e.g. the tree
}

LRESULT FilterSortListViewForm::OnChangeEditFilterText(
   WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString filterText;
   m_filterEditBox.GetWindowText(filterText);

   m_listViewData.SetFilterText(filterText);

   return 0;
}
