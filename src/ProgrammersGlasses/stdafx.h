//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file stdafx.h
/// \brief precompiled header support
//
#pragma once

#pragma warning(push)
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '+' on a 4 byte value and then casting the result to a 8 byte value.Cast the value to the wider type before calling operator '+' to avoid overflow(io.2).
#pragma warning(disable: 26454) // Arithmetic overflow: '-' operation produces a negative unsigned result at compile time (io.5).
#pragma warning(disable: 26812) // The enum type 'E' is unscoped.Prefer 'enum class' over 'enum' (Enum.3).
#include <ulib/config/Wtl.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '+' on a 4 byte value and then casting the result to a 8 byte value.Cast the value to the wider type before calling operator '+' to avoid overflow(io.2).
#pragma warning(disable: 26454) // Arithmetic overflow: '-' operation produces a negative unsigned result at compile time (io.5).
#include <thirdparty/tabbingframework/atlgdix.h>
#include <thirdparty/tabbingframework/DotNetTabCtrl.h>
#include <thirdparty/tabbingframework/TabbedMDI.h>
#include <thirdparty/tabbingframework/TabbedFrame.h>
#pragma warning(pop)

#include <ulib/Path.hpp>

#include <memory>
#include <vector>

#include "Helper.hpp"
