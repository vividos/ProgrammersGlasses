//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file AnonymousObjectHeader.cpp
/// \brief header definition of anonymous object files
//
#include "stdafx.h"
#include "AnonymousObjectHeader.hpp"
#include "CoffHeader.hpp"
#include "ImportObjectHeader.hpp"
#include "DisplayFormatHelper.hpp"

const StructDefinition g_definitionAnonymousObjectHeader = StructDefinition({
   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::signature1),
      sizeof(AnonymousObjectHeader::signature1),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderSignature1ToDisplayText,
      _T("Signature 1")),

   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::signature2),
      sizeof(AnonymousObjectHeader::signature2),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderSignature2ToDisplayText,
      _T("Signature 2")),

   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::version),
      sizeof(AnonymousObjectHeader::version),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderVersionToDisplayText,
      _T("Version")),

   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::targetMachine),
      sizeof(AnonymousObjectHeader::targetMachine),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapCoffTargetMachineToDisplayText,
      _T("Target machine")),

   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::timeStamp),
      sizeof(AnonymousObjectHeader::timeStamp),
      4,
      true, // little-endian
      [](LPCVOID data, size_t)
      {
         time_t time = *reinterpret_cast<const DWORD*>(data);
         return DisplayFormatHelper::FormatDateTime(time);
      },
      _T("Time stamp")),

   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::classID),
      sizeof(AnonymousObjectHeader::classID),
      1,
      true, // little-endian
      [](LPCVOID data, size_t size)
      {
         return DisplayFormatHelper::FormatGUID((const BYTE*)data, size);
      },
      _T("COM object class ID")),

   StructField(
      offsetof(AnonymousObjectHeader, AnonymousObjectHeader::sizeOfData),
      sizeof(AnonymousObjectHeader::sizeOfData),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Size of data")),
   });
