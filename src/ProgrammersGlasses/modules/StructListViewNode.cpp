//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file StructListViewNode.cpp
/// \brief node with a list view showing a structure
//
#include "stdafx.h"
#include "StructListViewNode.hpp"
#include "userinterface/StructListView.hpp"

StructListViewNode::StructListViewNode(const CString& displayName, NodeTreeIconID iconID,
   const StructDefinition& structDefinition, LPCVOID structBasePointer,
   LPCVOID fileBasePointer)
   :StaticNode(displayName, iconID),
   m_structDefinition(structDefinition),
   m_structBasePointer(structBasePointer),
   m_fileBasePointer(fileBasePointer)
{
}

std::shared_ptr<IContentView> StructListViewNode::GetContentView()
{
   return std::make_shared<StructListView>(m_structDefinition, m_structBasePointer, m_fileBasePointer);
}
