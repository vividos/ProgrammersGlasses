@echo off
REM
REM Programmer's Glasses - a developer's file content viewer
REM Copyright (c) 2023 Michael Fink
REM
REM Runs tests and collects coverage information
REM

REM set this to your OpenCppCoverage folder
set OPENCPPCOVERAGE=C:\Projekte\Tools\OpenCppCoverage\

REM
REM Preparations
REM
set PATH=%PATH%;%OPENCPPCOVERAGE%

REM
REM Run unit tests
REM
set FILES=test\lib-with-coff-obj.lib ^
    ..\bin\Release\ProgrammersGlasses.exe

OpenCppCoverage.exe ^
   --continue_after_cpp_exception ^
   --cover_children ^
   --sources ProgrammersGlasses ^
   --excluded_sources ProgrammersGlasses\thirdparty ^
   --excluded_sources ProgrammersGlasses\userinterface ^
   --excluded_sources intermediate\vcpkg_installed ^
   --export_type SonarQube:CoverageReport-SonarQube.xml ^
   --export_type html:CoverageReport ^
   --modules ProgrammersGlasses.exe ^
   -- ..\bin\Release\ProgrammersGlasses.exe ^
   %FILES%
