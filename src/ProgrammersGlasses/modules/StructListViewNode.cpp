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
   const StructDefinition& structDefinition, LPCVOID basePointer)
   :StaticNode(displayName, iconID),
   m_structDefinition(structDefinition),
   m_basePointer(basePointer)
{
}

std::shared_ptr<IContentView> StructListViewNode::GetContentView()
{
   return std::make_shared<StructListView>(m_structDefinition, m_basePointer);
}
