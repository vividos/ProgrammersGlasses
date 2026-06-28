//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2026 Michael Fink
//
/// \file HexDataViewNode.cpp
/// \brief node showing hex data
//
#include "stdafx.h"
#include "HexDataViewNode.hpp"
#include "userinterface/HexDataView.hpp"

std::shared_ptr<IContentView> HexDataViewNode::GetContentView()
{
   return std::make_shared<HexDataView>(
      m_file,
      m_startOffset,
      m_dataSize);
}
