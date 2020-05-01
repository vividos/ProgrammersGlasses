//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file AboutDlg.cpp
/// \brief about dialog
//
#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.hpp"

LRESULT AboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CenterWindow(GetParent());
   return TRUE;
}
