param(
  [string]$Config = "Release",
  [string]$Platform = "x86",
  [string[]]$Targets = @()
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $repoRoot

# Kill any remaining devenv.com processes to prevent file locking
$devenvProcesses = Get-Process devenv -ErrorAction SilentlyContinue
if ($devenvProcesses) {
  Write-Host "Stopping background Visual Studio processes..."
  $devenvProcesses | Stop-Process -Force -ErrorAction SilentlyContinue
  Start-Sleep -Milliseconds 500
}

function Find-Devenv2017 {
  $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
  if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -version "[15.0,16.0)" -property installationPath
    if ($vsPath) {
      $candidate = Join-Path $vsPath "Common7\IDE\devenv.com"
      if (Test-Path $candidate) { return $candidate }
    }
  }

  $fallbacks = @(
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.com",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.com",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\devenv.com"
  )
  foreach ($fallback in $fallbacks) {
    if (Test-Path $fallback) { return $fallback }
  }

  throw "devenv.com for Visual Studio 2017 not found. Install VS 2017 with C++ workload (MFC) and try again."
}

$devenv = Find-Devenv2017

$dx9Root = Join-Path $repoRoot "third_party/dx9sdk"
if (Test-Path $dx9Root) {
  $env:INCLUDE = (Join-Path $dx9Root "Include") + ";" + $env:INCLUDE
  $env:LIB = (Join-Path $dx9Root "Lib") + ";" + $env:LIB
} else {
  Write-Warning "DX9 SDK path not found at $dx9Root. Build will likely fail unless DirectX 9 SDK is installed and on INCLUDE/LIB."
}

function Fix-LauncherVcxproj {
  param(
    [string]$VcxprojPath
  )

  # Fix Launcher project to include necessary libraries and link flags
  if ($VcxprojPath -like "*Launcher.vcxproj*") {
    # Wait a moment to ensure the file is not locked
    Start-Sleep -Milliseconds 200
    
    try {
      [xml]$vcxproj = Get-Content $VcxprojPath
      $ns = New-Object System.Xml.XmlNamespaceManager($vcxproj.NameTable)
      $ns.AddNamespace("ns", "http://schemas.microsoft.com/developer/msbuild/2003")

      $modified = $false
      # Find the Release|Win32 Link section
      $linkSections = $vcxproj.SelectNodes("//ns:ItemDefinitionGroup[contains(@Condition, 'Release')]//ns:Link", $ns)
      
      foreach ($linkSection in $linkSections) {
        # Check if dependencies are already set
        $depNode = $linkSection.SelectSingleNode("ns:AdditionalDependencies", $ns)
        if (-not $depNode -or -not $depNode.InnerText.Contains("wininet.lib")) {
          if (-not $depNode) {
            $depNode = $vcxproj.CreateElement("AdditionalDependencies", "http://schemas.microsoft.com/developer/msbuild/2003")
            $linkSection.AppendChild($depNode) | Out-Null
          }
          $depNode.InnerText = "wininet.lib;ZipArchive.lib;%(AdditionalDependencies)"
          $modified = $true
        }

        # Add library directory
        $libDirNode = $linkSection.SelectSingleNode("ns:AdditionalLibraryDirectories", $ns)
        if (-not $libDirNode) {
          $libDirNode = $vcxproj.CreateElement("AdditionalLibraryDirectories", "http://schemas.microsoft.com/developer/msbuild/2003")
          $linkSection.AppendChild($libDirNode) | Out-Null
        }
        if (-not $libDirNode.InnerText.Contains("../ZipArchive")) {
          $libDirNode.InnerText = "../ZipArchive;%(AdditionalLibraryDirectories)"
          $modified = $true
        }

        # Add SAFESEH:NO flag
        $optsNode = $linkSection.SelectSingleNode("ns:AdditionalOptions", $ns)
        if (-not $optsNode) {
          $optsNode = $vcxproj.CreateElement("AdditionalOptions", "http://schemas.microsoft.com/developer/msbuild/2003")
          $linkSection.AppendChild($optsNode) | Out-Null
        }
        if (-not $optsNode.InnerText.Contains("/SAFESEH:NO")) {
          $optsNode.InnerText = "/SAFESEH:NO %(AdditionalOptions)"
          $modified = $true
        }

        # Add IgnoreSpecificDefaultLibraries
        $ignoreNode = $linkSection.SelectSingleNode("ns:IgnoreSpecificDefaultLibraries", $ns)
        if (-not $ignoreNode) {
          $ignoreNode = $vcxproj.CreateElement("IgnoreSpecificDefaultLibraries", "http://schemas.microsoft.com/developer/msbuild/2003")
          $linkSection.AppendChild($ignoreNode) | Out-Null
        }
        if (-not $ignoreNode.InnerText.Contains("MSVCRT.lib")) {
          $ignoreNode.InnerText = "MSVCRT.lib;%(IgnoreSpecificDefaultLibraries)"
          $modified = $true
        }
      }

      if ($modified) {
        $vcxproj.Save($VcxprojPath)
      }
    } catch {
      Write-Warning "Could not fix Launcher.vcxproj (file may be locked): $_"
    }
  }
}

function Fix-WarFareVcxproj {
  param(
    [string]$VcxprojPath,
    [string]$ProjectDir
  )

  # Fix WarFare project to include necessary include and library paths
  if ($VcxprojPath -like "*WarFare.vcxproj*") {
    Start-Sleep -Milliseconds 200
    
    try {
      [xml]$vcxproj = Get-Content $VcxprojPath
      $ns = New-Object System.Xml.XmlNamespaceManager($vcxproj.NameTable)
      $ns.AddNamespace("ns", "http://schemas.microsoft.com/developer/msbuild/2003")

      $modified = $false
      
      # Try to set PlatformToolset to v100 (VS2010) which has better legacy support
      $propGroups = $vcxproj.SelectNodes("//ns:PropertyGroup[@Label='Configuration']", $ns)
      foreach ($propGroup in $propGroups) {
        $toolsetNode = $propGroup.SelectSingleNode("ns:PlatformToolset", $ns)
        if ($toolsetNode) {
          if ($toolsetNode.InnerText -ne "v140") {
            $toolsetNode.InnerText = "v140"
            $modified = $true
          }
        }
      }
      
      $compileNodes = $vcxproj.SelectNodes("//ns:ItemDefinitionGroup//ns:ClCompile", $ns)
      
      foreach ($compileNode in $compileNodes) {
        # Add include directories
        $incNode = $compileNode.SelectSingleNode("ns:AdditionalIncludeDirectories", $ns)
        if (-not $incNode) {
          $incNode = $vcxproj.CreateElement("AdditionalIncludeDirectories", "http://schemas.microsoft.com/developer/msbuild/2003")
          $compileNode.AppendChild($incNode) | Out-Null
        }
        
        # Use SDK 10 with explicit UCRT path first
        $neededIncludes = @(
          "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt",
          "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um",
          "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared",
          "$ProjectDir",
          "$ProjectDir\..\N3BASE",
          "$ProjectDir\..\JPEG",
          "$repoRoot\third_party\dx8sdk\DXF\include",
          "$repoRoot\third_party\dx9sdk\Include"
        )
        
        $newIncludePath = ($neededIncludes | Where-Object { $_ -and (Test-Path $_) }) -join ";"
        $newIncludePath += ";%(AdditionalIncludeDirectories)"
        
        if ($incNode.InnerText -ne $newIncludePath) {
          $incNode.InnerText = $newIncludePath
          $modified = $true
        }
        
        # Add preprocessor defines - keep existing defines by prepending new ones
        $defNode = $compileNode.SelectSingleNode("ns:PreprocessorDefinitions", $ns)
        if (-not $defNode) {
          $defNode = $vcxproj.CreateElement("PreprocessorDefinitions", "http://schemas.microsoft.com/developer/msbuild/2003")
          $compileNode.AppendChild($defNode) | Out-Null
        }
        
        $requiredDefs = @("_WIN32_WINNT=0x0601", "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN", "STRICT", "_N3GAME")
        $currentDefs = $defNode.InnerText
        $needsUpdate = $false
        foreach ($def in $requiredDefs) {
          if ($currentDefs -notlike "*$def*") {
            $needsUpdate = $true
            break
          }
        }
        if ($needsUpdate) {
          # Prepend required defines to existing defines (preserving original %(PreprocessorDefinitions) reference)
          $defNode.InnerText = ($requiredDefs -join ";") + ";" + $defNode.InnerText
          $modified = $true
        }
        
        # Disable standard conformance to allow old code patterns
        $confNode = $compileNode.SelectSingleNode("ns:ConformanceMode", $ns)
        if (-not $confNode) {
          $confNode = $vcxproj.CreateElement("ConformanceMode", "http://schemas.microsoft.com/developer/msbuild/2003")
          $compileNode.AppendChild($confNode) | Out-Null
        }
        if ($confNode.InnerText -ne "false") {
          $confNode.InnerText = "false"
          $modified = $true
        }
        
        # Add compiler flags for legacy code compatibility
        $addOptNode = $compileNode.SelectSingleNode("ns:AdditionalOptions", $ns)
        if (-not $addOptNode) {
          $addOptNode = $vcxproj.CreateElement("AdditionalOptions", "http://schemas.microsoft.com/developer/msbuild/2003")
          $compileNode.AppendChild($addOptNode) | Out-Null
        }
        # /FS = shared PDB writes, /Zc:forScope- = disable for-loop scope conformance (fixes C2065)
        # /permissive = more permissive parsing for legacy code
        $reqFlags = "/FS /Zc:forScope- /permissive"
        $currentOpts = $addOptNode.InnerText
        if ($currentOpts -notlike "**/FS*") {
          $addOptNode.InnerText = $reqFlags + " " + $currentOpts
          $modified = $true
        }
      }

      # Force dynamic CRT (/MD)
      $clNodes = $vcxproj.SelectNodes("//ns:ItemDefinitionGroup//ns:ClCompile", $ns)
      foreach ($clNode in $clNodes) {
        $rtLibNode = $clNode.SelectSingleNode("ns:RuntimeLibrary", $ns)
        if (-not $rtLibNode) {
          $rtLibNode = $vcxproj.CreateElement("RuntimeLibrary", "http://schemas.microsoft.com/developer/msbuild/2003")
          $clNode.AppendChild($rtLibNode) | Out-Null
        }
        if ($rtLibNode.InnerText -ne "MultiThreadedDLL") {
          $rtLibNode.InnerText = "MultiThreadedDLL"
          $modified = $true
        }
      }

      # Add linker library directories
      $linkNodes = $vcxproj.SelectNodes("//ns:ItemDefinitionGroup//ns:Link", $ns)
      foreach ($linkNode in $linkNodes) {
        # Add legacy_stdio_definitions.lib for old __snprintf symbols and DX9 libs
        $addDepNode = $linkNode.SelectSingleNode("ns:AdditionalDependencies", $ns)
        if ($addDepNode) {
          $currentDeps = $addDepNode.InnerText
          $newDeps = ""
          if ($currentDeps -notlike "*legacy_stdio_definitions*") { $newDeps += "legacy_stdio_definitions.lib;" }
          if ($currentDeps -notlike "*d3d9.lib*") { $newDeps += "d3d9.lib;" }
          if ($currentDeps -notlike "*d3dx9.lib*") { $newDeps += "d3dx9.lib;" }
          if ($currentDeps -notlike "*DxErr.lib*") { $newDeps += "DxErr.lib;" }
          
          if ($newDeps) {
            $addDepNode.InnerText = $newDeps + $currentDeps
            $modified = $true
          }
        }

        $libDirNode = $linkNode.SelectSingleNode("ns:AdditionalLibraryDirectories", $ns)
        if (-not $libDirNode) {
          $libDirNode = $vcxproj.CreateElement("AdditionalLibraryDirectories", "http://schemas.microsoft.com/developer/msbuild/2003")
          $linkNode.AppendChild($libDirNode) | Out-Null
        }
        $dx8LibPath = "$repoRoot\third_party\dx8sdk\DXF\lib"
        $dx9LibPath = "$repoRoot\third_party\dx9sdk\Lib\x86"
        $ucrtLibPath = "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x86"
        $umLibPath = "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\um\x86"
        if ($libDirNode.InnerText -notlike "*dx9sdk*") {
          $libDirNode.InnerText = "$dx9LibPath;$dx8LibPath;$ucrtLibPath;$umLibPath;%(AdditionalLibraryDirectories)"
          $modified = $true
        }

        # Disable SAFESEH for legacy libraries
        $safeSehNode = $linkNode.SelectSingleNode("ns:ImageHasSafeExceptionHandlers", $ns)
        if (-not $safeSehNode) {
          $safeSehNode = $vcxproj.CreateElement("ImageHasSafeExceptionHandlers", "http://schemas.microsoft.com/developer/msbuild/2003")
          $linkNode.AppendChild($safeSehNode) | Out-Null
        }
        if ($safeSehNode.InnerText -ne "false") {
          $safeSehNode.InnerText = "false"
          $modified = $true
        }

        # Remove deprecated LIBC.lib
        $ignoreLibNode = $linkNode.SelectSingleNode("ns:IgnoreSpecificDefaultLibraries", $ns)
        if (-not $ignoreLibNode) {
          $ignoreLibNode = $vcxproj.CreateElement("IgnoreSpecificDefaultLibraries", "http://schemas.microsoft.com/developer/msbuild/2003")
          $linkNode.AppendChild($ignoreLibNode) | Out-Null
        }
        if ($ignoreLibNode.InnerText -notlike "*LIBC*") {
          $ignoreLibNode.InnerText = "LIBC.lib;%(IgnoreSpecificDefaultLibraries)"
          $modified = $true
        }
      }

      if ($modified) {
        $vcxproj.Save($VcxprojPath)
      }
    } catch {
      Write-Warning "Could not fix WarFare.vcxproj (file may be locked): $_"
    }
  }
}

function Build-Dsw {
  param(
    [string]$Name,
    [string]$DswRelPath,
    [string]$Platform
  )

  $dswPath = Join-Path $repoRoot $DswRelPath
  if (-not (Test-Path $dswPath)) {
    throw "DSW not found: $dswPath"
  }

  $solutionDir = Split-Path $dswPath -Parent
  Push-Location $solutionDir
  try {
    # For Launcher, preserve the vcxproj before upgrade
    $vcxprojPath = [System.IO.Path]::ChangeExtension($dswPath, ".vcxproj")
    $vcxprojBackup = "$vcxprojPath.fixed"
    if ($Name -eq "Launcher" -and (Test-Path $vcxprojPath)) {
      Copy-Item -Path $vcxprojPath -Destination $vcxprojBackup -Force
    }

    # Delete .vcxproj and related files before upgrade to prevent overwrite dialogs
    Get-ChildItem -Path $solutionDir -Filter "*.vcxproj*" -ErrorAction SilentlyContinue | Remove-Item -Force -ErrorAction SilentlyContinue
    $sln = [System.IO.Path]::ChangeExtension($dswPath, ".sln")
    Remove-Item -Path $sln -Force -ErrorAction SilentlyContinue
    Start-Sleep -Milliseconds 200

    Write-Host "Upgrading $Name workspace..."
    & $devenv $dswPath /upgrade | Out-Null

    $slnPath = [System.IO.Path]::ChangeExtension($dswPath, ".sln")
    if (-not (Test-Path $slnPath)) {
      throw "Upgrade did not produce a solution file at $slnPath"
    }

    # Restore Launcher vcxproj if it was modified
    if ($Name -eq "Launcher" -and (Test-Path $vcxprojBackup)) {
      Copy-Item -Path $vcxprojBackup -Destination $vcxprojPath -Force
    }

    # Fix Launcher vcxproj if needed
    if (Test-Path $vcxprojPath) {
      if ($Name -eq "Launcher") {
        Fix-LauncherVcxproj -VcxprojPath $vcxprojPath
      } elseif ($Name -eq "WarFare") {
        Fix-WarFareVcxproj -VcxprojPath $vcxprojPath -ProjectDir $solutionDir
      }
    }

    $cfgs = @("$Config|$Platform")
    if ($Platform -ieq "Win32") { $cfgs += "$Config|x86" }
    if ($Platform -ieq "x86") { $cfgs += "$Config|Win32" }

    $built = $false
    foreach ($cfg in $cfgs | Select-Object -Unique) {
    Write-Host "Building $Name ($cfg)..."
    
    # Use MSBuild for WarFare to avoid PDB locking issues with parallel compilation
    if ($Name -eq "WarFare") {
      $msbuild = "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\MSBuild.exe"
      if (Test-Path $msbuild) {
        # For WarFare, use x86 platform not Win32
        $cfgName = $cfg.Split('|')[0]
        $platName = if ($cfg.Split('|')[1] -eq "Win32") { "x86" } else { $cfg.Split('|')[1] }
        & $msbuild $slnPath /p:Configuration=$cfgName /p:Platform=$platName /m:1 /p:PreferredToolArchitecture=x64
      } else {
        & $devenv $slnPath /Build $cfg
      }
    } else {
      & $devenv $slnPath /Build $cfg
    }
    
    if ($LASTEXITCODE -eq 0) { 
      $built = $true
      break 
    }
    Write-Warning "Build with configuration $cfg failed (will try next option if available)."
  }
    if (-not $built) {
      throw "Failed to build $Name. Tried configs: $($cfgs -join ', ')"
    }
  } finally {
    Pop-Location
  }
}

if (-not $Targets -or $Targets.Count -eq 0) {
  throw "No client targets specified."
}

$dswSpecs = @{
  Launcher = @{
    Dsw = "Client Source Code/Launcher/Launcher/Launcher.dsw"
    Output = "1097 Client/Launcher.exe"
  }
  WarFare = @{
    Dsw = "Client Source Code/WarFare/WarFare.dsw"
    Output = "1097 Client/KnightOnLine.exe"
  }
  Option = @{
    Dsw = "Client Source Code/Option/Option.dsw"
    Output = "1097 Client/Option.exe"
  }
}

$targetsToBuild = $Targets |
  Where-Object { $_ -and $_.Trim().Length -gt 0 } |
  ForEach-Object { $_.Trim() } |
  Select-Object -Unique

# If no targets specified, show usage and examples
if (-not $targetsToBuild -or $targetsToBuild.Count -eq 0) {
  Write-Host "No targets specified. Usage:"
  Write-Host ""
  Write-Host "  .\build-client.ps1 -Targets Launcher"
  Write-Host "  .\build-client.ps1 -Targets WarFare"
  Write-Host "  .\build-client.ps1 -Targets Launcher,WarFare"
  Write-Host ""
  Write-Host "Available targets: $($dswSpecs.Keys -join ', ')"
  exit 0
}

# Build ZipArchive dependency for Launcher if needed
if ($targetsToBuild -contains "Launcher") {
  Write-Host "Building ZipArchive dependency..."
  $zipArchiveDir = Join-Path $repoRoot "Client Source Code/Launcher/ZipArchive"
  $zipArchiveDsw = Join-Path $zipArchiveDir "ZipArchive.dsw"
  if (Test-Path $zipArchiveDsw) {
    Push-Location $zipArchiveDir
    try {
      # Clean up old project files to prevent overwrite dialog
      Get-ChildItem -Path $zipArchiveDir -Filter "*.vcxproj*" -ErrorAction SilentlyContinue | Remove-Item -Force -ErrorAction SilentlyContinue
      $zipArchiveSln = [System.IO.Path]::ChangeExtension($zipArchiveDsw, ".sln")
      Remove-Item -Path $zipArchiveSln -Force -ErrorAction SilentlyContinue
      Start-Sleep -Milliseconds 200
      
      & $devenv $zipArchiveDsw /upgrade | Out-Null
      $cfgStr = if ($Config -ieq "Release") { "Static Release|x86" } else { "Debug|x86" }
      Write-Host "Building ZipArchive ($cfgStr)..."
      & $devenv $zipArchiveSln /Build $cfgStr 2>&1 | Out-Null
      
      # Copy the lib to the root directory for linking
      $sourceLib = Join-Path $zipArchiveDir "$(if ($Config -ieq 'Release') { 'Static_Release' } else { 'Debug' })\ZipArchive.lib"
      $targetLib = Join-Path $zipArchiveDir "ZipArchive.lib"
      if (Test-Path $sourceLib) {
        Copy-Item -Path $sourceLib -Destination $targetLib -Force
      }
    } finally {
      Pop-Location
    }
  }
}

foreach ($target in $targetsToBuild) {
  if (-not $dswSpecs.ContainsKey($target)) {
    Write-Warning "Skipping unknown client target '$target'. Supported targets: $($dswSpecs.Keys -join ', ')."
    continue
  }

  $spec = $dswSpecs[$target]
  Build-Dsw -Name $target -DswRelPath $spec.Dsw -Platform $Platform
}

function Copy-Executable {
  param(
    [string]$Label,
    [string[]]$SourcePaths,
    [string]$DestinationPath
  )

  foreach ($src in $SourcePaths) {
    $fullSrc = Join-Path $repoRoot $src
    if (Test-Path $fullSrc) {
      $destFull = Join-Path $repoRoot $DestinationPath
      $destDir = Split-Path $destFull -Parent
      if (-not (Test-Path $destDir)) {
        New-Item -ItemType Directory -Path $destDir | Out-Null
      }
      Copy-Item -Path $fullSrc -Destination $destFull -Force
      Write-Host ("   Copied {0} to {1}" -f $Label, $DestinationPath)
      return
    }
  }

  Write-Warning ("Could not find built {0}. Tried: {1}" -f $Label, ($SourcePaths -join ", "))
}

function Copy-BuiltOutputs {
  param(
    [string]$Target
  )

  switch ($Target.ToLower()) {
    "warfare" {
      Copy-Executable -Label "KnightOnLine.exe" -SourcePaths @(
        "Client Source Code/WarFare/Release/KnightOnLine.exe",
        "Client Source Code/WarFare/Release/WarFare.exe",
        "Client Source Code/WarFare/KnightOnLine.exe"
      ) -DestinationPath $dswSpecs["WarFare"].Output
    }
    "launcher" {
      Copy-Executable -Label "Launcher.exe" -SourcePaths @(
        "Client Source Code/Launcher/Launcher/Release/Launcher.exe",
        "Client Source Code/Launcher/Release/Launcher.exe"
      ) -DestinationPath $dswSpecs["Launcher"].Output
    }
    "option" {
      Copy-Executable -Label "Option.exe" -SourcePaths @(
        "Client Source Code/Option/Release/Option.exe"
      ) -DestinationPath $dswSpecs["Option"].Output
    }
  }
}

function Copy-GameData {
  param(
    [string]$ExecutableDir
  )
  
  $dataSource = Join-Path $repoRoot "KnightOnline_cn_1097\Data"
  $dataDest = Join-Path $ExecutableDir "Data"
  
  if (Test-Path $dataSource) {
    if (Test-Path $dataDest) {
      Write-Host "   Data folder already exists, skipping copy"
    } else {
      Write-Host "   Copying game data files to output directory..."
      Copy-Item -Path $dataSource -Destination $dataDest -Recurse -Force
      Write-Host "   Game data files copied successfully"
    }
  } else {
    Write-Warning "Game data source not found at $dataSource"
  }
}

Write-Host ""
Write-Host "Build outputs:"
foreach ($target in $targetsToBuild) {
  if ($dswSpecs.ContainsKey($target)) {
    $label = if ($target -ieq "WarFare") { "KnightOnline (WarFare)" } else { $target }
    $outputPath = $dswSpecs[$target].Output
    Write-Host ("  {0}: {1}" -f $label, $outputPath)
    Copy-BuiltOutputs -Target $target
    
    # Copy game data for WarFare client
    if ($target -ieq "WarFare") {
      $execDir = Split-Path -Parent $outputPath
      Copy-GameData -ExecutableDir $execDir
    }
  }
}
