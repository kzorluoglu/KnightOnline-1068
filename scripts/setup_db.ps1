param(
  [string]$SaPassword = "YourStrong!Pass123",
  [string]$ContainerName = "ko-sql",
  [string]$SqlImage = "mcr.microsoft.com/mssql/server:2019-latest",
  [int]$Port = 1433,
  [string]$SqlLogin = "knight",
  [string]$SqlPassword = "knight",
  [string]$DriverName = "ODBC Driver 17 for SQL Server"
)

$ErrorActionPreference = "Stop"

function Require-Command {
  param([string]$Name)
  if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
    throw "Missing command: $Name"
  }
}

function Write-Info {
  param([string]$Message)
  Write-Host $Message
}

Require-Command "docker"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $repoRoot
Set-Location $repoRoot

Write-Info "==> Ensuring SQL Server container..."
$portOwner = docker ps --format "{{.Names}}|{{.Ports}}" | ForEach-Object {
  $parts = $_ -split '\|', 2
  if ($parts.Count -eq 2 -and $parts[1] -match ":$Port->") { $parts[0] }
} | Select-Object -First 1
$existing = docker ps -a --format "{{.Names}}" | Where-Object { $_ -eq $ContainerName }
if (-not $existing -and $portOwner -and $portOwner -ne $ContainerName) {
  throw "Port $Port is already in use by container '$portOwner'. Stop it or rerun with -Port to choose a different host port."
}
$existing = docker ps -a --format "{{.Names}}" | Where-Object { $_ -eq $ContainerName }
if (-not $existing) {
  docker run -e "ACCEPT_EULA=Y" -e "MSSQL_SA_PASSWORD=$SaPassword" `
    -p "$Port`:1433" --name $ContainerName -d $SqlImage | Out-Null
  if ($LASTEXITCODE -ne 0) {
    throw "Failed to create container $ContainerName. Resolve docker run errors above (port conflicts?) and retry."
  }
} else {
  $running = docker ps --format "{{.Names}}" | Where-Object { $_ -eq $ContainerName }
  if (-not $running) {
    docker start $ContainerName | Out-Null
    if ($LASTEXITCODE -ne 0) {
      throw "Failed to start container $ContainerName. Port $Port may be in use by another container."
    }
  }
}

Write-Info "==> Waiting for SQL Server to accept connections..."
$ready = $false
for ($i = 0; $i -lt 30; $i++) {
  try {
    docker exec $ContainerName /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P $SaPassword -Q "SELECT 1" | Out-Null
    $ready = $true
    break
  } catch {
    Start-Sleep -Seconds 2
  }
}
if (-not $ready) {
  throw "SQL Server did not become ready in time."
}

Write-Info "==> Creating databases and legacy login..."
$createSql = @"
IF DB_ID('KN_Online') IS NULL CREATE DATABASE KN_Online;
IF DB_ID('Knight_Account') IS NULL CREATE DATABASE Knight_Account;
IF DB_ID('Knight') IS NULL CREATE DATABASE Knight;
IF DB_ID('Repent') IS NULL CREATE DATABASE Repent;
IF NOT EXISTS (SELECT 1 FROM sys.sql_logins WHERE name = '$SqlLogin')
  CREATE LOGIN [$SqlLogin] WITH PASSWORD = '$SqlPassword', CHECK_POLICY = OFF;
EXEC sp_addsrvrolemember '$SqlLogin','sysadmin';
"@
docker exec $ContainerName /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P $SaPassword -Q $createSql | Out-Null

Write-Info "==> Importing SQL scripts..."
$tmpDir = Join-Path $env:TEMP "ko-sql"
New-Item -ItemType Directory -Force -Path $tmpDir | Out-Null

$knOnline = "KNIGHT ONLINE/sql/SQLScript/kn_online.sql"
$versionSql = "KNIGHT ONLINE/sql/SQLScript/version.sql"
$knAccount = "KNIGHT ONLINE/sql/SQLScript/knightaccount.sql"

if (-not (Test-Path $knOnline)) { throw "Missing: $knOnline" }
if (-not (Test-Path $versionSql)) { throw "Missing: $versionSql" }
if (-not (Test-Path $knAccount)) { throw "Missing: $knAccount" }

$knAccountOut = Join-Path $tmpDir "knightaccount_docker.sql"
$lines = Get-Content $knAccount
$marker = $lines | Where-Object { $_ -match '^use \[Knight_Account\]' } | Select-Object -First 1
$start = $lines.IndexOf($marker)
if ($start -lt 0) { throw "Could not find 'use [Knight_Account]' in knightaccount.sql" }
$lines[$start..($lines.Length - 1)] | Set-Content $knAccountOut -Encoding ASCII

docker cp $knOnline "$ContainerName`:/tmp/kn_online.sql" | Out-Null
docker exec $ContainerName /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P $SaPassword -d KN_Online -i /tmp/kn_online.sql | Out-Null

docker cp $knAccountOut "$ContainerName`:/tmp/knightaccount.sql" | Out-Null
docker exec $ContainerName /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P $SaPassword -d Knight_Account -i /tmp/knightaccount.sql | Out-Null

docker cp $versionSql "$ContainerName`:/tmp/version.sql" | Out-Null
docker exec $ContainerName /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P $SaPassword -d Knight -i /tmp/version.sql | Out-Null

Write-Info "==> Done."
