//
// Programmer's Glasses - a developer's file content viewer
// Copyright (c) 2020 Michael Fink
//
/// \file PngImageModule.cpp
/// \brief module to load PNG image files
//
#include "stdafx.h"
#include "PngImageReader.hpp"
#include "PngHeader.hpp"
#include "modules/CodeTextViewNode.hpp"
#include "modules/StructListViewNode.hpp"

bool PngImageReader::IsPngFile(const File& file)
{
   const PngFileHeader& header = *file.Data<PngFileHeader>();

   return memcmp(header.signature, c_pngFileHeaderSignatureBytes, sizeof(PngFileHeader::signature)) == 0;
}

PngImageReader::PngImageReader(const File& file)
   :m_file(file)
{
}

void PngImageReader::Load()
{
   const PngFileHeader& fileHeader = *m_file.Data<PngFileHeader>();

   auto rootNode = std::make_shared<CodeTextViewNode>(
      _T("Summary"),
      NodeTreeIconID::nodeTreeIconDocument);

   CString summaryText;
   summaryText.Append(_T("PNG file: ") + m_file.Filename() + _T("\n\n"));
   summaryText.Append(_T("Summary:\n"));

   auto pngHeaderNode = std::make_shared<StructListViewNode>(
      _T("PNG header"),
      NodeTreeIconID::nodeTreeIconBinary,
      g_definitionPngFileHeader,
      &fileHeader,
      m_file.Data());

   rootNode->ChildNodes().push_back(pngHeaderNode);

   const BYTE* chunkPtr = m_file.Data<BYTE>(sizeof(PngFileHeader));

   while (m_file.IsValidRange(chunkPtr, sizeof(PngChunkHeader) + 4))
   {
      const PngChunkHeader& chunkHeader = *reinterpret_cast<const PngChunkHeader*>(chunkPtr);

      CString chunkType{ chunkHeader.chunkType, sizeof(chunkHeader.chunkType) };

      auto pngChunkNode = std::make_shared<StructListViewNode>(
         _T("PNG chunk: ") + chunkType,
         NodeTreeIconID::nodeTreeIconBinary,
         g_definitionPngChunkHeader,
         &chunkHeader,
         m_file.Data());

      rootNode->ChildNodes().push_back(pngChunkNode);

      if (chunkType == _T("IHDR"))
      {
         const PngImageHeader& imageHeader = *reinterpret_cast<const PngImageHeader*>(chunkPtr + 8);

         summaryText.AppendFormat(_T("Image size: %u x %u (%u bit per channel)\n"),
            SwapEndianness(imageHeader.width),
            SwapEndianness(imageHeader.height),
            imageHeader.bitDepth);

         summaryText.AppendFormat(_T("Image type: %s\n"),
            GetValueFromMapOrDefault<DWORD>(
               g_mapPngColourTypeToDisplayText,
               (DWORD)imageHeader.colourType,
               _T("invalid")));

         summaryText.AppendFormat(_T("Interlaced: %s\n"),
            imageHeader.interlaceMethod == 0 ? _T("no") :
            imageHeader.interlaceMethod == 1 ? _T("Adam7") : _T("invalid"));

         auto pngImageHeaderNode = std::make_shared<StructListViewNode>(
            _T("PNG image header"),
            NodeTreeIconID::nodeTreeIconBinary,
            g_definitionPngImageHeader,
            &imageHeader,
            m_file.Data());

         rootNode->ChildNodes().push_back(pngImageHeaderNode);
      }

      chunkPtr += SwapEndianness(chunkHeader.length) + sizeof(PngChunkHeader) + 4;

      if (chunkType == _T("IEND"))
      {
         if (m_file.IsValidPointer(chunkPtr))
         {
            const BYTE* endPtr = m_file.Data<BYTE>(m_file.Size());

            summaryText.AppendFormat(
               _T("Warning: Garbage bytes at the end of the file (size: %08zx bytes)"),
               static_cast<size_t>(endPtr - chunkPtr));
         }

         break;
      }
   }

   rootNode->SetText(summaryText);

   m_rootNode = rootNode;
}

void PngImageReader::Cleanup()
{
   // nothing expensive to cleanup here
}
