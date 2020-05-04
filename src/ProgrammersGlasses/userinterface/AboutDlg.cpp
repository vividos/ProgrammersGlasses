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
#include "modules/ModuleManager.hpp"
#include "modules/IModule.hpp"
#include "../../version.hpp"

LRESULT AboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);
   DlgResize_Init();

   CenterWindow(GetParent());

   CString versionText;
   versionText.Format(_T("Version %i.%i.%i Build %i"),
      MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER, BUILD_NUMBER);

   SetDlgItemText(IDC_STATIC_ABOUT_VERSION, versionText);

   m_hyperlinkLicense.SetHyperLink(_T(WEBLINK_LICENSE));
   m_hyperlinkWebpage.SetHyperLink(_T(WEBLINK_PROJECT_PAGE));

   InitModulesList();

   return TRUE;
}

void AboutDlg::InitModulesList()
{
   const UINT firstModuleBitmap = IDB_MODULE_COFF;
   const UINT maxModuleNumber = 256;

   m_listModules.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
   m_listModules.AddColumn(_T(""), 0);

   CImageList imageList;
   imageList.Create(32, 32, ILC_COLOR24, 0, maxModuleNumber);

   for (UINT bitmapId = firstModuleBitmap; bitmapId < firstModuleBitmap + maxModuleNumber; bitmapId++)
   {
      HBITMAP bitmap = AtlLoadBitmapImage(bitmapId);
      if (bitmap == nullptr)
         break;
      imageList.Add(bitmap);
   }

   m_listModules.SetImageList(imageList, LVSIL_SMALL);

   ModuleManager manager;
   for (auto module : manager.GetModuleList())
   {
      int imageIndex = (UINT)module->IconID() - firstModuleBitmap;

      m_listModules.InsertItem(m_listModules.GetItemCount(),
         module->DisplayName(),
         imageIndex);
   }

   m_listModules.SetColumnWidth(0, LVSCW_AUTOSIZE);
}
