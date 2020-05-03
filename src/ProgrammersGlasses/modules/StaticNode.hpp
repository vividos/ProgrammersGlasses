//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file StaticNode.hpp
/// \brief node with static properties
//
#pragma once

#include "modules/INode.hpp"

/// \brief Static node
/// \details A static node can be used when only the content view method from
/// INode has to be overridden and the other values are static. These can be
/// passed in the ctor.
class StaticNode : public INode
{
public:
   /// ctor
   StaticNode(const CString& displayName, NodeTreeIconID iconID)
      :m_displayName(displayName),
      m_iconID(iconID)
   {
   }

   /// returns child nodes list; non-const version
   std::vector<std::shared_ptr<INode>>& ChildNodes()
   {
      return m_childNodes;
   }

   // Inherited via INode
   virtual const CString& DisplayName() const override
   {
      return m_displayName;
   }

   virtual NodeTreeIconID IconID() const override
   {
      return m_iconID;
   }

   virtual const std::vector<std::shared_ptr<INode>>& ChildNodes() const override
   {
      return m_childNodes;
   }

private:
   CString m_displayName;     ///< display name for node
   NodeTreeIconID m_iconID;   ///< tree icon ID
   std::vector<std::shared_ptr<INode>> m_childNodes;  ///< child nodes for this node
};
