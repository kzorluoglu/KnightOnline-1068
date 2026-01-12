# Build script for UIE editor
# Similar to build-warfare.ps1

param(
    [string]$Configuration = "Release",
    [string]$Platform = "Win32"
)

$ErrorActionPreference = "Stop"

Write-Host "Building UIE editor..." -ForegroundColor Green

# Change to UIE directory
Push-Location "Client Source Code\UIE"

try {
    # Build with MSBuild
    $msbuildPath = "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe"
    if (!(Test-Path $msbuildPath)) {
        throw "MSBuild not found at $msbuildPath"
    }

    & $msbuildPath UIE.vcxproj /p:Configuration=$Configuration /p:Platform=$Platform /verbosity:minimal

    if ($LASTEXITCODE -ne 0) {
        throw "MSBuild failed with exit code $LASTEXITCODE"
    }

    Write-Host "UIE build successful." -ForegroundColor Green

    # Copy to client folder
    $sourceExe = ".\Release\UIE.exe"
    $destFolder = "..\..\1097 Client\"

    if (Test-Path $sourceExe) {
        Copy-Item $sourceExe $destFolder -Force
        Write-Host "Copied UIE.exe to $destFolder" -ForegroundColor Green
    } else {
        Write-Warning "UIE.exe not found at $sourceExe"
    }

} finally {
    Pop-Location
}