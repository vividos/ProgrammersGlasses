@echo off
REM
REM Programmer's Glasses - a developer's file content viewer
REM Copyright (C) 2022-2026 Michael Fink
REM
REM Runs SonarCloud analysis build
REM

echo SonarCloud.cmd - Runs SonarCloud analysis build
echo.

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles%\Microsoft Visual Studio\18\Community

REM set this to your SonarQube tools folder
set SONARQUBE=C:\Projekte\Tools\SonarQube

REM
REM Preparations
REM
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

set DOTNET_CLI_TELEMETRY_OPTOUT=1

REM install SonarScanner, if not available yet
dotnet tool install dotnet-sonarscanner --tool-path .dotnet-tools

set PATH=%PATH%;"%CD%\.dotnet-tools"

REM check for SONARLOGIN env var
if "%SONARLOGIN%" == "" echo "Environment variable SONARLOGIN is not set! Obtain a new token and set the environment variable!"
if "%SONARLOGIN%" == "" exit 1

set PATH=%PATH%;%SONARQUBE%\build-wrapper-win-x86;%OPENCPPCOVERAGE%

REM
REM Build using SonarQube scanner for MSBuild
REM
rmdir .\.sonarqube /s /q 2> nul
rmdir .\.bw-output /s /q 2> nul
mkdir .\.sonar-cache 2> nul

dotnet-sonarscanner begin ^
    /k:"ProgrammersGlasses" ^
    /v:"0.1.0" ^
    /d:"sonar.cfamily.build-wrapper-output=%CD%\.bw-output" ^
    /d:"sonar.host.url=https://sonarcloud.io" ^
    /d:"sonar.cfamily.threads=4" ^
    /d:"sonar.cfamily.analysisCache.mode=fs " ^
    /d:"sonar.cfamily.analysisCache.path=.sonar-cache" ^
    /d:"sonar.coverageReportPaths=%CD%\CoverageReport-SonarQube.xml" ^
    /o:"vividos-github" ^
    /d:"sonar.token=%SONARLOGIN%"
if errorlevel 1 goto end

REM
REM Rebuild Release|x64
REM
build-wrapper-win-x86-64.exe ^
   --out-dir .bw-output ^
   msbuild ProgrammersGlasses.slnx /m /property:Configuration=Release,Platform=x64 /target:Rebuild

REM
REM Run tests
REM
call RunTestsAndCollectCoverage.cmd

dotnet-sonarscanner end /d:"sonar.token=%SONARLOGIN%"

:end

pause
