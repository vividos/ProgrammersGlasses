@echo off
REM
REM Programmer's Glasses - a developer's file content viewer
REM Copyright (C) 2023-2026 Michael Fink
REM
REM Generates bitmap images from SVG sources
REM

REM set this to your ImageMagick magick.exe
set IMAGEMAGICK="C:\Projekte\Tools\ImageMagick-7.0.11-1-portable-Q16-x64\magick.exe"

REM the sizes to generate
set SIZES=16 20 24 32 40 48 64

echo Generating ribbon bitmap images for sizes: %SIZES%

for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none FolderOpened.svg ..\ribbon\file_open_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none CloseDocument.svg ..\ribbon\file_close_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none Copy.svg ..\ribbon\edit_copy_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none CloseSolution.svg ..\ribbon\app_exit_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none DialogGroup.svg ..\ribbon\window_cascade_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none SplitScreenHorizontally.svg ..\ribbon\window_horizontal_%%X.png
for %%X in (%SIZES%) do %IMAGEMAGICK% -size %%Xx%%X -background none SplitScreenVertically.svg ..\ribbon\window_vertical_%%X.png

echo Generating resource bitmap images

set TOOL=C:\Projekte\Tools\ConvertBitmap32Bit.exe
SET INTERMEDIATE=..\..\..\..\..\intermediate
%IMAGEMAGICK% -size 32x32 -background none Application_16x.svg %INTERMEDIATE%\module_app.png
%IMAGEMAGICK% -size 32x32 -background none Audio_16x.svg %INTERMEDIATE%\module_audio.png
%IMAGEMAGICK% -size 32x32 -background none DynamicLibrary_16x.svg %INTERMEDIATE%\module_dynamiclib.png
%IMAGEMAGICK% -size 32x32 -background none Image_16x.svg %INTERMEDIATE%\module_image.png
%IMAGEMAGICK% -size 32x32 -background none Library_16x.svg %INTERMEDIATE%\module_staticlib.png
%IMAGEMAGICK% -size 32x32 -background none PlayVideo_16x.svg %INTERMEDIATE%\module_video.png

%IMAGEMAGICK% -size 16x16 -background none Binary_16x.svg %INTERMEDIATE%\node_binary.png
%IMAGEMAGICK% -size 16x16 -background none Document_noHalo_16x.svg %INTERMEDIATE%\node_document.png
%IMAGEMAGICK% -size 16x16 -background none Item_16x.svg %INTERMEDIATE%\node_item.png
%IMAGEMAGICK% -size 16x16 -background none Library_16x.svg %INTERMEDIATE%\node_library.png
%IMAGEMAGICK% -size 16x16 -background none LegacyPackage_16x.svg %INTERMEDIATE%\node_object.png
%IMAGEMAGICK% -size 16x16 -background none Table_16x.svg %INTERMEDIATE%\node_table.png

set RESFILES=module_app module_audio module_dynamiclib module_image module_staticlib module_video ^
	node_binary node_document node_item node_library node_object node_table

for %%X in (%RESFILES%) do (
	echo converting %%X...
	 %TOOL% %INTERMEDIATE%\%%X.png ..\%%X.bmp
)

echo Finished generating bitmap images.

pause
