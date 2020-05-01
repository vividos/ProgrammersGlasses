//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file NodeAndContentView.cpp
/// \brief node and content view
//
#include "stdafx.h"
#include "resource.h"
#include "NodeAndContentView.hpp"

LRESULT NodeAndContentView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

   m_pane.SetPaneContainerExtendedStyle(PANECNT_NOBORDER | PANECNT_NOCLOSEBUTTON);
   m_pane.Create(m_splitter, _T("Nodes"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_nodeTreeView.Create(m_pane, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);

   m_pane.SetClient(m_nodeTreeView);

   m_splitter.SetSplitterPanes(m_pane, nullptr);
   m_splitter.SetSplitterPosPct(25);

   m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);

   return 0;
}

void NodeAndContentView::OnFinalMessage(HWND /*hWnd*/)
{
   delete this;
}
