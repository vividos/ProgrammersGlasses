//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImage.cpp
/// \brief C64 disk image
//
#include "stdafx.h"
#include "DiskImage.hpp"

/// list of supported disk image infos
std::vector<DiskImageInfo> g_diskImageTypeToInfoList
{
   {
      DiskImageType::Invalid,
      0,
      0,
      0,
      0,
      1,
      1,
      {},
      false,
   },
   {
      DiskImageType::D64,
      35,
      683,
      174848,
      18,
      10,
      3,
      {
         { (BYTE)1, (BYTE)17, 21 },
         { (BYTE)18, (BYTE)24, 19 },
         { (BYTE)25, (BYTE)30, 18 },
         { (BYTE)31, (BYTE)35, 17 },
      },
      false,
   },
   {
      DiskImageType::D64,
      35,
      683,
      174848 + 683,
      18,
      10,
      3,
      {
         { (BYTE)1, (BYTE)17, 21 },
         { (BYTE)18, (BYTE)24, 19 },
         { (BYTE)25, (BYTE)30, 18 },
         { (BYTE)31, (BYTE)35, 17 },
      },
      true, // with error codes
   },
   {
      DiskImageType::D64Extended,
      40,
      768,
      196608,
      18,
      10,
      3,
      {
         { (BYTE)1, (BYTE)17, 21 },
         { (BYTE)18, (BYTE)24, 19 },
         { (BYTE)25, (BYTE)30, 18 },
         { (BYTE)31, (BYTE)40, 17 },
      },
      false,
   },
   {
      DiskImageType::D64Extended,
      40,
      768,
      196608 + 768,
      18,
      10,
      3,
      {
         { (BYTE)1, (BYTE)17, 21 },
         { (BYTE)18, (BYTE)24, 19 },
         { (BYTE)25, (BYTE)30, 18 },
         { (BYTE)31, (BYTE)40, 17 },
      },
      true, // with error codes
   },
   {
      DiskImageType::D71,
      70,
      1366,
      349696,
      18,
      10,
      3,
      {
         { (BYTE)1, (BYTE)17, 21 },
         { (BYTE)18, (BYTE)24, 19 },
         { (BYTE)25, (BYTE)30, 18 },
         { (BYTE)31, (BYTE)35, 17 },
         { (BYTE)36, (BYTE)52, 21 },
         { (BYTE)53, (BYTE)59, 19 },
         { (BYTE)60, (BYTE)65, 18 },
         { (BYTE)66, (BYTE)70, 17 },
      },
      false,
   },
   {
      DiskImageType::D71,
      70,
      1366,
      349696 + 1366,
      18,
      10,
      3,
      {
         { (BYTE)1, (BYTE)17, 21 },
         { (BYTE)18, (BYTE)24, 19 },
         { (BYTE)25, (BYTE)30, 18 },
         { (BYTE)31, (BYTE)35, 17 },
         { (BYTE)36, (BYTE)52, 21 },
         { (BYTE)53, (BYTE)59, 19 },
         { (BYTE)60, (BYTE)65, 18 },
         { (BYTE)66, (BYTE)70, 17 },
      },
      true, // with error codes
   },
   {
      DiskImageType::D81,
      80,
      3200,
      819200,
      40,
      1,
      1,
      {
         { (BYTE)1, (BYTE)80, 40 },
      },
      false,
   },
   {
      DiskImageType::D81,
      80,
      3200,
      819200 + 3200,
      40,
      1,
      1,
      {
         { (BYTE)1, (BYTE)80, 40 },
      },
      true, // with error codes
   }
};

bool DiskImage::IsDiskImage(const BYTE* data, size_t size)
{
   return FindDiskImageInfo(data, size).diskImageType != Invalid;
}

const DiskImageInfo& DiskImage::FindDiskImageInfo(const BYTE* data, size_t size)
{
   (void)data;

   for (const auto& diskImageInfo : g_diskImageTypeToInfoList)
   {
      if (diskImageInfo.fileSize == size)
      {
         return diskImageInfo;
      }
   }

   return g_diskImageTypeToInfoList[0];
}

DiskImage::DiskImage(const BYTE* data, size_t size)
   :m_diskImageInfo(FindDiskImageInfo(data, size)),
   m_data(data),
   m_size(size)
{
   ScanBlockAvailabilityMap();
   if (m_diskImageInfo.errorFlags)
      ScanBlockErrorList();
}

bool DiskImage::IsValidTrack(BYTE track) const
{
   if (track == 0)
      return false; // tracks are 1-based

   for (const auto& trackRange : m_diskImageInfo.trackToNumSectorList)
   {
      BYTE startTrack = std::get<0>(trackRange);
      BYTE endTrack = std::get<1>(trackRange);

      if (track >= startTrack && track <= endTrack)
         return true;
   }

   return false;
}

bool DiskImage::IsValidSector(BYTE track, BYTE sector) const
{
   return IsValidTrack(track) &&
      sector < GetNumSectorsInTrack(track);
}

size_t DiskImage::GetNumSectorsInTrack(BYTE track) const
{
   if (track == 0)
      return 0; // tracks are 1-based

   for (const auto& trackRange : m_diskImageInfo.trackToNumSectorList)
   {
      BYTE startTrack = std::get<0>(trackRange);
      BYTE endTrack = std::get<1>(trackRange);

      if (track >= startTrack && track <= endTrack)
      {
         return std::get<2>(trackRange);
      }
   }

   return 0;
}

bool DiskImage::GetNextBlockInChain(BYTE& track, BYTE& sector) const
{
   if (!IsValidSector(track, sector))
      return false;

   size_t offset = GetBlockOffset(track, sector);
   if (offset >= m_size)
      return false;

   const BYTE* block = m_data + offset;
   track = block[0];
   sector = block[1];

   return IsValidSector(track, sector);
}

size_t DiskImage::GetBlockOffset(BYTE track, BYTE sector) const
{
   size_t blockNumber = 0;
   for (const auto& trackRange : m_diskImageInfo.trackToNumSectorList)
   {
      BYTE startTrack = std::get<0>(trackRange);
      BYTE endTrack = std::get<1>(trackRange);

      size_t numSectors = std::get<2>(trackRange);
      if (track >= startTrack && track <= endTrack)
      {
         // in this range
         blockNumber += size_t(track - startTrack) * numSectors + sector;
         break;
      }
      else
      {
         // after this range
         blockNumber += size_t(endTrack - startTrack + 1) * numSectors;
      }
   }

   return blockNumber * 256;
}

BYTE DiskImage::GetNextInterleavedSectorNumber(BYTE track, BYTE sector) const
{
   BYTE interleaveValue = track == m_diskImageInfo.directoryTrack
      ? m_diskImageInfo.bamInterleave
      : m_diskImageInfo.sectorInterleave;

   size_t numSectors = GetNumSectorsInTrack(track);
   return static_cast<BYTE>(size_t(sector + interleaveValue) % numSectors);
}

bool DiskImage::IsSectorAvailable(BYTE track, BYTE sector) const
{
   if (m_blockAvailabilityMap.empty())
      return false;

   auto key = std::make_tuple(track, sector);
   auto iter = m_blockAvailabilityMap.find(key);
   return iter != m_blockAvailabilityMap.end()
      ? iter->second
      : false;
}

bool DiskImage::HasSectorError(BYTE track, BYTE sector) const
{
   auto key = std::make_tuple(track, sector);
   auto iter = m_blockErrorMap.find(key);
   return iter != m_blockErrorMap.end()
      ? iter->second > 0
      : false;
}

size_t DiskImage::GetDiskLabelOffset() const
{
   size_t directoryBlockOffset =
      GetBlockOffset(m_diskImageInfo.directoryTrack, 0);

   return directoryBlockOffset +
      (m_diskImageInfo.diskImageType != D81
      ? 0x90
      : 0x04);
}

void DiskImage::GetDirectoryStartTrackSector(BYTE& track, BYTE& sector) const
{
   track = m_diskImageInfo.directoryTrack;
   sector = m_diskImageInfo.diskImageType != D81 ? 1 : 3;
}

void DiskImage::ScanBlockAvailabilityMap()
{
   m_numFreeBlocks = 0;

   if (m_diskImageInfo.diskImageType == Invalid)
      return;

   size_t freeBlocksOffset = GetBlockOffset(m_diskImageInfo.directoryTrack, 0) + 4;

   if (m_diskImageInfo.diskImageType == D81)
      freeBlocksOffset = GetBlockOffset(m_diskImageInfo.directoryTrack, 1) + 0x10;

   size_t bamBitsOffset = freeBlocksOffset + 1;

   for (const auto& trackRange : m_diskImageInfo.trackToNumSectorList)
   {
      BYTE startTrack = std::get<0>(trackRange);
      BYTE endTrack = std::get<1>(trackRange);
      size_t numSectors = std::get<2>(trackRange);

      for (BYTE track = startTrack; track <= endTrack; track++)
      {
         if (track != m_diskImageInfo.directoryTrack ||
            (m_diskImageInfo.diskImageType == D71 && track != 53))
            m_numFreeBlocks += m_data[freeBlocksOffset];

         DWORD bamBits = 0;
         if (m_diskImageInfo.diskImageType != D81)
         {
            bamBits = m_data[bamBitsOffset] |
               (DWORD(m_data[bamBitsOffset + 1]) << 8) |
               (DWORD(m_data[bamBitsOffset + 2]) << 16);
         }
         else
         {
            // get the last bits in the loop, below, to keep using a DWORD
            bamBits = m_data[bamBitsOffset] |
               (DWORD(m_data[bamBitsOffset + 1]) << 8) |
               (DWORD(m_data[bamBitsOffset + 2]) << 16) |
               (DWORD(m_data[bamBitsOffset + 3]) << 24);
         }

         for (BYTE sector = 0; sector < numSectors; sector++)
         {
            auto key = std::make_tuple(track, sector);
            bool isAvailable = (bamBits & 1) != 0;
            bamBits >>= 1;

            m_blockAvailabilityMap[key] = isAvailable;

            // get the last 8 bits for sectors 32 to 39
            if (m_diskImageInfo.diskImageType == D81 &&
               sector == 31)
               bamBits = m_data[bamBitsOffset + 4];
         }

         if (m_diskImageInfo.diskImageType == D64 ||
            m_diskImageInfo.diskImageType == D64Extended)
         {
            freeBlocksOffset += 4;
            bamBitsOffset += 4;

            if (track == 35 &&
               m_diskImageInfo.diskImageType == D64Extended)
            {
               // switch to the area 0xAC-0xBF (Dolphin DOS)
               freeBlocksOffset = GetBlockOffset(m_diskImageInfo.directoryTrack, 0) + 0xAC;
               bamBitsOffset = freeBlocksOffset + 1;
            }
         }

         if (m_diskImageInfo.diskImageType == D71)
         {
            if (track < 35)
            {
               freeBlocksOffset += 4;
               bamBitsOffset += 4;
            }
            else if (track == 35)
            {
               // switch to block 53/0 for D71 format for BAM bits only
               bamBitsOffset = GetBlockOffset(53, 0);
               // free blocks continue at 0xDD-0xFF
               freeBlocksOffset = GetBlockOffset(m_diskImageInfo.directoryTrack, 0) + 0xDD;
            }
            else
            {
               freeBlocksOffset++;
               bamBitsOffset += 3;
            }
         }

         if (m_diskImageInfo.diskImageType == D81)
         {
            freeBlocksOffset += 6;
            bamBitsOffset += 6;

            if (track == 40)
            {
               // switch to block 40/2 for second half of BAM
               freeBlocksOffset = GetBlockOffset(m_diskImageInfo.directoryTrack, 2) + 0x10;
               bamBitsOffset = freeBlocksOffset + 1;
            }
         }
      }
   }
}

void DiskImage::ScanBlockErrorList()
{
   if (!m_diskImageInfo.errorFlags)
      return;

   size_t offset = m_diskImageInfo.fileSize - m_diskImageInfo.numBlocks;

   for (const auto& trackRange : m_diskImageInfo.trackToNumSectorList)
   {
      BYTE startTrack = std::get<0>(trackRange);
      BYTE endTrack = std::get<1>(trackRange);
      size_t numSectors = std::get<2>(trackRange);

      for (BYTE track = startTrack; track <= endTrack; track++)
      {
         for (BYTE sector = 0; sector < numSectors; sector++)
         {
            auto key = std::make_tuple(track, sector);
            BYTE errorCode = m_data[offset++];

            m_blockErrorMap[key] = errorCode;
         }
      }
   }
}
