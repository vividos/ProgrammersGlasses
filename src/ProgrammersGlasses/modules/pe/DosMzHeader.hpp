//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file DosMzHeader.hpp
/// \brief MS-DOS MZ header struct
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// MS-DOS MZ header
/// \see http://www.delorie.com/djgpp/doc/exe/
/// \see https://marcin-chwedczuk.github.io/a-closer-look-at-portable-executable-msdos-stub
struct DosMzHeader
{
   /// 'MZ', the magic number
   CHAR magicNumber[2];

   /// number of bytes in the last 256-byte block that are used; when 0, the
   /// entire block is used
   WORD bytesInLastBlock;

   /// number of whole 256-byte blocks in the file that are used
   WORD blocksInFile;

   /// number of relocations stored after this header; may be zero
   WORD numRelocs;

   /// number of paragraphs (16-byte blocks) after this header
   WORD numHeaderParagraphs;

   /// number of paragraphs of additional memory that the program will need
   WORD minExtraParagraphs;

   /// maximum number of paragraphs the program will use
   WORD maxExtraParagraphs;

   /// initial value of the SS (stack segment) register, relative to the
   /// segment the program was loaded at
   WORD initialSS;

   /// initial value of the SP (stack pointer) register
   WORD initialSP;

   /// checksum value
   WORD checksum;

   /// initial value of the IP (instruction pointer) register
   WORD initialIP;

   /// initial value of the CS (code segment) register, relative to the
   /// segment the program was loaded at
   WORD initialCS;

   /// offset to the relocation table from the start of the file
   WORD relocTableOffset;

   /// overlay number; normally set to zero, meaning it's the main program
   WORD overlayNumber;
};

static_assert(sizeof(DosMzHeader) == 0x1C, "DOS MZ header must be 28 bytes long");

#pragma pack(pop)

/// structure definition for DOS MZ header
extern StructDefinition g_definitionDosMzHeader;
