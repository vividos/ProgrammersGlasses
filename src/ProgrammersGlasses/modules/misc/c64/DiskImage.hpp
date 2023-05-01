//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImage.hpp
/// \brief C64 disk image
//
#pragma once

/// C64 disk image type
enum DiskImageType
{
   Invalid,       ///< invalid format or format not recognized
   D64,           ///< d64 with 35 tracks, from floppy 1540/1541
   D64Extended,   ///< d64 with 40 tracks
   D71,           ///< d71 with 70 tracks, from floppy 1570/1571
   D81,           ///< d81 with 80 tracks, from floppy 1581
};

/// infos for a specific disk image
struct DiskImageInfo
{
   DiskImageType diskImageType; ///< type of the disk image
   BYTE numTracks; ///< number of tracks
   size_t numBlocks; ///< number of blocks
   DWORD fileSize; ///< file size of disk image
   BYTE directoryTrack; ///< directory track number
   BYTE sectorInterleave; ///< sector interleave for tracks
   BYTE bamInterleave; ///< sector interleave for the BAM
   /// list of: start and end track and the number of sectors in them
   std::vector<std::tuple<BYTE, BYTE, size_t>> trackToNumSectorList;
   bool errorFlags; ///< when true, file has additional error codes at the end
};

/// \brief C64 disk image
/// \details Currently supported formats: D64, D71, D81.
/// Note that track numbers are 1-based, and sector numbers are 0-based.
/// \see http://unusedino.de/ec64/technical/formats/d64.html
/// \see http://unusedino.de/ec64/technical/formats/d71.html
/// \see http://unusedino.de/ec64/technical/formats/d81.html
class DiskImage
{
public:
   /// returns if given data represents a disk image handled by this class
   static bool IsDiskImage(const BYTE* data, size_t size);

   /// ctor; takes data of given size and recognizes the disk image
   DiskImage(const BYTE* data, size_t size);

   /// returns if it's a valid disk image (false when not recognized)
   bool IsValid() const
   {
      return GetDiskImageType() != Invalid;
   }

   /// returns the recognized disk image type
   DiskImageType GetDiskImageType() const
   {
      return m_diskImageInfo.diskImageType;
   }

   /// returns if given track number is valid (track numbers are 1-based)
   bool IsValidTrack(BYTE track) const;

   /// returns if given sector number at a track is valid (sector numbers are
   /// 0-based)
   bool IsValidSector(BYTE track, BYTE sector) const;

   /// returns number of sectors in track
   size_t GetNumSectorsInTrack(BYTE track) const;

   /// returns the next block in chain, or false when at the end or invalid
   /// track/sector encountered
   bool GetNextBlockInChain(BYTE& track, BYTE& sector) const;

   /// returns an offset of a block at track (1-based) and sector (0-based)
   size_t GetBlockOffset(BYTE track, BYTE sector) const;

   /// returns next sector number, taking interleaving of sector numbers in
   /// account; ignores if the sector is free
   BYTE GetNextInterleavedSectorNumber(BYTE track, BYTE sector) const;

   /// returns if given sector on track is marked as available or allocated in
   /// the BAM
   bool IsSectorAvailable(BYTE track, BYTE sector) const;

   /// returns if a given sector on a track has an error
   bool HasSectorError(BYTE track, BYTE sector) const;

   /// returns the number of free blocks on disk
   size_t GetNumFreeBlocks() const
   {
      return m_numFreeBlocks;
   }

   /// returns offset of disk label (which is 16 bytes long)
   size_t GetDiskLabelOffset() const;

   /// returns the start track and sector of the directory
   void GetDirectoryStartTrackSector(BYTE& track, BYTE& sector) const;

private:
   /// finds disk image info from given disk image data
   static const DiskImageInfo& FindDiskImageInfo(const BYTE* data, size_t size);

   /// scans block availability map
   void ScanBlockAvailabilityMap();

   /// scans block error list if present
   void ScanBlockErrorList();

private:
   /// disk image data
   const BYTE* m_data;

   /// disk image size
   size_t m_size;

   /// disk image info
   const DiskImageInfo& m_diskImageInfo;

   /// number of free blocks on the disk
   size_t m_numFreeBlocks = 0;

   /// block availability map
   std::map<std::tuple<BYTE, BYTE>, bool> m_blockAvailabilityMap;

   /// block error map
   std::map<std::tuple<BYTE, BYTE>, BYTE> m_blockErrorMap;
};
