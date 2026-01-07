param(
  [Alias("Target")][string[]]$Targets = @(),
  [string]$Config = "Release",
  [string]$Generator = "Visual Studio 15 2017",
  [string]$Arch = "Win32",
  [string]$BuildDir = "build",
  [string]$DistDir = "dist"
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

function Copy-ItemIfExists {
  param(
    [string]$Source,
    [string]$Destination
  )
  if (-not (Test-Path $Source)) { return $false }
  $srcResolved = (Resolve-Path $Source).ProviderPath
  $dstResolved = (Resolve-Path $Destination -ErrorAction SilentlyContinue)
  if ($dstResolved) {
    $dstResolved = $dstResolved.ProviderPath
    if ($srcResolved -ieq $dstResolved) { return $true }
  }
  $destParent = Split-Path $Destination -Parent
  if ($destParent -and -not (Test-Path $destParent)) {
    New-Item -ItemType Directory -Path $destParent -Force | Out-Null
  }
  Copy-Item $Source $Destination -Recurse -Force
  return $true
}

function Copy-DirIfExists {
  param(
    [string[]]$Sources,
    [string]$Destination
  )
  foreach ($src in $Sources) {
    if (Test-Path $src) {
      New-Item -ItemType Directory -Path $Destination -Force | Out-Null
      Copy-Item (Join-Path $src '*') $Destination -Recurse -Force
      return $true
    }
  }
  return $false
}

function Invoke-CMakeBuild {
  param(
    [string]$Gen,
    [string]$ArchArg,
    [string]$Cfg,
    [string[]]$Targets,
    [string]$Dir
  )

  if ($Gen -match "NMake|Ninja") {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    $vcvarsall = $null
    if (Test-Path $vswhere) {
      $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -version "[15.0,16.0)" -property installationPath
      if ($vsPath) {
        $candidate = Join-Path $vsPath "VC\Auxiliary\Build\vcvarsall.bat"
        if (Test-Path $candidate) { $vcvarsall = $candidate }
      }
    }
    if (-not $vcvarsall) {
      $fallbacks = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
      )
      foreach ($fallback in $fallbacks) {
        if (Test-Path $fallback) { $vcvarsall = $fallback; break }
      }
    }
    if (-not $vcvarsall) {
      throw "vcvarsall.bat not found. Install MSVC Build Tools or pass a Visual Studio generator."
    }

    $targetArg = ""
    if ($Targets -and $Targets.Count -gt 0) {
      $targetArg = " --target " + ($Targets -join " ")
    }

    $cmd = @(
      '"' + $vcvarsall + '"',
      "x86",
      "&&",
      "`"$script:CMakePath`" -S . -B `"$Dir`" -G `"$Gen`"",
      "&&",
      "`"$script:CMakePath`" --build `"$Dir`" --config $Cfg$targetArg"
    ) -join " "

    cmd /c $cmd
  } else {
    Invoke-CMake @("-S", ".", "-B", $Dir, "-G", $Gen, "-A", $ArchArg)
    $buildArgs = @("--build", $Dir, "--config", $Cfg)
    if ($Targets -and $Targets.Count -gt 0) {
      $buildArgs += "--target"
      $buildArgs += $Targets
    }
    Invoke-CMake $buildArgs
  }
}

$script:CMakePath = Find-CMake
if (-not $script:CMakePath) {
  throw "CMake not found. Install CMake or add it to PATH."
}

Invoke-CMakeBuild -Gen $Generator -ArchArg $Arch -Cfg $Config -Targets $Targets -Dir $BuildDir

$distRoot = Join-Path $repoRoot $DistDir
$distBin = Join-Path $distRoot "bin"
New-Item -ItemType Directory -Path $distBin -Force | Out-Null

$packageSpecs = @(
  @{
    Name = "Aujard"
    ExeRel = "KNIGHT ONLINE/Aujard/$Config/Aujard.exe"
    Configs = @("KNIGHT ONLINE/Aujard/Aujard.ini", "dist/bin/Aujard/Aujard.ini")
    DataDirs = @()
  },
  @{
    Name = "Ebenezer"
    ExeRel = "KNIGHT ONLINE/Ebenezer/$Config/Ebenezer.exe"
    Configs = @("KNIGHT ONLINE/Ebenezer/Server.ini")
    DataDirs = @(@{From=@("KNIGHT ONLINE/Ebenezer/map", "dist/bin/Ebenezer/MAP"); To="MAP"})
  },
  @{
    Name = "GameServer"
    ExeRel = "KNIGHT ONLINE/GameServer/$Config/GameServer.exe"
    Configs = @("KNIGHT ONLINE/GameServer/server.ini")
    DataDirs = @(
      @{From=@("KNIGHT ONLINE/GameServer/map", "dist/bin/GameServer/MAP"); To="MAP"},
      @{From=@("KNIGHT ONLINE/GameServer/quests", "dist/bin/GameServer/quests"); To="quests"}
    )
  },
  @{
    Name = "VersionManager"
    ExeRel = "KNIGHT ONLINE/LogInServer/$Config/VersionManager.exe"
    Configs = @("KNIGHT ONLINE/LogInServer/Version.ini")
    DataDirs = @()
  }
)

foreach ($spec in $packageSpecs) {
  $targetDir = Join-Path $distBin $spec.Name
  New-Item -ItemType Directory -Path (Join-Path $targetDir "logs") -Force | Out-Null

  $exeDest = Join-Path $targetDir ("{0}.exe" -f $spec.Name)
  if (-not (Test-Path $exeDest)) {
    $exePath = Join-Path $repoRoot (Join-Path $BuildDir $spec.ExeRel)
    if (Test-Path $exePath) {
      Copy-Item $exePath $exeDest -Force
    } else {
      Write-Warning "Executable not found: $exePath (did the build succeed for $($spec.Name)?)"
    }
  }

  foreach ($cfg in $spec.Configs) {
    $cfgPath = Join-Path $repoRoot $cfg
    if (Copy-ItemIfExists -Source $cfgPath -Destination (Join-Path $targetDir (Split-Path $cfgPath -Leaf))) {
      break
    }
  }

  foreach ($data in $spec.DataDirs) {
    $destDir = Join-Path $targetDir $data.To
    Copy-DirIfExists -Sources ($data.From | ForEach-Object { Join-Path $repoRoot $_ }) -Destination $destDir | Out-Null
  }
}
