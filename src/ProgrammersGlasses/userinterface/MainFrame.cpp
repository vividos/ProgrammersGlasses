//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file MainFrame.cpp
/// \brief main frame window
//
#include "stdafx.h"
#include "res/Ribbon.h"
#include "resource.h"
#include "AboutDlg.hpp"
#include "MainFrame.hpp"
#include "NodeAndContentView.hpp"
#include <algorithm>
#include <vector>

BOOL MainFrame::PreTranslateMessage(MSG* msg)
{
   if (baseClass::PreTranslateMessage(msg))
      return TRUE;

   HWND hWnd = MDIGetActive();
   if (hWnd != NULL)
      return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM)msg);

   return FALSE;
}

BOOL MainFrame::OnIdle()
{
   bool hasOpenFile = MDIGetActive() != nullptr;
   UIEnable(ID_FILE_CLOSE, hasOpenFile, false);

   return FALSE;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_commandBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
   m_tabbedClient.UseMDIChildIcon(true);

   CreateSimpleStatusBar();

   CreateMDIClient();

   // subclass MDI client
   m_tabbedClient.SetTabOwnerParent(m_hWnd);
   ATLVERIFY(TRUE == m_tabbedClient.SubclassWindow(m_hWndMDIClient));

   m_commandBar.SetMDIClient(m_hWndMDIClient);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   ShowRibbonUI(true);

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   bHandled = FALSE;
   return 1;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal();
   return 0;
}

LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString filter{ _T("All files (*.*)|*.*||")};

   // exchange pipe char '|' with 0-char for commdlg
   std::vector<TCHAR> buffer { filter.GetString(), filter.GetString() + filter.GetLength() };
   std::replace(buffer.begin(), buffer.end(), _T('|'), _T('\0'));

   CFileDialog dlg{ TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST, buffer.data(), m_hWnd };

   if (IDOK == dlg.DoModal())
      OpenFile(dlg.m_ofn.lpstrFile);

   UpdateLayout();

   return 0;
}

LRESULT MainFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HWND hWnd = MDIGetActive();
   if (hWnd != NULL)
      return (BOOL)::SendMessage(hWnd, WM_CLOSE, 0, 0);

   return 0;
}

void MainFrame::OpenFile(const CString& filename)
{
   NodeAndContentView* view = new NodeAndContentView();
   view->CreateEx(m_hWndClient, rcDefault, Path::FilenameAndExt(filename));

   MDIMaximize(view->m_hWnd);
}
