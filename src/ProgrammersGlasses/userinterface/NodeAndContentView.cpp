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
#include "modules/IReader.hpp"
#include "modules/INode.hpp"
#include "modules/IContentView.hpp"

/// first node bitmap ID
const UINT c_firstNodeBitmap = IDB_NODE_DOCUMENT;

/// maximum number of node bitmaps
const UINT c_maxNodeBitmapNumber = 512;

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

   InitTree();

   m_nodeTreeView.Expand(TVI_ROOT, TVE_EXPAND);

   m_nodeTreeView.Select(TVI_ROOT, TVGN_FIRSTVISIBLE | TVGN_CARET);

   return 0;
}

LRESULT NodeAndContentView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (m_contentView != nullptr)
   {
      m_contentView->DestroyView();
      m_contentView.reset();
   }

   return 0;
}

void NodeAndContentView::OnFinalMessage(HWND /*hWnd*/)
{
   if (m_reader != nullptr)
   {
      m_reader->Cleanup();
      m_reader.reset();
   }

   delete this;
}

LRESULT NodeAndContentView::OnTreeViewSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)pnmh;

   HTREEITEM selectedItem = pnmtv->itemNew.hItem;

   INode* nodePtr = (INode *)m_nodeTreeView.GetItemData(selectedItem);

   ChangeContentView(*nodePtr);

   return 0;
}

void NodeAndContentView::InitTree()
{
   CImageList imageList;
   imageList.Create(16, 16, ILC_COLOR24, 0, c_maxNodeBitmapNumber);

   for (UINT bitmapId = c_firstNodeBitmap; bitmapId < c_firstNodeBitmap + c_maxNodeBitmapNumber; bitmapId++)
   {
      HBITMAP bitmap = AtlLoadBitmapImage(bitmapId);
      if (bitmap == nullptr)
         break;
      imageList.Add(bitmap);
   }

   m_nodeTreeView.SetImageList(imageList, TVSIL_NORMAL);

   ATLASSERT(m_reader != nullptr); // there must be a reader

   m_reader->Load();

   std::shared_ptr<INode> rootNode = m_reader->RootNode();
   ATLASSERT(rootNode != nullptr);

   AddNodesRecursive(*rootNode, TVI_ROOT);
}

void NodeAndContentView::AddNodesRecursive(const INode& node, HTREEITEM parentItem)
{
   HTREEITEM item = m_nodeTreeView.InsertItem(node.DisplayName(), parentItem, TVI_LAST);

   int imageIndex = (UINT)node.IconID() - c_firstNodeBitmap;
   m_nodeTreeView.SetItemImage(item, imageIndex, imageIndex);
   m_nodeTreeView.SetItemData(item, (DWORD_PTR)&node);

   for (auto subnode : node.ChildNodes())
      AddNodesRecursive(*subnode, item);
}

void NodeAndContentView::ChangeContentView(INode& node)
{
   if (m_contentView != nullptr)
   {
      m_contentView->DestroyView();
      m_contentView.reset();
   }

   m_contentView = node.GetContentView();

   if (m_contentView)
   {
      HWND view = m_contentView->CreateView(m_splitter.m_hWnd);

      m_splitter.SetSplitterPanes(m_pane, view);

      m_splitter.SetSinglePaneMode(SPLIT_PANE_NONE);
   }
}
