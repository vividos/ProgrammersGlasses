@echo off
REM
REM Programmer's Glasses - a developer's file content viewer
REM Copyright (c) 2023-2026 Michael Fink
REM
REM Runs tests and collects coverage information
REM

echo RunTestsAndCollectCoverage.cmd - Runs tests and collects coverage
echo.

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles%\Microsoft Visual Studio\18\Community

REM
REM Preparations
REM
set PATH=%PATH%;%VSINSTALL%\Common7\IDE\Extensions\Microsoft\CodeCoverage.Console

REM install ReportGenerator, if not available yet
dotnet tool install dotnet-reportgenerator-globaltool --tool-path ..\intermediate\.dotnet-tools

set PATH=%PATH%;"%CD%\..\intermediate\.dotnet-tools"

rmdir ..\intermediate\CoverageReport /s /q 2> nul

REM
REM Run tests with files
REM
set FILES=test\lib-with-coff-obj.lib ^
    test\anon.obj ^
    test\bigobj.obj ^
    test\teststaticlib.lib ^
    test\teststaticlib.obj ^
    test\teststaticlib.pdb ^
    ..\intermediate\Release\ProgrammersGlasses\DataHelper.obj ^
    ..\intermediate\vcpkg_installed\x64-windows-static\lib\ulib.lib ^
    ProgrammersGlasses\userinterface\res\Ribbon\app_exit_16.png ^
    test\JCH-Blizzard.sid ^
    ..\bin\Release\ProgrammersGlasses.exe

echo Collecting code coverage...

Microsoft.CodeCoverage.Console.exe collect ^
   --settings CodeCoverage.runsettings ^
   --output ..\intermediate\CoverageReport-cobertura.xml ^
   ..\bin\Release\ProgrammersGlasses.exe ^
   --console ^
   %FILES%

echo Converting Cobertura to SonarQube xml...

echo Generating report...

ReportGenerator ^
    -reports:..\intermediate\CoverageReport-cobertura.xml ^
    -reporttypes:Html;SonarQube ^
    -filefilters:-*\vctools\* ^
    -targetdir:..\intermediate\CoverageReport
