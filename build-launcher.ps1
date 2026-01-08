#!/usr/bin/env powershell
<#
.SYNOPSIS
Build the Launcher executable

.EXAMPLE
.\build-launcher.ps1
.\build-launcher.ps1 -Config Release -Platform x86
#>

param(
  [string]$Config = "Release",
  [string]$Platform = "x86"
)

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
& "$scriptRoot\build-client.ps1" -Config $Config -Platform $Platform -Targets "Launcher"
