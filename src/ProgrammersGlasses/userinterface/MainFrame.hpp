//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file MainFrame.hpp
/// \brief main frame window
//
#pragma once

#include "ModuleManager.hpp"

/// window message to open files at startup
#define WM_OPEN_FILES WM_USER + 1

/// main frame base class that supports ribbons and tabbing and MDI frames
template <typename T,
   typename TBase = CTabbedMDIClient<CDotNetTabCtrl<CTabViewTabItem>>,
   typename TWinTraits = ATL::CFrameWinTraits>
   class ATL_NO_VTABLE CTabbedRibbonMDIFrameWindowImpl :
   public CRibbonFrameWindowImplBase<T, CTabbedMDIFrameWindowImpl<T, TBase, CMDIWindow, TWinTraits>>
{
};

/// \brief main frame window
/// \details the main frame window displays a tab view that hosts the different child windows to
/// display the file contents
class MainFrame :
   public CTabbedRibbonMDIFrameWindowImpl<MainFrame>,
   public CMessageFilter,
   public CIdleHandler
{
   typedef CTabbedRibbonMDIFrameWindowImpl<MainFrame> baseClass;

public:
   /// ctor
   MainFrame(const std::vector<CString>& filenamesList)
      :m_filenamesList(filenamesList)
   {
   }

   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

   BEGIN_RIBBON_CONTROL_MAP(MainFrame)
   END_RIBBON_CONTROL_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_OPEN_FILES, OnOpenFiles)
      MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
      COMMAND_ID_HANDLER(ID_FILE_CLOSE, OnFileClose)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTileHorizontal)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_VERT, OnWindowTileVertical)
      CHAIN_MDI_CHILD_COMMANDS()
      CHAIN_MSG_MAP(baseClass)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // update map for menus and toolbars
   BEGIN_UPDATE_UI_MAP(MainFrame)
      UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_RIBBON)
   END_UPDATE_UI_MAP()

private:
   /// called before messages are translated
   virtual BOOL PreTranslateMessage(MSG* msg);

   /// called when the app is idle
   virtual BOOL OnIdle();

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when the window is about to be created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the window is destroyed
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called at startup when files should be opened
   LRESULT OnOpenFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when one or more files are dropped on the main window
   LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the about command is clicked
   LRESULT OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when the exit command is clicked
   LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called to open a new file
   LRESULT OnFileOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called to close the currently open file
   LRESULT OnFileClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when the command to cascade MDI windows is sent
   LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDICascade();
      return 0;
   }

   /// called when the command to tile the MDI windows horizontally is sent
   LRESULT OnWindowTileHorizontal(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDITile(MDITILE_HORIZONTAL);
      return 0;
   }

   /// called when the command to tile the MDI windows vertically is sent
   LRESULT OnWindowTileVertical(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDITile(MDITILE_VERTICAL);
      return 0;
   }

   /// opens new file
   void OpenFile(const CString& filename);

   /// adds a "all supported files" filter in front of the filter string
   static CString AddSupportedFilesFilter(const CString& filter);

public:
   /// MDI command bar for tabbing
   CTabbedMDICommandBarCtrl m_commandBar;

   /// tabbed MDI client window
   CTabbedMDIClient<CDotNetTabCtrl<CTabViewTabItem>> m_tabbedClient;

   /// list of filenames to open at startup
   std::vector<CString> m_filenamesList;

   /// manager for all modules
   ModuleManager m_moduleManager;
};
