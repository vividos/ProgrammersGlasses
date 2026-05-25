//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file StringHelper.hpp
/// \brief string helper functions
//
#pragma once

#include <vector>

/// Splits a string by one or more split characters
std::vector<CString> StringSplit(const CString& text, LPCTSTR splitChars, bool addEmptyEntries);

/// indents a multiline string with space characters
void IndentText(CString& text, int numberOfSpaces);

/// checks if the text to search contains the text to find somewhere, using
/// case independent comparison
bool ContainsIgnoreCase(const CString& textToSearch, const CString& textToFind);

/// escapes all non-printable characters to be displayed correctly
CString EscapeText(CString text);
