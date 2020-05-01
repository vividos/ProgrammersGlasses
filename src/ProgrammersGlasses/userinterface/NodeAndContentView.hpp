//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file NodeAndContentView.hpp
/// \brief node and content view
//
#pragma once

/// \brief view to show a node tree and a content view
class NodeAndContentView :
   public CMDIChildWindowImpl<NodeAndContentView>
{
   typedef CMDIChildWindowImpl<NodeAndContentView> baseClass;

public:
   /// ctor
   NodeAndContentView()
   {
   }

   /// called when the view is about to be destroyed
   virtual void OnFinalMessage(HWND hWnd);

   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

private:
   BEGIN_MSG_MAP(NodeAndContentView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when the view is created
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   // splitter for tree and content views
   CSplitterWindow m_splitter;

   /// pane container for the tree view
   CPaneContainer m_pane;

   /// node tree view
   CTreeViewCtrl m_nodeTreeView;
};
