//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CodeTextViewNode.hpp
/// \brief node showing a code text view
//
#pragma once

#include "StaticNode.hpp"

class IContentView;

/// \brief node showing a code text view
class CodeTextViewNode : public StaticNode
{
public:
   /// ctor
   CodeTextViewNode(const CString& displayName, NodeTreeIconID iconID)
      :StaticNode(displayName, iconID)
   {
   }

   /// sets node text to display
   void SetText(const CString& text)
   {
      m_text = text;
   }

   // Inherited via INode
   std::shared_ptr<IContentView> GetContentView();

private:
   /// node text to display
   CString m_text;
};
