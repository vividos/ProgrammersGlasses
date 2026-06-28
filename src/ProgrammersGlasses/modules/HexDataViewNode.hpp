//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2026 Michael Fink
//
/// \file HexDataViewNode.hpp
/// \brief node showing hex data
//
#pragma once

#include "StaticNode.hpp"

class IContentView;
class File;

/// \brief node showing hex data
class HexDataViewNode : public StaticNode
{
public:
   /// ctor
   HexDataViewNode(
      const CString& displayName,
      NodeTreeIconID iconID,
      const File& file,
      size_t startOffset,
      size_t dataSize)
      :StaticNode(displayName, iconID),
      m_file(file),
      m_startOffset(startOffset),
      m_dataSize(dataSize)
   {
   }

   // Inherited via INode
   std::shared_ptr<IContentView> GetContentView() override;

private:
   /// file data to display
   const File& m_file;

   /// start offset in file
   size_t m_startOffset;

   /// size of data to display
   size_t m_dataSize;
};
