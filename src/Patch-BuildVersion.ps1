<#
	Programmer's Glasses - a developer's file content viewer
	Copyright (c) 2020 Michael Fink

.SYNOPSIS
	Writes a version number to the file version.hpp
#>

param (
	[Parameter(Mandatory=$true)][string]$version = "1.0.0.0"
)

$scriptPath = split-path -parent $MyInvocation.MyCommand.Definition

Write-Host "Patching build version $version..."

# split version number
$array = $version.Split(".")
$majorVersion = $array[0]
$minorVersion = $array[1]
$releaseNumber = $array[2]
$buildNumber = $array[3]
$buildYear = Get-Date -format yyyy

# modify version.hpp
$versionHeader = Get-Content "$scriptPath\version.hpp"

$versionHeader = $versionHeader -replace "MAJOR_VERSION [0-9]+","MAJOR_VERSION $majorVersion"
$versionHeader = $versionHeader -replace "MINOR_VERSION [0-9]+","MINOR_VERSION $minorVersion"
$versionHeader = $versionHeader -replace "RELEASE_NUMBER [0-9]+","RELEASE_NUMBER $releaseNumber"
$versionHeader = $versionHeader -replace "BUILD_NUMBER [0-9]+","BUILD_NUMBER $buildNumber"
$versionHeader = $versionHeader -replace "BUILD_YEAR [0-9]+","BUILD_YEAR $buildYear"

Out-File -FilePath "$scriptPath\version.hpp" -InputObject $versionHeader -Encoding ASCII

Write-Host "Done patching."
