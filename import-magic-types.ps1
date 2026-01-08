# Import MAGIC_TYPE CSV files (no headers)
$dbPath = "c:\Users\kzorluoglu\CLionProjects\KnightOnline-1068\Rokoko DB 1098 DUMP from SQL 2000"
$conn = New-Object System.Data.SqlClient.SqlConnection("Server=localhost,1433;Database=Knight;User Id=knight;Password=knight;Encrypt=no;TrustServerCertificate=yes;")
$conn.Open()

$tableMappings = @{
    "MAGIC_TYPE2" = @("iNum", "Name", "Description", "HitType", "HitRate", "AddDamage", "AddRange", "NeedArrow")
    "MAGIC_TYPE3" = @("iNum", "Name", "Description", "Radius", "Angle", "DirectType", "FirstDamage", "EndDamage", "TimeDamage", "Duration", "Attribute")
    "MAGIC_TYPE5" = @("iNum", "Name", "Description", "Type", "ExpRecover", "NeedStone")
    "MAGIC_TYPE6" = @("dwID", "szEngName", "szName", "i1", "dw6")
}

foreach ($table in @("MAGIC_TYPE2", "MAGIC_TYPE3", "MAGIC_TYPE5", "MAGIC_TYPE6")) {
    $csvFile = "$dbPath\$table.csv"
    
    if (!(Test-Path $csvFile)) {
        Write-Host "File not found: $csvFile"
        continue
    }
    
    $columns = $tableMappings[$table]
    Write-Host "Importing $table with columns: $($columns -join ', ')"
    
    $importedCount = 0
    $errorCount = 0
    
    $lines = Get-Content $csvFile
    foreach ($line in $lines) {
        $parts = $line -split ',(?=(?:[^"]*"[^"]*")*[^"]*$)' | ForEach-Object { $_.Trim('"').Trim() }
        
        if ($parts.Count -ne $columns.Count) {
            $errorCount++
            continue
        }
        
        $values = @()
        for ($i = 0; $i -lt $parts.Count; $i++) {
            $val = $parts[$i].Trim()
            if ($val -eq "" -or $val -eq "NULL") {
                $values += "NULL"
            } else {
                $val = $val -replace "'", "''"
                $values += "'$val'"
            }
        }
        
        $sql = "INSERT INTO dbo.$table ($($columns -join ', ')) VALUES ($($values -join ', '))"
        
        try {
            $cmd = $conn.CreateCommand()
            $cmd.CommandText = $sql
            $cmd.ExecuteNonQuery() | Out-Null
            $importedCount++
        } catch {
            $errorCount++
            if ($errorCount -le 3) {
                Write-Host "Error: $($_.Exception.Message)"
                Write-Host "SQL: $sql"
            }
        }
    }
    
    Write-Host "${table}: Imported $importedCount rows, $errorCount errors"
}

$conn.Close()

# Verify import
Write-Host ""
Write-Host "Final counts:"
$conn = New-Object System.Data.SqlClient.SqlConnection("Server=localhost,1433;Database=Knight;User Id=knight;Password=knight;Encrypt=no;TrustServerCertificate=yes;")
$conn.Open()
foreach ($table in @("MAGIC_TYPE2", "MAGIC_TYPE3", "MAGIC_TYPE5", "MAGIC_TYPE6")) {
    $cmd = $conn.CreateCommand()
    $cmd.CommandText = "SELECT COUNT(*) FROM dbo.$table"
    $count = $cmd.ExecuteScalar()
    Write-Host "${table}: $count rows"
}
$conn.Close()
