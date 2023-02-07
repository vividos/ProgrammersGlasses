//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file AboutDlg.hpp
/// \brief about dialog
//
#pragma once

class ModuleManager;

/// \brief About dialog
/// \details The about dialog shows the version, license and the list of
/// modules. The dialog is resizable.
class AboutDlg :
   public CDialogImpl<AboutDlg>,
   public CWinDataExchange<AboutDlg>,
   public CDialogResize<AboutDlg>
{
public:
   /// ctor
   explicit AboutDlg(const ModuleManager& moduleManager)
      :m_moduleManager(moduleManager)
   {
   }

   /// dialog ID
   enum { IDD = IDD_ABOUTBOX };

   BEGIN_DDX_MAP(AboutDlg)
      DDX_CONTROL(IDC_STATIC_ABOUT_WEBPAGE, m_hyperlinkWebpage)
      DDX_CONTROL(IDC_STATIC_ABOUT_LICENSE, m_hyperlinkLicense)
      DDX_CONTROL_HANDLE(IDC_LIST_ABOUT_MODULES, m_listModules)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(AboutDlg)
      DLGRESIZE_CONTROL(IDC_LIST_ABOUT_MODULES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_MSG_MAP(AboutDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
      CHAIN_MSG_MAP(CDialogResize<AboutDlg>)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when dialog is initialized
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the dialog is closed
   LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      EndDialog(wID);
      return 0;
   }

private:
   /// initializes modules list
   void InitModulesList();

   /// module manager
   const ModuleManager& m_moduleManager;

   /// web page hyperlink
   CHyperLink m_hyperlinkWebpage;

   /// license hyperlink
   CHyperLink m_hyperlinkLicense;

   /// list with all available modules
   CListViewCtrl m_listModules;
};
