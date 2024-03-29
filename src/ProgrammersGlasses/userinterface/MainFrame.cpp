//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file MainFrame.cpp
/// \brief main frame window
//
#include "stdafx.h"
#include "resource.h"
#include "res/Ribbon.h"
#include "AboutDlg.hpp"
#include "MainFrame.hpp"
#include "NodeAndContentView.hpp"
#include <algorithm>
#include <vector>

static const LPCTSTR c_appSettingsRegKey = _T("Software\\ProgrammersView");

BOOL MainFrame::PreTranslateMessage(MSG* msg)
{
   if (baseClass::PreTranslateMessage(msg))
      return TRUE;

   HWND hWnd = MDIGetActive();
   if (hWnd != nullptr)
      return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM)msg);

   return FALSE;
}

BOOL MainFrame::OnIdle()
{
   bool hasOpenFile = MDIGetActive() != nullptr;
   UIEnable(ID_FILE_CLOSE, hasOpenFile, false);

   UIEnable(ID_WINDOW_CASCADE, hasOpenFile, false);
   UIEnable(ID_WINDOW_TILE_HORZ, hasOpenFile, false);
   UIEnable(ID_WINDOW_TILE_VERT, hasOpenFile, false);

   return FALSE;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_commandBar.Create(m_hWnd, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);

   CreateSimpleStatusBar();

   // create MDI client
   m_tabbedClient.UseMDIChildIcon(true);
   m_tabbedClient.SetTabOwnerParent(m_hWnd);
   CreateMDIClient();

   m_commandBar.SetMDIClient(m_hWndMDIClient);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   ShowRibbonUI(true);

   DragAcceptFiles(true);

   m_mru.ReadFromRegistry(c_appSettingsRegKey);

   if (!m_filenamesList.empty())
      PostMessage(WM_OPEN_FILES);

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   bHandled = FALSE;
   return 1;
}

LRESULT MainFrame::OnOpenFiles(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   for (const auto& filename : m_filenamesList)
      OpenFile(filename);

   m_filenamesList.clear();

   return 0;
}

LRESULT MainFrame::OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   HDROP hDropInfo = (HDROP)wParam;
   UINT maxIndex = ::DragQueryFile(hDropInfo, UINT(-1), nullptr, 0);

   CString filename;

   for (UINT index = 0; index < maxIndex; index++)
   {
      ::DragQueryFile(hDropInfo, index, filename.GetBuffer(MAX_PATH), MAX_PATH);
      filename.ReleaseBuffer();

      m_filenamesList.push_back(filename);
   }

   ::DragFinish(hDropInfo);

   PostMessage(WM_OPEN_FILES);

   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) const
{
   AboutDlg dlg{ m_moduleManager };
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
   CString filter{ m_moduleManager.GetAllFilterStrings() };

   filter = AddSupportedFilesFilter(filter);
   filter += _T("All files (*.*)|*.*||");

   // exchange pipe char '|' with 0-char for commdlg
   std::vector<TCHAR> buffer{ filter.GetString(), filter.GetString() + filter.GetLength() };
   std::replace(buffer.begin(), buffer.end(), _T('|'), _T('\0'));

   CFileDialog dlg{ TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, buffer.data(), m_hWnd };

   if (IDOK == dlg.DoModal())
      OpenFile(dlg.m_ofn.lpstrFile);

   UpdateLayout();

   return 0;
}

LRESULT MainFrame::OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString filename;
   if (m_mru.GetFromList(wID, filename))
      OpenFile(filename);

   return 0;
}

LRESULT MainFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HWND hWnd = MDIGetActive();
   if (hWnd != nullptr)
      return (BOOL)::SendMessage(hWnd, WM_CLOSE, 0, 0);

   return 0;
}

void MainFrame::OpenFile(const CString& filename)
{
   if (!Path::FileExists(filename))
   {
      CString message{ _T("Couldn't open file: ") + filename };
      AtlMessageBox(m_hWnd,
         message.GetString(),
         IDR_MAINFRAME,
         MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   if (!m_moduleManager.IsModuleAvailable(filename))
   {
      CString message{ _T("No suitable module found for file: ") + filename };
      AtlMessageBox(m_hWnd,
         message.GetString(),
         IDR_MAINFRAME,
         MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   auto reader = m_moduleManager.LoadFile(filename);

   if (reader == nullptr)
   {
      CString message{ _T("The module couldn't initialize a reader for the file: ") + filename };
      AtlMessageBox(m_hWnd,
         message.GetString(),
         IDR_MAINFRAME,
         MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   // NodeAndContentView deletes itself in OnFinalMessage()
   NodeAndContentView* view = new NodeAndContentView(reader);
   view->CreateEx(m_hWndClient, rcDefault, Path::FilenameAndExt(filename));

   MDIMaximize(view->m_hWnd);

   m_mru.AddToList(filename);
   m_mru.WriteToRegistry(c_appSettingsRegKey);
}

CString MainFrame::AddSupportedFilesFilter(const CString& filter)
{
   std::vector<CString> filterStringsList = StringSplit(filter, _T("|"), true);

   ATLASSERT(filterStringsList.size() % 2 == 0); // filter strings list entries must be even

   CString supportedExtensions;
   for (size_t index = 0; index < filterStringsList.size(); index += 2)
   {
      CString extensions = filterStringsList[index + 1];
      extensions.TrimRight(_T(';'));
      supportedExtensions += extensions + _T(";");
   }

   supportedExtensions.TrimRight(_T(';'));

   return _T("All supported files|") + supportedExtensions + _T("|") + filter;
}
