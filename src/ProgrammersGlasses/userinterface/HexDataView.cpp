//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2026 Michael Fink
//
/// \file HexDataView.cpp
/// \brief view to display data as hexadecimal bytes
//

// since some of the code originates from Bjarke Viksoe's Hex editor control,
// I'm keeping the copyright message here

/////////////////////////////////////////////////////////////////////////////
// Hex Editor - A hex editor control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2004 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
#include "stdafx.h"
#include "HexDataView.hpp"
#include "File.hpp"

#define ROUND(x) (x / m_dataSize * m_dataSize)

HexDataView::HexDataView(
   const File& file,
   size_t blockStartOffset,
   size_t blockSize)
   :m_file(file),
   m_blockStartOffset(blockStartOffset),
   m_blockSize(blockSize),
   m_firstLineOffset(m_blockStartOffset)
{
}

/// this is the font we prefer
static LPCTSTR c_fontName = _T("Consolas");

LRESULT HexDataView::OnCreate(
   UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CLogFont logFont;
   logFont.SetHeight(12);
   _tcscpy_s(logFont.lfFaceName, c_fontName);
   logFont.lfPitchAndFamily = FIXED_PITCH; // make sure Windows finds a fixed-width font
   m_fontEditor = logFont.CreateFontIndirect();

   CClientDC dc = m_hWnd;
   CFontHandle oldFont = dc.SelectFont(m_fontEditor);
   dc.GetTextMetrics(&m_tmEditor);
   dc.SelectFont(oldFont);

   ModifyStyle(0, WS_VSCROLL);
   SetScrollPos(SB_VERT, 0, TRUE);

   return 0;
}

LRESULT HexDataView::OnSize(
   UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   SetScrollRange(
      SB_VERT,
      0,
      (int)(m_blockSize / m_bytesPerLine) - GetLinesPerPage() + 1,
      TRUE);

   return 0;
}

LRESULT HexDataView::OnVScroll(
   UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS | SIF_RANGE | SIF_TRACKPOS };
   GetScrollInfo(SB_VERT, &si);

   // SetScrollPos takes an integer, so let's hope the scroll pos doesn't exceed 2^31
   int scrollPos =
      static_cast<int>((m_firstLineOffset - m_blockStartOffset) / m_bytesPerLine);

   switch (LOWORD(wParam))
   {
   case SB_TOP:
      scrollPos = 0;
      break;

   case SB_BOTTOM:
      scrollPos = si.nMax;
      break;

   case SB_LINEUP:
      if (scrollPos > 0)
         scrollPos -= 1;
      break;

   case SB_LINEDOWN:
      scrollPos += 1;
      break;

   case SB_PAGEUP:
      if (scrollPos > GetLinesPerPage())
         scrollPos -= GetLinesPerPage();
      else
         scrollPos = 0;
      break;

   case SB_PAGEDOWN:
      scrollPos += GetLinesPerPage();
      break;

   case SB_THUMBTRACK:
   case SB_THUMBPOSITION:
      scrollPos = si.nTrackPos;
      break;
   }

   if (scrollPos < si.nMin)
      scrollPos = si.nMin;

   if (scrollPos > si.nMax)
      scrollPos = si.nMax;

   if (scrollPos == si.nPos)
      return 0;

   SetScrollPos(SB_VERT, scrollPos, TRUE);

   m_firstLineOffset =
      m_blockStartOffset +
      static_cast<DWORD_PTR>(scrollPos) * m_bytesPerLine;

   Invalidate();

   return 0;
}

LRESULT HexDataView::OnLButtonDblClk(
   UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

   bool inDataPane = false;
   DWORD_PTR filePosition = 0;

   if (!GetPosFromPoint(pt, filePosition, inDataPane))
      return 0;

   m_selectionInDataPane = inDataPane;

   SetSel(filePosition, filePosition + m_dataSize);

   return 0;
}

LRESULT HexDataView::OnMouseWheel(
   UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   int zDelta =
      (uMsg == WM_MOUSEWHEEL)
      ? (int)(short)HIWORD(wParam)
      : (int)wParam;

   for (int scrollCount = 0; scrollCount < abs(zDelta / WHEEL_DELTA * 2); scrollCount++)
      SendMessage(WM_VSCROLL, zDelta > 0 ? SB_LINEUP : SB_LINEDOWN);

   return 0;
}

LRESULT HexDataView::OnLButtonDown(
   UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   SetFocus();

   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

   bool inDataPane = false;
   DWORD_PTR filePosition = 0;

   if (!GetPosFromPoint(pt, filePosition, inDataPane))
      return 0;

   m_selectionInDataPane = inDataPane;

   SetSel(filePosition, filePosition);

   // If user is dragging the mouse, we'll initiate a selection...
   ClientToScreen(&pt);

   if (DragDetect(m_hWnd, pt))
      SetCapture();

   return 0;
}

LRESULT HexDataView::OnLButtonUp(
   UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   if (GetCapture() != m_hWnd)
      return 0;

   SendMessage(WM_MOUSEMOVE, wParam, lParam);

   ReleaseCapture();

   return 0;
}

LRESULT HexDataView::OnMouseMove(
   UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   if (GetCapture() != m_hWnd)
      return 0;

   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

   // Mouse outside client rectangle? Let's scroll the view
   CRect rcClient;
   GetClientRect(&rcClient);

   if (pt.y < 0 && m_firstLineOffset > m_blockStartOffset)
      SendMessage(WM_VSCROLL, SB_LINEUP);

   if (pt.y > rcClient.bottom - rcClient.top)
      SendMessage(WM_VSCROLL, SB_LINEDOWN);

   // Expand the selection if mouse is over a valid position?
   bool inDataPane = false;
   DWORD_PTR filePosition = 0;

   if (!GetPosFromPoint(pt, filePosition, inDataPane))
      return 0;

   if (m_selectionInDataPane != inDataPane)
      return 0;

   SetSel(m_selStart, filePosition == 0 ? 0 : filePosition + m_dataSize);

   return 0;
}

BOOL HexDataView::SetSel(DWORD_PTR startSel, DWORD_PTR endSel, BOOL noScroll)
{
   ATLASSERT(::IsWindow(m_hWnd));

   DWORD_PTR blockEndOffset = m_blockStartOffset + m_blockSize;

   if (endSel == (DWORD_PTR)-1)
      endSel = blockEndOffset;

   if (startSel == endSel && endSel >= blockEndOffset)
      endSel = blockEndOffset - 1;

   if (startSel != endSel && endSel > blockEndOffset)
      endSel = blockEndOffset;

   if (startSel >= blockEndOffset)
      startSel = blockEndOffset - 1;

   startSel = ROUND(startSel);
   endSel = ROUND(endSel);

   m_selStart = startSel;
   m_selEnd = endSel;

   if (!noScroll)
      RecalcPosition(endSel);

   return Invalidate();
}

void HexDataView::GetSel(DWORD_PTR& startSel, DWORD_PTR& endSel) const
{
   startSel = m_selStart;
   endSel = m_selEnd;

   // return values in normalized form
   if (startSel > endSel)
      std::swap(startSel, endSel);
}

int HexDataView::GetLineHeight() const
{
   ATLASSERT(::IsWindow(m_hWnd));

   return m_tmEditor.tmHeight;
}

int HexDataView::GetLinesPerPage() const
{
   ATLASSERT(::IsWindow(m_hWnd));

   CRect rcClient;
   GetClientRect(&rcClient);
   return (rcClient.bottom - rcClient.top) / GetLineHeight();
}

BOOL HexDataView::GetPosFromPoint(
   POINT pt, DWORD_PTR& filePosition, bool& inDataPane) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   ATLASSERT(m_rcData.left > 0);   // Oops! Not initialized! Call UpdateWindow() or delay the call!!!

   // Get rectangles for columns. Expand them a bit so it's easier
   // to hit with the mouse...
   CRect rcData = m_rcData;
   CRect rcAscii = m_rcAscii;
   rcData.InflateRect(4, 0);
   rcAscii.InflateRect(4, 0);

   if (rcData.PtInRect(pt))
   {
      rcData.OffsetRect(-2, -2);

      LONG xpos =
         (pt.x - rcData.left) / (((m_dataSize * 2) + 1) * m_tmEditor.tmAveCharWidth) * m_dataSize;

      if (xpos < 0)
         xpos = 0;

      if ((DWORD)xpos > m_bytesPerLine - 1)
         xpos = m_bytesPerLine - 1;

      LONG ypos = (pt.y - rcData.top) / GetLineHeight();
      filePosition =
         m_firstLineOffset +
         (DWORD_PTR)xpos + ((DWORD_PTR)ypos * m_bytesPerLine);

      inDataPane = true;

      return TRUE;
   }

   if (rcAscii.PtInRect(pt))
   {
      rcAscii.OffsetRect(4, 0);

      LONG xpos = (pt.x - rcAscii.left) / m_tmEditor.tmAveCharWidth;

      if (xpos < 0)
         xpos = 0;

      if ((DWORD)xpos > m_bytesPerLine - 1)
         xpos = m_bytesPerLine - 1;

      LONG ypos = (pt.y - rcAscii.top) / GetLineHeight();
      filePosition =
         m_firstLineOffset +
         (DWORD_PTR)xpos + ((DWORD_PTR)ypos * m_bytesPerLine);

      inDataPane = false;

      return TRUE;
   }

   return FALSE;
}

void HexDataView::RecalcPosition(DWORD_PTR filePosition)
{
   // Is new selection-position out of bounds?
   // If so, we need to set a new view position.
   DWORD_PTR page = (GetLinesPerPage() - 1) * m_bytesPerLine;
   if (filePosition < m_firstLineOffset)
   {
      m_firstLineOffset =
         filePosition - (filePosition % m_bytesPerLine);

      int scrollPos =
         static_cast<int>((m_firstLineOffset - m_blockStartOffset) / m_bytesPerLine);

      SetScrollPos(SB_VERT, scrollPos);
      Invalidate();
   }
   else if (filePosition > m_firstLineOffset + page)
   {
      m_firstLineOffset =
         filePosition - (filePosition % m_bytesPerLine);

      if (m_firstLineOffset >= page)
         m_firstLineOffset -= page;
      else
         m_firstLineOffset = m_blockStartOffset;

      int scrollPos =
         static_cast<int>((m_firstLineOffset - m_blockStartOffset)/ m_bytesPerLine);

      SetScrollPos(SB_VERT, scrollPos);
      Invalidate();
   }
}

void HexDataView::DoPaint(CDCHandle dc)
{
   CRect rcClient;
   GetClientRect(&rcClient);

   dc.FillSolidRect(&rcClient, ::GetSysColor(COLOR_WINDOW));

   rcClient.left += m_margin.cx;
   rcClient.top += m_margin.cy;

   CFontHandle oldFont = dc.SelectFont(m_fontEditor);

   int numLines = GetLinesPerPage() + 1;
   int lineHeight = GetLineHeight();

   DWORD_PTR currentOffset = m_firstLineOffset;
   DWORD_PTR blockEndOffset = m_blockStartOffset + m_blockSize;

   ::ZeroMemory(&m_rcData, sizeof(m_rcData));
   ::ZeroMemory(&m_rcAscii, sizeof(m_rcAscii));
   m_rcData.top = m_rcAscii.top = m_margin.cy;

   COLORREF clrTextH, clrBackH;
   COLORREF clrTextN = ::GetSysColor(COLOR_GRAYTEXT);
   COLORREF clrBackN = ::GetSysColor(COLOR_WINDOW);
   bool isHighlighted = false;

   dc.SetBkMode(OPAQUE);

   DWORD_PTR selStart = 0;
   DWORD_PTR selEnd = 0;
   GetSel(selStart, selEnd);

   int ypos = rcClient.top;
   TCHAR buffer[64] = { 0 };

   for (int lineIndex = 0; lineIndex < numLines; lineIndex++)
   {
      LONG xpos = rcClient.left;

      // Draw address text
      if (currentOffset < blockEndOffset)
      {
         dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
         dc.SetBkColor(clrBackN);

         ::wsprintf(buffer, _T("%08I64X  "), currentOffset);

         RECT rcAddress = { xpos, ypos, xpos + 200, ypos + lineHeight };

         dc.DrawText(
            buffer,
            -1,
            &rcAddress,
            DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOCLIP);

         xpos += static_cast<LONG>(m_tmEditor.tmAveCharWidth * _tcslen(buffer));
      }

      // Draw hex values
      {
         if (m_rcData.left == 0)
            m_rcData.left = xpos;

         clrBackH = ::GetSysColor(
            m_selectionInDataPane
            ? COLOR_HIGHLIGHT
            : COLOR_BTNFACE);

         clrTextH = ::GetSysColor(
            m_selectionInDataPane
            ? COLOR_HIGHLIGHTTEXT
            : COLOR_WINDOWTEXT);

         dc.SetTextColor(clrTextN);
         dc.SetBkColor(clrBackN);
         isHighlighted = false;

         for (DWORD_PTR lineByteOffset = 0; lineByteOffset < m_bytesPerLine; lineByteOffset += m_dataSize)
         {
            if (currentOffset + lineByteOffset >= blockEndOffset)
               break;

            if (currentOffset + lineByteOffset >= selStart &&
               currentOffset + lineByteOffset < selEnd)
            {
               if (!isHighlighted)
               {
                  dc.SetTextColor(clrTextH);
                  dc.SetBkColor(clrBackH);
                  isHighlighted = true;
               }
            }
            else
            {
               if (isHighlighted)
               {
                  dc.SetTextColor(clrTextN);
                  dc.SetBkColor(clrBackN);
                  isHighlighted = false;
               }
            }

            LPTSTR p = buffer + (lineByteOffset == 0 ? 0 : 1);
            buffer[0] = ' ';
            LPCBYTE byteOffset = m_file.Data<BYTE>(currentOffset + lineByteOffset);
            switch (m_dataSize) {
            case 1:
               ::wsprintf(p, _T("%02X"), (long)*(byteOffset));
               break;
            case 2:
               ::wsprintf(p, _T("%04X"), (long)*(LPWORD)(byteOffset));
               break;
            case 4:
               ::wsprintf(p, _T("%08X"), (long)*(LPDWORD)(byteOffset));
               break;
            default:
               ATLASSERT(false);
            }

            CRect rcData = { xpos, ypos, xpos + 200, ypos + lineHeight };

            dc.DrawText(
               buffer,
               -1,
               &rcData,
               DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOCLIP);

            xpos += static_cast<LONG>(m_tmEditor.tmAveCharWidth * _tcslen(buffer));
         }

         if (m_rcData.right == 0)
            m_rcData.right = xpos;
      }

      // Draw ASCII representation
      {
         xpos += m_tmEditor.tmAveCharWidth * 3;

         if (m_rcAscii.left == 0)
            m_rcAscii.left = xpos;

         xpos = m_rcAscii.left;

         clrBackH = ::GetSysColor(
            m_selectionInDataPane
            ? COLOR_BTNFACE
            : COLOR_HIGHLIGHT);

         clrTextH = ::GetSysColor(
            m_selectionInDataPane
            ? COLOR_WINDOWTEXT
            : COLOR_HIGHLIGHTTEXT);

         dc.SetTextColor(clrTextN);
         dc.SetBkColor(clrBackN);
         isHighlighted = false;

         DWORD_PTR lineByteOffset = 0;
         for (; lineByteOffset < m_bytesPerLine; lineByteOffset++)
         {
            if (currentOffset + lineByteOffset >= blockEndOffset)
               break;

            if (currentOffset + lineByteOffset >= selStart &&
               currentOffset + lineByteOffset < selEnd)
            {
               if (!isHighlighted)
               {
                  dc.SetTextColor(clrTextH);
                  dc.SetBkColor(clrBackH);
                  isHighlighted = true;
               }
            }
            else
            {
               if (isHighlighted)
               {
                  dc.SetTextColor(clrTextN);
                  dc.SetBkColor(clrBackN);
                  isHighlighted = false;
               }
            }

            TCHAR ch = *m_file.Data<BYTE>(currentOffset + lineByteOffset);
            ch = isprint(ch) ? ch : m_invalidAsciiChar;

            CRect rcAscii = { xpos, ypos, xpos + 100, ypos + lineHeight };

            dc.DrawText(
               &ch,
               1,
               &rcAscii,
               DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOCLIP);

            xpos += m_tmEditor.tmAveCharWidth;
         }

         if (m_rcAscii.right == 0)
            m_rcAscii.right = xpos;
      }

      currentOffset += m_bytesPerLine;
      ypos += lineHeight;
   }

   m_rcData.bottom = m_rcAscii.bottom = ypos;

   dc.SelectFont(oldFont);
}
