//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file IReader.hpp
/// \brief reader interface
//
#pragma once

#include "File.hpp"

class INode;

/// \brief Reader interface
/// \details A reader that can read a specific format and returns detail infos.
/// The reader builds a tree of nodes that can display more specific infos, such
/// as a summary, contained objects or the like.
class IReader
{
public:
   /// dtor
   virtual ~IReader() noexcept {}

   /// Returns the filename of the file to be read
   virtual const CString& Filename() const = 0;

   /// Returns the root node of the file tree; only available after calling Load()
   virtual std::shared_ptr<INode> RootNode() const = 0;

   /// Loads all nodes and necessary infos
   virtual void Load() = 0;

   /// Performs time consuming cleanup, if any
   virtual void Cleanup() = 0;
};
