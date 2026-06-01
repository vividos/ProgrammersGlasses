//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2026 Michael Fink
//
/// \file AnonymousObjectHeaderBigObj.cpp
/// \brief header definition of BigObj anonymous object files
//
#include "stdafx.h"
#include "AnonymousObjectHeaderBigObj.hpp"
#include "CoffHeader.hpp"
#include "ImportObjectHeader.hpp"
#include "DisplayFormatHelper.hpp"

const StructDefinition g_definitionAnonymousObjectHeaderBigObj = StructDefinition({
   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::signature1),
      sizeof(AnonymousObjectHeaderBigObj::signature1),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderSignature1ToDisplayText,
      _T("Signature 1")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::signature2),
      sizeof(AnonymousObjectHeaderBigObj::signature2),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderSignature2ToDisplayText,
      _T("Signature 2")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::version),
      sizeof(AnonymousObjectHeaderBigObj::version),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapNonCoffHeaderVersionToDisplayText,
      _T("Version")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::targetMachine),
      sizeof(AnonymousObjectHeaderBigObj::targetMachine),
      2,
      true, // little-endian
      StructFieldType::valueMapping,
      g_mapCoffTargetMachineToDisplayText,
      _T("Target machine")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::timeStamp),
      sizeof(AnonymousObjectHeaderBigObj::timeStamp),
      4,
      true, // little-endian
      [](LPCVOID data, size_t)
      {
         time_t time = *reinterpret_cast<const DWORD*>(data);
         return DisplayFormatHelper::FormatDateTime(time);
      },
      _T("Time stamp")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::classID),
      sizeof(AnonymousObjectHeaderBigObj::classID),
      1,
      true, // little-endian
      [](LPCVOID data, size_t size)
      {
         return DisplayFormatHelper::FormatGUID((const BYTE*)data, size);
      },
      _T("COM object class ID")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::sizeOfData),
      sizeof(AnonymousObjectHeaderBigObj::sizeOfData),
      1,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Size of data")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::flags),
      sizeof(AnonymousObjectHeaderBigObj::flags),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Flags")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::metadataSize),
      sizeof(AnonymousObjectHeaderBigObj::metadataSize),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("CLR Metadata size")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::metadataOffset),
      sizeof(AnonymousObjectHeaderBigObj::metadataOffset),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("CLR Metadata offset")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::numberOfSections),
      sizeof(AnonymousObjectHeaderBigObj::numberOfSections),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of sections")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::pointerToSymbolTable),
      sizeof(AnonymousObjectHeaderBigObj::pointerToSymbolTable),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Pointer to symbol table")),

   StructField(
      offsetof(AnonymousObjectHeaderBigObj, AnonymousObjectHeaderBigObj::numberOfSymbols),
      sizeof(AnonymousObjectHeaderBigObj::numberOfSymbols),
      4,
      true, // little-endian
      StructFieldType::unsignedInteger,
      _T("Number of symbols")),
   });
