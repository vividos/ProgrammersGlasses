//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file StructListViewNode.hpp
/// \brief node with a list view showing a structure
//
#pragma once

#include "modules/INode.hpp"

class StructDefinition;

/// \brief List view node showing a data structure
/// \details The node uses a list view showing data using a structure definition that defines how
/// to interpret the data.
class StructListViewNode : public INode
{
public:
   /// ctor
   StructListViewNode(const CString& displayName, NodeTreeIconID iconID,
      const StructDefinition& structDefinition, LPCVOID basePointer);

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
      return std::vector<std::shared_ptr<INode>>();
   }

   virtual std::shared_ptr<IContentView> GetContentView() override;

private:
   CString m_displayName;     ///< display name for node
   NodeTreeIconID m_iconID;   ///< tree icon ID

   /// structure definition to use
   const StructDefinition& m_structDefinition;

   /// base pointer where structure is located in memory
   LPCVOID m_basePointer;
};
