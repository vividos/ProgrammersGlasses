//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file ImportObjectHeader.cpp
/// \brief header definition of import object files
//
#include "stdafx.h"
#include "ImportObjectHeader.hpp"
#include "CoffHeader.hpp"
#include "DisplayFormatHelper.hpp"

std::map<DWORD, LPCTSTR> g_mapNonCoffHeaderSignature1ToDisplayText =
{
   { 0x0, _T("non COFF header sig1") },
};

std::map<DWORD, LPCTSTR> g_mapNonCoffHeaderSignature2ToDisplayText =
{
   { 0xFFFF, _T("non COFF header sig2") },
};

std::map<DWORD, LPCTSTR> g_mapNonCoffHeaderVersionToDisplayText =
{
   { 0, _T("Import object") },
   { 1, _T("Anonymous object") },
   { 2, _T("ANON_OBJECT_HEADER_V2") },
};

/// mapping from import object type bits to display text
std::map<DWORD, LPCTSTR> g_mapImportObjectTypeToDisplayText =
{
   { 0, _T("IMPORT_OBJECT_CODE") },
   { 1, _T("IMPORT_OBJECT_DATA") },
   { 2, _T("IMPORT_OBJECT_CONST") },
};

/// mapping from import object name type bits to display text
std::map<DWORD, LPCTSTR> g_mapImportObjectNameTypeToDisplayText =
{
   { 0, _T("IMPORT_OBJECT_ORDINAL") },
   { 1, _T("IMPORT_OBJECT_NAME") },
   { 2, _T("IMPORT_OBJECT_NAME_NO_PREFIX") },
   { 3, _T("IMPORT_OBJECT_NAME_UNDECORATE") },
   { 4, _T("IMPORT_OBJECT_NAME_EXPORTAS") },
};

/// flags bitfield
std::vector<BitfieldDescriptor> g_listImportObjectHeaderFlagBits =
{
   BitfieldDescriptor{ 0, 2, StructFieldType::valueMapping, g_mapImportObjectTypeToDisplayText },
   BitfieldDescriptor{ 2, 3, StructFieldType::valueMapping, g_mapImportObjectNameTypeToDisplayText },
   BitfieldDescriptor{ 5, 11, StructFieldType::unsignedInteger },
};

StructDefinition g_definitionImportObjectHeader = StructDefinition({
   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::signature1),
      sizeof(ImportObjectHeader::signature1),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderSignature1ToDisplayText,
      _T("Signature 1")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::signature2),
      sizeof(ImportObjectHeader::signature2),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderSignature2ToDisplayText,
      _T("Signature 2")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::version),
      sizeof(ImportObjectHeader::version),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderVersionToDisplayText,
      _T("Version")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::targetMachine),
      sizeof(ImportObjectHeader::targetMachine),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapCoffTargetMachineToDisplayText,
      _T("Target machine")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::timeStamp),
      sizeof(ImportObjectHeader::timeStamp),
      4,
      true, // little-endian
      [](LPCVOID data, size_t)
      {
         time_t time = *reinterpret_cast<const DWORD*>(data);
         return DisplayFormatHelper::FormatDateTime(time);
      },
      _T("Time stamp")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::sizeOfData),
      sizeof(ImportObjectHeader::sizeOfData),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Size of data")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::ordinalOrHint),
      sizeof(ImportObjectHeader::ordinalOrHint),
      2,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Ordinal or hint")),

   StructField(
      offsetof(ImportObjectHeader, ImportObjectHeader::flags),
      sizeof(ImportObjectHeader::flags),
      2,
      true, // little-endian
      StructFieldType::bitfieldMapping,
      g_listImportObjectHeaderFlagBits,
      _T("Flags bitfield")),
   });
