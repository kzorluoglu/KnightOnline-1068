param(
  [string]$Target = "GameServer",
  [string]$Config = "Release",
  [string]$Generator = "Visual Studio 17 2022",
  [string]$Arch = "Win32",
  [string]$BuildDir = "build"
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $repoRoot

function Find-CMake {
  $cmd = Get-Command cmake -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Path }

  $candidates = @(
    "$env:ProgramFiles\CMake\bin\cmake.exe",
    "$env:ProgramFiles(x86)\CMake\bin\cmake.exe"
  )

  foreach ($path in $candidates) {
    if (Test-Path $path) { return $path }
  }

  $jetbrainsRoots = @(
    "$env:ProgramFiles\JetBrains",
    "$env:ProgramFiles(x86)\JetBrains"
  )

  foreach ($root in $jetbrainsRoots) {
    if (Test-Path $root) {
      $cmakeExe = Get-ChildItem -Path $root -Filter cmake.exe -Recurse -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match "CLion" -and $_.FullName -match "cmake\\win\\x64\\bin" } |
        Select-Object -First 1
      if ($cmakeExe) { return $cmakeExe.FullName }
    }
  }

  return $null
}

function Invoke-CMake {
  param([string[]]$CMakeArgs)
  if (-not $script:CMakePath) { throw "CMake not found. Install CMake or add it to PATH." }
  & $script:CMakePath @CMakeArgs
}

function Invoke-CMakeBuild {
  param(
    [string]$Gen,
    [string]$ArchArg,
    [string]$Cfg,
    [string]$TargetName,
    [string]$Dir
  )

  if ($Gen -match "NMake|Ninja") {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    $vcvarsall = $null
    if (Test-Path $vswhere) {
      $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
      if ($vsPath) {
        $candidate = Join-Path $vsPath "VC\Auxiliary\Build\vcvarsall.bat"
        if (Test-Path $candidate) { $vcvarsall = $candidate }
      }
    }
    if (-not $vcvarsall) {
      $fallback = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
      if (Test-Path $fallback) { $vcvarsall = $fallback }
    }
    if (-not $vcvarsall) {
      throw "vcvarsall.bat not found. Install MSVC Build Tools or pass a Visual Studio generator."
    }

    $cmd = @(
      '"' + $vcvarsall + '"',
      "x86",
      "&&",
      "`"$script:CMakePath`" -S . -B `"$Dir`" -G `"$Gen`"",
      "&&",
      "`"$script:CMakePath`" --build `"$Dir`" --config $Cfg --target $TargetName"
    ) -join " "

    cmd /c $cmd
  } else {
    Invoke-CMake @("-S", ".", "-B", $Dir, "-G", $Gen, "-A", $ArchArg)
    Invoke-CMake @("--build", $Dir, "--config", $Cfg, "--target", $TargetName)
  }
}

$script:CMakePath = Find-CMake
if (-not $script:CMakePath) {
  throw "CMake not found. Install CMake or add it to PATH."
}

Invoke-CMakeBuild -Gen $Generator -ArchArg $Arch -Cfg $Config -TargetName $Target -Dir $BuildDir
