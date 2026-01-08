#!/usr/bin/env powershell
<#
.SYNOPSIS
Build the WarFare executable (KnightOnLine.exe)

.DESCRIPTION
Builds the WarFare client application which produces KnightOnLine.exe.
Note: The build may require a Visual Studio Developer Command Prompt for 
proper environment variable setup to build successfully.

.EXAMPLE
.\build-warfare.ps1
.\build-warfare.ps1 -Config Release -Platform x86
#>

param(
  [string]$Config = "Release",
  [string]$Platform = "x86"
)

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
& "$scriptRoot\build-client.ps1" -Config $Config -Platform $Platform -Targets "WarFare"
