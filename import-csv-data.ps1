param(
    [string]$SQLServerHost = "localhost",
    [int]$SQLServerPort = 1433,
    [string]$SQLUser = "knight",
    [string]$SQLPassword = "knight",
    [string]$Database = "Knight",
    [string]$CSVPath = "Rokoko DB 1098 DUMP from SQL 2000"
)

Write-Host "CSV Import to SQL Server (Client-Side)"
Write-Host "======================================`n"

# Step 1: Discover CSV files
Write-Host "Step 1: Discovering CSV files..."
$csvFiles = Get-ChildItem -Path $CSVPath -Filter "*.csv" -ErrorAction SilentlyContinue | Sort-Object Name
$fileCount = $csvFiles.Count
Write-Host "Found $fileCount CSV files`n"

Write-Host "Step 2: Importing CSV files to SQL Server..."
Write-Host "Server: $($SQLServerHost),$($SQLServerPort)"
Write-Host "Database: $Database`n"

$imported = 0
$skipped = 0
$failed = 0

$connectionString = "Server=$SQLServerHost,$SQLServerPort;Database=$Database;User Id=$SQLUser;Password=$SQLPassword;Encrypt=no;TrustServerCertificate=yes;"

foreach ($csvFile in $csvFiles) {
    $fileName = $csvFile.Name
    $csvPath = $csvFile.FullName
    $tableName = [System.IO.Path]::GetFileNameWithoutExtension($fileName)
    
    Write-Host "Importing: $fileName..." -NoNewline
    
    # Check if file is empty
    if ($csvFile.Length -eq 0) {
        Write-Host " [SKIPPED - empty]"
        $skipped++
        continue
    }
    
    try {
        # Connect to SQL Server
        $conn = New-Object System.Data.SqlClient.SqlConnection($connectionString)
        $conn.Open()
        
        # Read CSV file
        $csvData = Get-Content -Path $csvPath
        if (-not $csvData -or $csvData.Count -eq 0) {
            Write-Host " [SKIPPED - no data]"
            $conn.Close()
            $skipped++
            continue
        }
        
        # Get first line to determine column count
        $firstLine = if ($csvData -is [array]) { $csvData[0] } else { $csvData }
        $columns = $firstLine -split ','
        $colCount = $columns.Count
        
        # Check if table exists, create if not
        $checkTableSQL = "SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='dbo' AND TABLE_NAME='$tableName'"
        $cmd = $conn.CreateCommand()
        $cmd.CommandText = $checkTableSQL
        $tableExists = [int]$cmd.ExecuteScalar()
        
        if ($tableExists -eq 0) {
            # Create table with generic columns
            $tableDefParts = @()
            for ($i = 0; $i -lt $colCount; $i++) {
                $tableDefParts += "[Col$i] NVARCHAR(MAX)"
            }
            $tableDef = $tableDefParts -join ', '
            
            $createTableSQL = "CREATE TABLE dbo.$tableName ($tableDef)"
            $cmd = $conn.CreateCommand()
            $cmd.CommandText = $createTableSQL
            $cmd.CommandTimeout = 60
            $cmd.ExecuteNonQuery() | Out-Null
        }
        
        # Build INSERT statements for all rows
        $totalRows = 0
        $batchSQL = ""
        $batchRowCount = 0
        
        # Convert to array if single line
        if ($csvData -is [string]) {
            $csvData = @($csvData)
        }
        
        foreach ($line in $csvData) {
            if ([string]::IsNullOrWhiteSpace($line)) { continue }
            
            # Escape single quotes in CSV data
            $values = $line -split ',' | ForEach-Object {
                $val = $_.Trim()
                # Escape single quotes
                $val = $val -replace "'", "''"
                "'$val'"
            }
            $valueList = $values -join ', '
            
            $insertSQL = "INSERT INTO dbo.$tableName VALUES ($valueList);"
            $batchSQL += $insertSQL + "`n"
            $batchRowCount++
            $totalRows++
            
            # Execute batch every 100 rows to avoid command text limit
            if ($batchRowCount -ge 100) {
                $cmd = $conn.CreateCommand()
                $cmd.CommandText = $batchSQL
                $cmd.CommandTimeout = 300
                try {
                    $cmd.ExecuteNonQuery() | Out-Null
                } catch {
                    # Log error but continue
                    Write-Host " [WARNING: Batch insert error: $_]"
                }
                $batchSQL = ""
                $batchRowCount = 0
            }
        }
        
        # Execute remaining batch
        if ($batchSQL) {
            $cmd = $conn.CreateCommand()
            $cmd.CommandText = $batchSQL
            $cmd.CommandTimeout = 300
            $cmd.ExecuteNonQuery() | Out-Null
        }
        
        $conn.Close()
        
        Write-Host " [OK - $totalRows rows]"
        $imported++
        
    } catch {
        Write-Host " [ERROR]"
        Write-Host "  Error details: $_"
        $failed++
    }
}

Write-Host "`nImport Summary:"
Write-Host "  ✓ Imported: $imported tables"
Write-Host "  ⚠ Skipped: $skipped tables"
Write-Host "  ✗ Failed: $failed tables"
Write-Host "`nDone!"
