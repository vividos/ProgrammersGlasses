//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CodeTextView.cpp
/// \brief code text view
//
#include "stdafx.h"
#include "CodeTextView.hpp"

CodeTextView::CodeTextView(const CString& text)
   :m_text(text)
{
   m_text.Replace(_T("\n"), _T("\r\n"));
}

BOOL CodeTextView::PreTranslateMessage(MSG* msg)
{
   msg;
   return FALSE;
}

LRESULT CodeTextView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT ret = DefWindowProc(uMsg, wParam, lParam);
   bHandled = TRUE;

   Init();

   return ret;
}

void CodeTextView::Init()
{
   CLogFont logFont;
   logFont.SetHeight(12);
   _tcscpy_s(logFont.lfFaceName, _T("Consolas"));
   CFontHandle codeFont = logFont.CreateFontIndirect();

   SetFont(codeFont);

   // set to maximum buffer text
   SetLimitText(0);

   SetWindowText(m_text);
}
