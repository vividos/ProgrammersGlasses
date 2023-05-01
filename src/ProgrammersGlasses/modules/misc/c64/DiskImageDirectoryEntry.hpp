//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImageDirectoryEntry.hpp
/// \brief C64 disk image directory entry
//
#pragma once

#pragma pack(push, 1)

/// directory entry for a C64 disk image (d64, d71 and d81)
struct DiskImageDirectoryEntry
{
   /// next track, or 0 when at the end; only set at the start of a block
   BYTE nextTrack;
   /// next sector; only set at the start of a block
   BYTE nextSector;

   /// file type and bits
   BYTE fileType;

   /// track number of file start
   BYTE fileStartTrack;
   /// sector number of file start
   BYTE fileStartSector;

   /// filename in PETASCII
   BYTE filename[16];

   /// track of side sector block for REL files
   BYTE relSideSectorBlockTrack;
   /// sector of side sector block for REL files
   BYTE relSideSectorBlockSector;
   /// file record length for REL files
   BYTE relFileRecordLength;

   /// unused (except for GEOS files)
   BYTE unused[6];

   /// file size in blocks, low byte
   BYTE fileSizeLow;
   /// file size in blocks, high byte
   BYTE fileSizeHigh;
};

#pragma pack(pop)

static_assert(sizeof(DiskImageDirectoryEntry) == 0x20,
   "Disk image directory entry must be 32 bytes long");
