//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2026 Michael Fink
//
/// \file version.hpp
/// \brief version number
//
#pragma once

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 0
#define BUILD_NUMBER 0
#define GIT_HASH "none"
#define BUILD_YEAR 2020

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

#define COPYRIGHT_TEXT "Copyright (c) 2020-" STRINGIFY(BUILD_YEAR) " Michael Fink"

#define PRODUCT_VERSION_TEXT STRINGIFY(MAJOR_VERSION) "." STRINGIFY(MINOR_VERSION) "." STRINGIFY(PATCH_VERSION)
#define FILE_VERSION_TEXT STRINGIFY(MAJOR_VERSION) "." STRINGIFY(MINOR_VERSION) "." STRINGIFY(PATCH_VERSION) " (" GIT_HASH ")"

#define WEBLINK_PROJECT_PAGE "https://github.com/vividos/ProgrammersGlasses/"
#define WEBLINK_LICENSE "https://github.com/vividos/ProgrammersGlasses/blob/master/LICENSE.md"
