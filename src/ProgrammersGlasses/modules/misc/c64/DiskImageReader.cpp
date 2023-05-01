//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2023 Michael Fink
//
/// \file DiskImageReader.cpp
/// \brief C64 disk image reader
//
#include "stdafx.h"
#include "DiskImageReader.hpp"
#include "DiskImageDirectoryEntry.hpp"
#include "modules/CodeTextViewNode.hpp"

bool DiskImageReader::IsDiskImageFile(const File& file)
{
   return DiskImage::IsDiskImage(file.Data<BYTE>(), file.Size());
}

DiskImageReader::DiskImageReader(const File& file)
   :m_file(file),
   m_image(file.Data<BYTE>(), file.Size())
{
}

void DiskImageReader::Load()
{
   auto rootNode = std::make_shared<CodeTextViewNode>(_T("Directory"),
      NodeTreeIconID::nodeTreeIconDocument);

   CString summaryText;
   AddDirectoryText(summaryText);
   rootNode->SetText(summaryText);

   AddBlockAvailabilityMap(rootNode);

   m_rootNode = rootNode;
}

void DiskImageReader::AddDirectoryText(CString& directoryText) const
{
   size_t diskLabelOffset = m_image.GetDiskLabelOffset();

   CString diskLabel = GetAsciiFromPetascii(
      m_file.Data<BYTE>(diskLabelOffset), 16);

   CString diskId = GetAsciiFromPetascii(
      m_file.Data<BYTE>(diskLabelOffset + 16 + 2), 5);

   directoryText.AppendFormat(_T("0 \"%.16s\" %.5s\n"),
      diskLabel.GetString(),
      diskId.GetString());

   BYTE track = 0;
   BYTE sector = 0xFF;
   m_image.GetDirectoryStartTrackSector(track, sector);

   if (m_image.IsValidSector(track, sector))
   {
      do
      {
         size_t blockOffset = m_image.GetBlockOffset(track, sector);

         for (size_t entryOffset = 0; entryOffset < 256; entryOffset += 32)
         {
            const DiskImageDirectoryEntry& directoryEntry =
               *m_file.Data<DiskImageDirectoryEntry>(blockOffset + entryOffset);

            if (directoryEntry.fileType == 0x00)
               continue; // scratched file

            WORD blockSize =
               (WORD(directoryEntry.fileSizeHigh) << 8) |
               directoryEntry.fileSizeLow;

            CString filename = GetAsciiFromPetascii(
               directoryEntry.filename, 16) + _T("\"");

            CString typeText = FormatFileType(
               directoryEntry.fileType,
               m_image.GetDiskImageType() == D81);

            directoryText.AppendFormat(
               _T("%-5u\"%-.17s %s\n"),
               blockSize, filename.GetString(),
               typeText.GetString());
         }

      } while (m_image.GetNextBlockInChain(track, sector));
   }

   size_t numFreeBlocks = m_image.GetNumFreeBlocks();
   directoryText.AppendFormat(_T("%zu blocks free."), numFreeBlocks);
}

CString DiskImageReader::GetAsciiFromPetascii(const BYTE* data, size_t size)
{
   CString text;

   for (size_t index = 0; index < size; index++)
   {
      BYTE petasciiChar = data[index];

      CHAR asciiChar = petasciiChar & 0x7f;

      if (petasciiChar >= 'A' && petasciiChar <= 'Z')
         asciiChar += 32;
      else if (petasciiChar >= 'a' && petasciiChar <= 'z')
         asciiChar -= 32;
      else if (petasciiChar == 0x7f)
         asciiChar = 0x3f;

      text += asciiChar;
   }

   return text;
}

CString DiskImageReader::FormatFileType(BYTE fileType, bool isD81)
{
   CString typeText;
   switch (fileType & 7)
   {
   case 0: typeText = _T("DEL"); break;
   case 1: typeText = _T("SEQ"); break;
   case 2: typeText = _T("PRG"); break;
   case 3: typeText = _T("USR"); break;
   case 4: typeText = _T("REL"); break;
   case 5: typeText = isD81 ? _T("CBM") : _T("???"); break;
   default: typeText = _T("???"); break;
   }

   TCHAR prefixChar = _T(' ');
   if ((fileType & 0x80) == 0) typeText += _T("*");
   if ((fileType & 0x40) != 0) typeText += _T("<");
   if ((fileType & 0x20) != 0) prefixChar = _T('@');

   return prefixChar + typeText;
}

void DiskImageReader::AddBlockAvailabilityMap(std::shared_ptr<StaticNode> node) const
{
   auto bamNode = std::make_shared<CodeTextViewNode>(
      _T("Block Availability Map"),
      NodeTreeIconID::nodeTreeIconDocument);

   CString text{ _T("Block Availability Map\n") };

   size_t maxSectorNumber = m_image.GetNumSectorsInTrack(1);

   if (maxSectorNumber < 25)
   {
      text += _T("    0    5    10   15   20\n");
      text += _T("    |    |    |    |    |\n");
   }
   else
   {
      text += _T("    0    5    10   15   20   25   30   35   40\n");
      text += _T("    |    |    |    |    |    |    |    |    |\n");
   }

   text += _T("   +") + CString(_T('-'), (int)maxSectorNumber) + _T("+\n");

   BYTE lastTrack = 1;
   BYTE lastTrackNumSectors = 0;

   for (BYTE track = 1; m_image.IsValidTrack(track); track++)
   {
      if (track == 1 || (track % 5) == 0)
         text.AppendFormat(_T("% 2u "), track);
      else
         text += _T("   ");

      BYTE maxNumSector = static_cast<BYTE>(m_image.GetNumSectorsInTrack(track));

      text.AppendChar(_T('|'));

      for (BYTE sector = 0; sector < maxNumSector; sector++)
      {
         bool isAvailable = m_image.IsSectorAvailable(track, sector);

         text.AppendChar(isAvailable ? _T(' ') : _T('*'));
      }

      if (track > 1 && maxNumSector < lastTrackNumSectors)
      {
         // track length changed, so add some nice connecting lines
         int numDashChars = lastTrackNumSectors - maxNumSector - 1;
         text += _T("+") +
            (numDashChars > 0 ? CString(_T('-'), numDashChars) : _T("")) +
            _T("+\n");
      }
      else
         text.Append(_T("|\n"));

      // D71 changes disk side; add a separator
      if (track == 35 &&
         m_image.GetDiskImageType() == D71)
      {
         text += _T("   +") + CString(_T('-'), (int)maxSectorNumber - 4) + _T("+---+\n");
      }

      lastTrack = track;
      lastTrackNumSectors = maxNumSector;
   }

   size_t lastTrackSectorNumber = m_image.GetNumSectorsInTrack(lastTrack);
   text += _T("   +") + CString(_T('-'), (int)lastTrackSectorNumber) + _T("+\n");

   size_t numFreeBlocks = m_image.GetNumFreeBlocks();
   text.AppendFormat(_T("%zu blocks free.\n\n* = block allocated"), numFreeBlocks);

   bamNode->SetText(text);

   node->ChildNodes().push_back(bamNode);
}
