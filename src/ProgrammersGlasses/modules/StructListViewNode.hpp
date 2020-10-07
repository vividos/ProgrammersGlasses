//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file StructListViewNode.hpp
/// \brief node with a list view showing a structure
//
#pragma once

#include "modules/StaticNode.hpp"

class StructDefinition;

/// \brief List view node showing a data structure
/// \details The node uses a list view showing data using a structure definition that defines how
/// to interpret the data.
class StructListViewNode : public StaticNode
{
public:
   /// ctor
   StructListViewNode(const CString& displayName, NodeTreeIconID iconID,
      const StructDefinition& structDefinition, LPCVOID structBasePointer,
      LPCVOID fileBasePointer);

   // Inherited via INode
   virtual const std::vector<std::shared_ptr<INode>>& ChildNodes() const override
   {
      static std::vector<std::shared_ptr<INode>> emptyNodeList;
      return emptyNodeList;
   }

   virtual std::shared_ptr<IContentView> GetContentView() override;

private:
   /// structure definition to use
   const StructDefinition& m_structDefinition;

   /// base pointer where structure is located in memory
   LPCVOID m_structBasePointer;

   /// file's base pointer in memory
   LPCVOID m_fileBasePointer;
};
