//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020-2023 Michael Fink
//
/// \file DosMzHeader.hpp
/// \brief MS-DOS MZ header struct
//
#pragma once

#include "StructDefinition.hpp"

#pragma pack(push, 1)

/// MS-DOS MZ header
/// \see http://www.pinvoke.net/default.aspx/Structures.IMAGE_DOS_HEADER
/// \see http://www.delorie.com/djgpp/doc/exe/
/// \see https://marcin-chwedczuk.github.io/a-closer-look-at-portable-executable-msdos-stub
struct DosMzHeader
{
   /// e_magic: 'MZ', the magic number
   CHAR magicNumber[2];

   /// e_cblp: number of bytes in the last 256-byte block that are used; when
   /// 0, the entire block is used
   WORD bytesInLastBlock;

   /// e_cp: number of whole 256-byte blocks in the file that are used
   WORD blocksInFile;

   /// e_crlc: number of relocations stored after this header; may be zero
   WORD numRelocs;

   /// e_cparhdr: number of paragraphs (16-byte blocks) after this header
   WORD numHeaderParagraphs;

   /// e_minalloc: number of paragraphs of additional memory that the program will need
   WORD minExtraParagraphs;

   /// e_maxalloc: maximum number of paragraphs the program will use
   WORD maxExtraParagraphs;

   /// e_ss: initial value of the SS (stack segment) register, relative to the
   /// segment the program was loaded at
   WORD initialSS;

   /// e_sp: initial value of the SP (stack pointer) register
   WORD initialSP;

   /// e_csum: checksum value
   WORD checksum;

   /// e_ip: initial value of the IP (instruction pointer) register
   WORD initialIP;

   /// e_cs: initial value of the CS (code segment) register, relative to the
   /// segment the program was loaded at
   WORD initialCS;

   /// e_lfarlc: offset to the relocation table from the start of the file
   WORD relocTableOffset;

   /// e_ovno: overlay number; normally set to zero, meaning it's the main program
   WORD overlayNumber;

   /// e_res: reserved words
   WORD reserved[4];

   /// e_oemid: OEM identifier
   WORD oemID;

   /// e_oeminfo: OEM info
   WORD oemInfo;

   /// e_res2: reserved words, part 2
   WORD reserved2[10];

   /// e_lfanew: file offset to the new exe header (e.g. PE)
   DWORD newExecutableHeader;
};

static_assert(sizeof(DosMzHeader) == 0x40,
   "DOS MZ header must be 64 bytes long");

static_assert(sizeof(DosMzHeader) == sizeof(IMAGE_DOS_HEADER),
   "DOS MZ header must have same size as IMAGE_DOS_HEADER");

#pragma pack(pop)

/// structure definition for DOS MZ header
extern const StructDefinition g_definitionDosMzHeader;
