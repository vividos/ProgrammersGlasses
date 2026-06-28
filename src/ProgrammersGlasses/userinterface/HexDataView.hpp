//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2026 Michael Fink
//
/// \file HexDataView.hpp
/// \brief view to display data as hexadecimal bytes
//
#pragma once

#include <atlgdix.h>
#include "modules/IContentView.hpp"

class File;

/// window traits for the hex data view
typedef CWinTraitsOR<WS_VSCROLL> HexDataViewWinTraits;

/// \brief hex data view
/// inspired by CHexEditorCtrl from Bjarke Viksoe,
/// see https://www.viksoe.dk/code/hexeditor.htm
/// The control displays a block or all data from a File instance. First, the
/// address offset in the file is displayed, then the data bytes, and then the
/// same bytes as ASCII, when it's a printable data byte.
/// Data bytes can be selected, and the selection is also marked in the ASCII
/// area and vice versa.
class HexDataView :
   public CWindowImpl<HexDataView, CWindow, HexDataViewWinTraits>,
   public COffscreenDraw<HexDataView>,
   public IContentView
{
public:
   /// ctor; takes file and block start offset and size
   HexDataView(
      const File& file,
      size_t blockStartOffset,
      size_t blockSize);

   DECLARE_WND_CLASS_EX(
      _T("HexDataView"),
      CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
      COLOR_WINDOW + 1)

   /// called before messages are translated
   BOOL PreTranslateMessage(MSG* /*pMsg*/)
   {
      return FALSE;
   }

   /// sets a new selection
   BOOL SetSel(DWORD_PTR startSel, DWORD_PTR endSel, BOOL noScroll = FALSE);

   /// gets the current selection
   void GetSel(DWORD_PTR& startSel, DWORD_PTR& endSel) const;

   // Inherited via IContentView
   HWND CreateView(HWND parent, CRect& rect) override
   {
      return Create(parent, rect);
   }

   BOOL ProcessWindowMessage(
      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
      LRESULT& lResult) override
   {
      return HexDataView::ProcessWindowMessage(
         hWnd, uMsg, wParam, lParam, lResult, 0UL);
   }

   void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   /// file to display
   const File& m_file;

   /// start offset in file of block to display
   DWORD_PTR m_blockStartOffset;

   /// size of the data block to display
   DWORD_PTR m_blockSize;

   /// absolute file position of first line to display
   DWORD_PTR m_firstLineOffset;

   /// size of one data element (1, 2, 4)
   DWORD m_dataSize = 1;

   /// bytes per line to display
   DWORD m_bytesPerLine = 16;

   /// ASCII char to display for non-printable bytes
   TCHAR m_invalidAsciiChar = _T('.');

   /// current rectangle of the data area
   CRect m_rcData;

   /// current rectangle of the ASCII text area
   CRect m_rcAscii;

   DWORD_PTR m_selStart;       ///< Selection start
   DWORD_PTR m_selEnd;         ///< Selection end

   CFont m_fontEditor;     ///< editor font
   TEXTMETRIC m_tmEditor;  ///< text metric for the editor font

   /// indicates if selection is in data area (vs in ASCII area)
   bool m_selectionInDataPane;

   /// margin size, from top left
   CSize m_margin = CSize(3, 2);

   // Message map and handlers
   BEGIN_MSG_MAP(HexDataView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
      MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
      MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
      MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      CHAIN_MSG_MAP(COffscreenDraw<HexDataView>)
   END_MSG_MAP()

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   friend COffscreenDraw<HexDataView>;

   /// get height of one line for the selected font
   int GetLineHeight() const;

   /// returns the number of lines that fit on a page
   int GetLinesPerPage() const;

   /// recalculates display position for given new absolute file position
   void RecalcPosition(DWORD_PTR filePosition);

   /// gets the file position from given point in the view
   BOOL GetPosFromPoint(POINT pt, DWORD_PTR& filePosition, bool& inDataPane) const;

   // paints the control
   void DoPaint(CDCHandle dc);
};
