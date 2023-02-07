//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file SidFileHeader.cpp
/// \brief SID file header
//
#include "stdafx.h"
#include "SidFileHeader.hpp"

const std::map<DWORD, LPCTSTR> g_mapSidMagicIdsToDisplayText =
{
   { 0x50534944, _T("PSID")},
   { 0x52534944, _T("RSID")},
};

const StructDefinition g_definitionSidFileHeader = StructDefinition({
   StructField(
      offsetof(SidFileHeader, SidFileHeader::magicId),
      sizeof(SidFileHeader::magicId),
      4,
      false, // big-endian
      StructFieldType::valueMapping,
      g_mapSidMagicIdsToDisplayText,
      _T("Magic ID")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::version),
      sizeof(SidFileHeader::version),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("SID version")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::dataOffset),
      sizeof(SidFileHeader::dataOffset),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Offset of C64 data in file")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::loadAddress),
      sizeof(SidFileHeader::loadAddress),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Load address")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::initAddress),
      sizeof(SidFileHeader::initAddress),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Init address")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::playAddress),
      sizeof(SidFileHeader::playAddress),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Play address")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::numSongs),
      sizeof(SidFileHeader::numSongs),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Number of songs")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::startSong),
      sizeof(SidFileHeader::startSong),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Start song")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::speed),
      sizeof(SidFileHeader::speed),
      4,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Speed bit flags")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::name),
      sizeof(SidFileHeader::name),
      1,
      true,
      StructFieldType::text,
      _T("Name")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::author),
      sizeof(SidFileHeader::author),
      1,
      true,
      StructFieldType::text,
      _T("Author")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::released),
      sizeof(SidFileHeader::released),
      1,
      true,
      StructFieldType::text,
      _T("Released")),
   });

/// Formats a SID address where the data bytes contain the middle two nibbles of the address
static CString FormatSidAddress(LPCVOID data, size_t size)
{
   if (size < 1)
      return _T("invalid");
   BYTE address = *reinterpret_cast<const BYTE*>(data);
   if (address == 0)
      return _T("none");

   CString addressText;
   addressText.Format(_T("$D%02x0"), address);
   return addressText;
}

const StructDefinition g_definitionSidV2FileHeader = StructDefinition(
   g_definitionSidFileHeader, {
   StructField(
      offsetof(SidFileHeader, SidFileHeader::flags),
      sizeof(SidFileHeader::flags),
      2,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Flags")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::startPage),
      sizeof(SidFileHeader::startPage),
      1,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Start page")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::pageLength),
      sizeof(SidFileHeader::pageLength),
      1,
      false, // big-endian
      StructFieldType::unsignedInteger,
      _T("Page length")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::secondSidAddress),
      sizeof(SidFileHeader::secondSidAddress),
      1,
      false, // big-endian
      &FormatSidAddress,
      _T("Second SID address")),

   StructField(
      offsetof(SidFileHeader, SidFileHeader::thirdSidAddress),
      sizeof(SidFileHeader::thirdSidAddress),
      1,
      false, // big-endian
      &FormatSidAddress,
      _T("Third SID address")),
   });
