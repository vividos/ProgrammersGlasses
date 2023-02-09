//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file NodeAndContentView.hpp
/// \brief node and content view
//
#pragma once

class IReader;
class INode;
class IContentView;

/// \brief view to show a node tree and a content view
class NodeAndContentView :
   public CTabbedMDIChildWindowImpl<NodeAndContentView>
{
   typedef CTabbedMDIChildWindowImpl<NodeAndContentView> baseClass;

public:
   /// ctor
   explicit NodeAndContentView(std::shared_ptr<IReader> reader)
      :m_reader(reader)
   {
   }

   /// called when the view is about to be destroyed
   DECLARE_FRAME_WND_CLASS(nullptr, IDR_MAINFRAME)

private:
   BEGIN_MSG_MAP(NodeAndContentView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnTreeViewSelChanged)
      if (ForwardToContentView(hWnd, uMsg, wParam, lParam, lResult)) return TRUE;
      CHAIN_MSG_MAP(baseClass)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when the view is created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the view is destroyed
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called after receiving the final message
   virtual void OnFinalMessage(HWND hWnd);

   /// called when the selection of the tree view has changed
   LRESULT OnTreeViewSelChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   /// forwards messages to content view, e.g. edit commands
   BOOL ForwardToContentView(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lResult);

   /// initializes tree with content from reader
   void InitTree();

   /// adds nodes recursively to the tree
   void AddNodesRecursive(const INode& node, HTREEITEM parentItem);

   /// changes content view to show node's content
   void ChangeContentView(INode& node);

private:
   // splitter for tree and content views
   CSplitterWindow m_splitter;

   /// pane container for the tree view
   CPaneContainer m_pane;

   /// node tree view
   CTreeViewCtrl m_nodeTreeView;

   /// module reader to use for reading the file to display
   std::shared_ptr<IReader> m_reader;

   /// currently displayed content view
   std::shared_ptr<IContentView> m_contentView;
};
