//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file CodeTextViewNode.cpp
/// \brief node showing a code text view
//
#include "stdafx.h"
#include "CodeTextViewNode.hpp"
#include "userinterface/CodeTextView.hpp"

std::shared_ptr<IContentView> CodeTextViewNode::GetContentView()
{
   return std::make_shared<CodeTextView>(m_text);
}
