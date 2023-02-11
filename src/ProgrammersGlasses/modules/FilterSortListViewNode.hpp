//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file FilterSortListViewNode.hpp
/// \brief node with a list view that can be filtered and sorted
//
#pragma once

#include "modules/StaticNode.hpp"

class StructDefinition;

/// \brief List view node showing a filterable and sortable list view
/// \details The node uses a list view showing tabular data. The data can be
/// sorted and filtered in order to find relevant entries.
class FilterSortListViewNode : public StaticNode
{
public:
   /// ctor
   FilterSortListViewNode(const CString& displayName, NodeTreeIconID iconID,
      const std::vector<CString>& columnsList,
      const std::vector<std::vector<CString>>& data,
      bool allowFiltering);

   // Inherited via INode
   virtual std::shared_ptr<IContentView> GetContentView() override;

private:
   /// list of column names
   std::vector<CString> m_columnsList;

   /// all data to display, first by line, then by column
   std::vector<std::vector<CString>> m_data;

   /// indicates if the list view allows filtering entries
   bool m_allowFiltering;
};
