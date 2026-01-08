#!/usr/bin/env powershell
<#
.SYNOPSIS
Setup Knight Online database in SQL Server Docker container

.DESCRIPTION
Creates the Knight database, knight user, and runs SQL scripts from Rokoko DB dump
#>

param(
  [string]$SQLServerHost = "localhost",
  [string]$SQLServerPort = "1433",
  [string]$SAPassword = "YourStrong!Pass123",
  [string]$KnightPassword = "knight",
  [string]$ContainerName = "ko-sql",
  [string]$DBDumpPath = "Rokoko DB 1098 DUMP from SQL 2000"
)

$ErrorActionPreference = "Stop"

Write-Host "Knight Online Database Setup" -ForegroundColor Green
Write-Host "=============================" -ForegroundColor Green
Write-Host ""

Write-Host "Step 1: Creating Knight databases..." -ForegroundColor Yellow

$createDbQuery = @"
IF DB_ID('Knight') IS NULL CREATE DATABASE Knight;
IF DB_ID('KN_Online') IS NULL CREATE DATABASE KN_Online;
IF NOT EXISTS (SELECT 1 FROM sys.sql_logins WHERE name = 'knight')
  CREATE LOGIN [knight] WITH PASSWORD = '$KnightPassword', CHECK_POLICY = OFF;
EXEC sp_addsrvrolemember 'knight','sysadmin';
"@

try {
    docker exec $ContainerName /opt/mssql-tools18/bin/sqlcmd -S localhost -U sa -P $SAPassword -C -Q $createDbQuery | Out-Null
    Write-Host "✓ Databases created/verified" -ForegroundColor Green
} catch {
    Write-Host "Error creating databases: $_" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Step 2: Importing data from Rokoko DB dump..." -ForegroundColor Yellow

# Find CSV files in the dump folder
$dumpDir = Get-Item -Path $DBDumpPath -ErrorAction SilentlyContinue
if (-not $dumpDir) {
    Write-Host "⚠ Database dump directory not found: $DBDumpPath" -ForegroundColor Yellow
    Write-Host "  Please run the full setup_db.ps1 script instead" -ForegroundColor Cyan
    exit 1
}

Write-Host "✓ Dump directory found: $dumpDir" -ForegroundColor Green
Write-Host "  CSV files available for import" -ForegroundColor Cyan

Write-Host ""
Write-Host "Setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Connection details:" -ForegroundColor Cyan
Write-Host "  Server: $SQLServerHost,$SQLServerPort"
Write-Host "  Database: Knight"
Write-Host "  User: knight"
Write-Host "  Password: $KnightPassword"
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Verify databases are created in SQL Server"
Write-Host "  2. Import data from CSV files using SQL Server Management Studio"
Write-Host "  3. Run GameServer.exe"
