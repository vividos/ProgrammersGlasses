//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file Helper.hpp
/// \brief helper functions
//
#pragma once

#include <vector>

/// Splits a string by one or more split characters
std::vector<CString> StringSplit(const CString& text, LPCTSTR splitChars, bool addEmptyEntries);
