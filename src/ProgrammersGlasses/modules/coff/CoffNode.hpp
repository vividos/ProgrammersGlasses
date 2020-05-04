//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffNode.hpp
/// \brief single node in a tree structure of a COFF file
//
#pragma once

#include "modules/StaticNode.hpp"

class IContentView;

/// \brief node in a COFF file tree structure
class CoffNode : public StaticNode
{
public:
   /// ctor
   CoffNode(const CString& displayName, NodeTreeIconID iconID)
      :StaticNode(displayName, iconID)
   {
   }

   // Inherited via INode
   std::shared_ptr<IContentView> GetContentView();
};

