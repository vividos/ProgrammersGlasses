//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file INode.hpp
/// \brief single node in a tree structure of a reader
//
#pragma once

#include <vector>
#include <memory>

class IContentView;

/// Icon IDs from resources for node tree items
enum class NodeTreeIconID : UINT
{
   nodeTreeIconDocument = 1024,  ///< general document icon
   nodeTreeIconLibrary = 1025,   ///< library icon
   nodeTreeIconBinary = 1026,    ///< binary data icon
   nodeTreeIconItem = 1027,      ///< item (list item) icon
   nodeTreeIconObject = 1028,    ///< object (file) icon
   nodeTreeIconTable = 1029,     ///< table icon
};

/// \brief Node interface
/// \details specifies one node in the file's tree node structure
class INode
{
public:
   /// dtor
   virtual ~INode() = default;

   /// returns a display name for the node
   virtual const CString& DisplayName() const = 0;

   /// returns the node tree icon ID
   virtual NodeTreeIconID IconID() const = 0;

   /// Returns the list of child nodes of the file tree
   virtual const std::vector<std::shared_ptr<INode>>& ChildNodes() const = 0;

   /// returns a content view to display the node's data
   virtual std::shared_ptr<IContentView> GetContentView() = 0;
};
