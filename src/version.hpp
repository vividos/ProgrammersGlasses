//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file version.hpp
/// \brief version number
//
#pragma once

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define RELEASE_NUMBER 0
#define BUILD_NUMBER 0
#define BUILD_YEAR 2020

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

#define COPYRIGHT_TEXT "Copyright © 2020-" STRINGIFY(BUILD_YEAR) " Michael Fink"

#define VERSION_TEXT STRINGIFY(MAJOR_VERSION) "." STRINGIFY(MINOR_VERSION)
