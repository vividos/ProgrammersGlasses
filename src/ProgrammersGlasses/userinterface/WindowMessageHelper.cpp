//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file WindowMessageHelper.cpp
/// \brief window message helper functions
//
#include "stdafx.h"
#include "WindowMessageHelper.hpp"
#include <atltrace.h>

#ifdef DEBUG

/// defines a map entry, mapping from value to its text
#define MAPENTRY(T) { T, _T( #T ) }

/// message names for some widely used messages
static const std::map<UINT, LPCTSTR> s_messageNames =
{
   // general
   MAPENTRY(WM_NULL),
   MAPENTRY(WM_COMMAND),
   MAPENTRY(WM_SYSCOMMAND),
   MAPENTRY(WM_TIMER),

   // windows and dialogs
   MAPENTRY(WM_CREATE),
   MAPENTRY(WM_DESTROY),
   MAPENTRY(WM_INITDIALOG),
   MAPENTRY(WM_NCCREATE),
   MAPENTRY(WM_SHOWWINDOW),
   MAPENTRY(WM_SETFOCUS),
   MAPENTRY(WM_KILLFOCUS),
   MAPENTRY(WM_GETICON),
   MAPENTRY(WM_SETICON),
   MAPENTRY(WM_SETFONT),
   MAPENTRY(WM_CHILDACTIVATE),

   // notifications
   MAPENTRY(WM_NOTIFY),
   MAPENTRY(WM_NOTIFYFORMAT),
   MAPENTRY(WM_PARENTNOTIFY),
   MAPENTRY(WM_QUERYUISTATE),

   // painting
   MAPENTRY(WM_PAINT),
   MAPENTRY(WM_NCPAINT),
   MAPENTRY(WM_ERASEBKGND),
   MAPENTRY(WM_SETREDRAW),

   // sizing
   MAPENTRY(WM_SIZE),
   MAPENTRY(WM_MOVE),
   MAPENTRY(WM_GETMINMAXINFO),
   MAPENTRY(WM_NCCALCSIZE),
   MAPENTRY(WM_WINDOWPOSCHANGING),
   MAPENTRY(WM_WINDOWPOSCHANGED),

   // control color
   MAPENTRY(WM_CTLCOLORMSGBOX),
   MAPENTRY(WM_CTLCOLOREDIT),
   MAPENTRY(WM_CTLCOLORLISTBOX),
   MAPENTRY(WM_CTLCOLORBTN),
   MAPENTRY(WM_CTLCOLORDLG),
   MAPENTRY(WM_CTLCOLORSCROLLBAR),
   MAPENTRY(WM_CTLCOLORSTATIC),
};

/// returns a readable message name
static CString GetMessageName(UINT uMsg)
{
   CString name;

   // registered message?
   if (uMsg >= 0xc000 && uMsg <= 0xFFFF)
   {
      // https://devblogs.microsoft.com/oldnewthing/20171006-00/?p=97147
      bool gotMessageName =
         GetClipboardFormatName(uMsg, name.GetBuffer(256), 256);

      name.ReleaseBuffer();

      if (gotMessageName && !name.IsEmpty())
         return name;

      return _T("registered message");
   }

   // app wide message
   if (uMsg >= WM_APP)
   {
      name.Format(_T("WM_APP + %u"), uMsg - WM_APP);
      return name;
   }

   // reflected message
   if (uMsg >= OCM__BASE)
   {
      return GetMessageName(uMsg - OCM__BASE) + _T(", reflected");
   }

   // user defined message
   if (uMsg >= WM_USER)
   {
      name.Format(_T("WM_USER + %u"), uMsg - WM_APP);
      return name;
   }

   auto iter = s_messageNames.find(uMsg);
   if (iter != s_messageNames.end())
      return iter->second;

   return _T("unknown");
}

// notify names for some widely used notify codes
static const std::map<UINT, LPCTSTR> s_notifyNames =
{
   // general
   MAPENTRY(NM_CLICK),
   MAPENTRY(NM_DBLCLK),
   MAPENTRY(NM_SETFOCUS),
   MAPENTRY(NM_KILLFOCUS),
   MAPENTRY(NM_CUSTOMDRAW),
   MAPENTRY(NM_SETCURSOR),

   // list view
   MAPENTRY(LVN_ITEMCHANGING),
   MAPENTRY(LVN_ITEMCHANGED),
   MAPENTRY(LVN_INSERTITEM),
   MAPENTRY(LVN_DELETEITEM),
   MAPENTRY(LVN_DELETEALLITEMS),
   MAPENTRY(LVN_BEGINLABELEDIT),
   MAPENTRY(LVN_ENDLABELEDIT),
   MAPENTRY(LVN_COLUMNCLICK),
   MAPENTRY(LVN_BEGINDRAG),
   MAPENTRY(LVN_BEGINRDRAG),
   MAPENTRY(LVN_ODCACHEHINT),
   MAPENTRY(LVN_ODFINDITEM),
   MAPENTRY(LVN_ITEMACTIVATE),
   MAPENTRY(LVN_ODSTATECHANGED),
   MAPENTRY(LVN_HOTTRACK),
   MAPENTRY(LVN_GETDISPINFO),
   MAPENTRY(LVN_SETDISPINFO),

   // tree view
   MAPENTRY(TVN_ITEMCHANGINGA),
   MAPENTRY(TVN_ITEMCHANGINGW),
   MAPENTRY(TVN_ITEMCHANGEDA),
   MAPENTRY(TVN_ITEMCHANGEDW),
   MAPENTRY(TVN_SELCHANGING),
   MAPENTRY(TVN_SELCHANGED),
   MAPENTRY(TVN_GETDISPINFO),
   MAPENTRY(TVN_SETDISPINFO),
   MAPENTRY(TVN_ITEMEXPANDINGA),
   MAPENTRY(TVN_ITEMEXPANDINGW),
   MAPENTRY(TVN_ITEMEXPANDEDA),
   MAPENTRY(TVN_ITEMEXPANDEDW),
   MAPENTRY(TVN_BEGINDRAG),
   MAPENTRY(TVN_BEGINRDRAG),
   MAPENTRY(TVN_DELETEITEM),
   MAPENTRY(TVN_BEGINLABELEDIT),
   MAPENTRY(TVN_ENDLABELEDIT),

   // header view
   MAPENTRY(HDN_ITEMCHANGINGA),
   MAPENTRY(HDN_ITEMCHANGINGW),
   MAPENTRY(HDN_ITEMCHANGEDA),
   MAPENTRY(HDN_ITEMCHANGEDW),
   MAPENTRY(HDN_ITEMCLICKA),
   MAPENTRY(HDN_ITEMCLICKW),
};

// gets info text for a WM_NOTIFY message
static CString GetNotifyInfo(const NMHDR& notifyHeader)
{
   CString className;
   GetClassName(notifyHeader.hwndFrom, className.GetBuffer(256), 256);
   className.ReleaseBuffer();

   CString text;
   auto notifyNameIter = s_notifyNames.find(notifyHeader.code);
   if (notifyNameIter != s_notifyNames.end())
   {
      text.Format(
         _T("NMHDR from=0x%08llx class=%s code=%s (0x%04x)"),
         (UINT_PTR)notifyHeader.hwndFrom,
         className.GetString(),
         notifyNameIter->second,
         notifyHeader.code);
   }
   else
   {
      text.Format(
         _T("NMHDR from=0x%08llx class=%s code=0x%04x"),
         (UINT_PTR)notifyHeader.hwndFrom,
         className.GetString(),
         notifyHeader.code);
   }

   return text;
}

void AtlTraceMessage(
   _In_z_ LPCTSTR className,
   _In_z_ const char* fileName,
   _In_ int lineNo,
   _In_ HWND hWnd,
   _In_ UINT uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam)
{
   if (!IsTracingEnabled(atlTraceWindowing, 0))
      return;

   CString text;

   text.Format(
      _T("%s (0x%08llx) "),
      className,
      (UINT_PTR)hWnd);

   text.AppendFormat(
      _T("msg=%s (0x%04lx) "),
      GetMessageName(uMsg).GetString(),
      uMsg);

   if (uMsg == WM_NOTIFY)
   {
      NMHDR& notifyHeader = *reinterpret_cast<LPNMHDR>(lParam);

      text += GetNotifyInfo(notifyHeader);
   }
   else
   {
      text.AppendFormat(
         _T("wParam=0x%04llx lParam=0x%08llx"),
         wParam,
         lParam);
   }

   ATL::CTraceFileAndLineInfo(
      fileName,
      lineNo)(
         atlTraceWindowing,
         0,
         _T("%s\n"),
         text.GetString());
}
#endif
