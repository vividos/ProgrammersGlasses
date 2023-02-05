//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file FilterSortListViewNode.cpp
/// \brief node with a list view that can be filtered and sorted
//
#include "stdafx.h"
#include "FilterSortListViewNode.hpp"
#include "userinterface/FilterSortListView.hpp"

FilterSortListViewNode::FilterSortListViewNode(const CString& displayName, NodeTreeIconID iconID,
   std::vector<CString> columnsList,
   std::vector<std::vector<CString>> data,
   bool allowFiltering)
   :StaticNode(displayName, iconID),
   m_columnsList(columnsList),
   m_data(data),
   m_allowFiltering(allowFiltering)
{
}

std::shared_ptr<IContentView> FilterSortListViewNode::GetContentView()
{
   return std::make_shared<FilterSortListView>(m_columnsList, m_data);
}
