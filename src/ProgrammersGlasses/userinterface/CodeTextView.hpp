//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2021 Michael Fink
//
/// \file CodeTextView.hpp
/// \brief code text view
//
#pragma once

#include "modules/IContentView.hpp"

/// window traits for the code text view
typedef CWinTraitsOR<WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL> CodeTextViewWinTraits;

/// \brief code text view
/// \details a CEdit based view that shows a read-only text with a code font
/// (e.g. a monospace font)
class CodeTextView :
   public CWindowImpl<CodeTextView, CEdit, CodeTextViewWinTraits>,
   public CEditCommands<CodeTextView>,
   public IContentView
{
   typedef CWindowImpl<CodeTextView, CEdit, CodeTextViewWinTraits> baseClass;

public:
   /// ctor; takes text to display
   CodeTextView(const CString& text);

   DECLARE_WND_SUPERCLASS(nullptr, CEdit::GetWndClassName())

   /// called before messages are translated
   BOOL PreTranslateMessage(MSG* msg) const;

   // Inherited via IContentView
   virtual HWND CreateView(HWND parent) override
   {
      return Create(parent, rcDefault);
   }

   virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override
   {
      return CodeTextView::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0UL);
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   BEGIN_MSG_MAP(CodeTextView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      CHAIN_MSG_MAP_ALT(CEditCommands<CodeTextView>, 1)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when the window is about to be created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// initializes code text view
   void Init();

   /// text to display
   CString m_text;
};
