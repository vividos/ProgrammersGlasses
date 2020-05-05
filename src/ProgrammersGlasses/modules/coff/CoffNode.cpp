//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CoffNode.cpp
/// \brief single node in a tree structure of a COFF file
//
#include "stdafx.h"
#include "CoffNode.hpp"
#include "userinterface/CodeTextView.hpp"

std::shared_ptr<IContentView> CoffNode::GetContentView()
{
   return std::make_shared<CodeTextView>(m_text);
}
