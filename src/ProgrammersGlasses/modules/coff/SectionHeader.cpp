//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SectionHeader.cpp
/// \brief Section header definition
//
#include "stdafx.h"
#include "SectionHeader.hpp"
#include "DisplayFormatHelper.hpp"

StructDefinition g_definitionSectionHeader = StructDefinition({
   StructField(
      offsetof(SectionHeader, SectionHeader::name),
      sizeof(SectionHeader::name),
      1,
      true, // little-endian
      StructFieldType::text,
      _T("Section name")),

   StructField(
      offsetof(SectionHeader, SectionHeader::virtualSize),
      sizeof(SectionHeader::virtualSize),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Virtual Size")),

   StructField(
      offsetof(SectionHeader, SectionHeader::virtualAddress),
      sizeof(SectionHeader::virtualAddress),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Virtual Address")),
   });
