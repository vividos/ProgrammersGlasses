@echo off
REM
REM Programmer's Glasses - a developer's file content viewer
REM Copyright (C) 2023 Michael Fink
REM
REM Generates bitmap images from SVG sources
REM

REM set this to your ImageMagick magick.exe
set IMAGEMAGICK="C:\Projekte\Tools\ImageMagick-7.0.11-1-portable-Q16-x64\magick.exe"

REM the sizes to generate
set SIZES=16 20 24 32 40 48 64

echo Generating bitmap images for sizes: %SIZES%

for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none Copy_16x.svg ..\ribbon\edit_copy_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none CloseDocument_16x.svg ..\ribbon\file_close_%%X.png

echo Finished generating bitmap images.