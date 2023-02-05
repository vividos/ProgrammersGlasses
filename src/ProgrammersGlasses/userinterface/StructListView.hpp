//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file StructListView.hpp
/// \brief list view showing data structure fields
//
#pragma once

#include "modules/IContentView.hpp"

class StructDefinition;
struct StructField;

/// window traits for the field list view
typedef CWinTraitsOR<LVS_REPORT, LVS_EX_FULLROWSELECT> StructListViewWinTraits;

/// \brief List view showing fields of a structure, e.g. file header
/// \details The content of structures often can be displayed by a list of fields with a formatted
/// content. This list view uses a struct definition to display this structure. This separates the
/// structure definition from the user interface.
class StructListView :
   public CWindowImpl<StructListView, CListViewCtrl, StructListViewWinTraits>,
   public IContentView
{
   /// base class
   typedef CWindowImpl<StructListView, CListViewCtrl, StructListViewWinTraits> baseClass;

public:
   /// ctor
   StructListView(const StructDefinition& structDefinition, LPCVOID structBaseAddress, LPCVOID fileBaseAddress)
      :m_structDefinition(structDefinition),
      m_structBaseAddress(structBaseAddress),
      m_fileBaseAddress(fileBaseAddress)
   {
   }

   DECLARE_WND_SUPERCLASS(nullptr, CListViewCtrl::GetWndClassName())

   // Inherited via IContentView
   virtual HWND CreateView(HWND parent) override
   {
      return Create(parent, rcDefault);
   }

   virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override
   {
      return StructListView::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0UL);
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   BEGIN_MSG_MAP(StructListView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when the window is about to be created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// initializes list with struct definition and content
   void InitList();

private:
   /// structure definition
   const StructDefinition& m_structDefinition;

   /// structure base address
   LPCVOID m_structBaseAddress;

   /// file base address
   LPCVOID m_fileBaseAddress;
};
