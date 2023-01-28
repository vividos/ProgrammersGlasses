//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file IContentView.hpp
/// \brief content view interface
//
#pragma once

/// \brief Content view interface
/// \details A content view shows the contents of a single node in a WTL or
/// other HWND based view.
class IContentView
{
public:
   /// dtor
   virtual ~IContentView() = default;

   /// creates the content view
   virtual HWND CreateView(HWND parent) = 0;

   /// processes window message
   virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) = 0;

   /// destroys content view
   virtual void DestroyView() = 0;
};
