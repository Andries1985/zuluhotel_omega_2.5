# Analyze-MemoryUsage.ps1
# Analyzes memoryusagescripts.log for script memory usage statistics

param(
    [string]$LogFile = "memoryusagescripts.log",
    [int]$TopN = 10,
    [switch]$ExportCsv
)

# -- Validate file --------------------------------------------------------------
if (-not (Test-Path $LogFile)) {
    Write-Error "Log file not found: $LogFile"
    exit 1
}

Write-Host "`n=== Memory Usage Log Analyzer ===" -ForegroundColor Cyan
Write-Host "File: $(Resolve-Path $LogFile)" -ForegroundColor Gray
Write-Host "Parsed: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`n" -ForegroundColor Gray

# -- Parse the log --------------------------------------------------------------
$entries   = [System.Collections.Generic.List[PSCustomObject]]::new()
$snapshots = [System.Collections.Generic.List[PSCustomObject]]::new()

$currentTimestamp = $null
$currentSection   = $null
$snapshotEntries  = [System.Collections.Generic.List[PSCustomObject]]::new()

# Section header pattern (lines ending with colon, no memory number)
$sectionPattern   = '^\s*(\w+):\s*$'
# Script entry pattern: path followed by a memory number
$entryPattern     = '^\s*(\S+\.ecl)\s+(\d+)\s*$'
# Timestamp pattern: [MM/DD HH:MM:SS]
$timestampPattern = '^\[(\d{2}/\d{2}\s+\d{2}:\d{2}:\d{2})\]'

$lines = Get-Content $LogFile

foreach ($line in $lines) {

    if ($line -match $timestampPattern) {
        # Save previous snapshot
        if ($currentTimestamp -and $snapshotEntries.Count -gt 0) {
            $snapshots.Add([PSCustomObject]@{
                Timestamp   = $currentTimestamp
                EntryCount  = $snapshotEntries.Count
                TotalMemory = ($snapshotEntries | Measure-Object -Property Memory -Sum).Sum
            })
        }
        $currentTimestamp = $matches[1]
        $snapshotEntries  = [System.Collections.Generic.List[PSCustomObject]]::new()
        $currentSection   = $null
        continue
    }

    if ($line -match $sectionPattern) {
        $currentSection = $matches[1]
        continue
    }

    if ($line -match $entryPattern) {
        $script = $matches[1]
        $memory = [int]$matches[2]

        $obj = [PSCustomObject]@{
            Timestamp = $currentTimestamp
            Section   = $currentSection
            Script    = $script
            Memory    = $memory
        }
        $entries.Add($obj)
        $snapshotEntries.Add($obj)
        continue
    }
}

# Save the last snapshot
if ($currentTimestamp -and $snapshotEntries.Count -gt 0) {
    $snapshots.Add([PSCustomObject]@{
        Timestamp   = $currentTimestamp
        EntryCount  = $snapshotEntries.Count
        TotalMemory = ($snapshotEntries | Measure-Object -Property Memory -Sum).Sum
    })
}

if ($entries.Count -eq 0) {
    Write-Warning "No script entries found in the log file."
    exit 0
}

# -- Overall summary --------------------------------------------------------------
$totalMemory     = ($entries | Measure-Object -Property Memory -Sum).Sum
$totalInstances  = $entries.Count
$uniqueScripts   = ($entries | Select-Object -ExpandProperty Script -Unique).Count
$totalSnapshots  = $snapshots.Count

Write-Host "-- OVERALL SUMMARY --------------------------------------------" -ForegroundColor Yellow
Write-Host ("  Snapshots (timestamps) : {0}"  -f $totalSnapshots)
Write-Host ("  Total script instances : {0}"  -f $totalInstances)
Write-Host ("  Unique scripts         : {0}"  -f $uniqueScripts)
Write-Host ("  Total memory (all)     : {0:N0} bytes  ({1:N2} KB)" -f $totalMemory, ($totalMemory / 1KB))
Write-Host ""

# -- Per-script aggregation ---------------------------------------------------------
$scriptStats = $entries | Group-Object -Property Script | ForEach-Object {
    $mems = $_.Group | Select-Object -ExpandProperty Memory
    $sum  = ($mems | Measure-Object -Sum).Sum
    $avg  = ($mems | Measure-Object -Average).Average
    $min  = ($mems | Measure-Object -Minimum).Minimum
    $max  = ($mems | Measure-Object -Maximum).Maximum

    [PSCustomObject]@{
        Script       = $_.Name
        Instances    = $_.Count
        TotalMemory  = $sum
        AvgMemory    = [math]::Round($avg, 0)
        MinMemory    = $min
        MaxMemory    = $max
        MemoryGrowth = $max - $min   # potential leak indicator
    }
}

# -- Top N by total memory ----------------------------------------------------------
Write-Host ("-- TOP {0} SCRIPTS BY TOTAL MEMORY -----------------------------" -f $TopN) -ForegroundColor Yellow
$scriptStats | Sort-Object TotalMemory -Descending | Select-Object -First $TopN |
    Format-Table -AutoSize @(
        @{L='Script';       E={$_.Script};                    W=60}
        @{L='Instances';    E={$_.Instances};                 W=10; A='Right'}
        @{L='Total (B)';    E={'{0:N0}' -f $_.TotalMemory};  W=14; A='Right'}
        @{L='Avg (B)';      E={'{0:N0}' -f $_.AvgMemory};    W=10; A='Right'}
        @{L='Min (B)';      E={'{0:N0}' -f $_.MinMemory};    W=10; A='Right'}
        @{L='Max (B)';      E={'{0:N0}' -f $_.MaxMemory};    W=10; A='Right'}
    )

# -- Top N by instance count (most spawned) -----------------------------------------
Write-Host ("-- TOP {0} SCRIPTS BY INSTANCE COUNT ---------------------------" -f $TopN) -ForegroundColor Yellow
$scriptStats | Sort-Object Instances -Descending | Select-Object -First $TopN |
    Format-Table -AutoSize @(
        @{L='Script';     E={$_.Script};                   W=60}
        @{L='Instances';  E={$_.Instances};                W=10; A='Right'}
        @{L='Total (B)';  E={'{0:N0}' -f $_.TotalMemory}; W=14; A='Right'}
        @{L='Avg (B)';    E={'{0:N0}' -f $_.AvgMemory};   W=10; A='Right'}
    )

# -- Memory growth / leak suspects ---------------------------------------------------
$leakSuspects = $scriptStats | Where-Object { $_.MemoryGrowth -gt 0 } |
    Sort-Object MemoryGrowth -Descending | Select-Object -First $TopN

if ($leakSuspects) {
    Write-Host "-- MEMORY GROWTH SUSPECTS (Max - Min > 0) --------------------" -ForegroundColor Yellow
    $leakSuspects | Format-Table -AutoSize @(
        @{L='Script';         E={$_.Script};                       W=60}
        @{L='Instances';      E={$_.Instances};                    W=10; A='Right'}
        @{L='Min (B)';        E={'{0:N0}' -f $_.MinMemory};        W=10; A='Right'}
        @{L='Max (B)';        E={'{0:N0}' -f $_.MaxMemory};        W=10; A='Right'}
        @{L='Growth (B)';     E={'{0:N0}' -f $_.MemoryGrowth};     W=12; A='Right'}
    )
} else {
    Write-Host "  No memory growth detected across instances.`n" -ForegroundColor Green
}

# -- Snapshot timeline ----------------------------------------------------------------
if ($snapshots.Count -gt 1) {
    Write-Host "-- SNAPSHOT TIMELINE ------------------------------------------" -ForegroundColor Yellow
    $snapshots | Format-Table -AutoSize @(
        @{L='Timestamp';    E={$_.Timestamp};                    W=20}
        @{L='Instances';    E={$_.EntryCount};                   W=10; A='Right'}
        @{L='Total (B)';    E={'{0:N0}' -f $_.TotalMemory};      W=14; A='Right'}
        @{L='Total (KB)';   E={'{0:N2}' -f ($_.TotalMemory/1KB)};W=12; A='Right'}
    )
}

# -- Section breakdown ----------------------------------------------------------------
$sectionStats = $entries | Where-Object { $_.Section } |
    Group-Object -Property Section | ForEach-Object {
        [PSCustomObject]@{
            Section     = $_.Name
            Instances   = $_.Count
            TotalMemory = ($_.Group | Measure-Object -Property Memory -Sum).Sum
        }
    } | Sort-Object TotalMemory -Descending

if ($sectionStats) {
    Write-Host "-- MEMORY BY SECTION ------------------------------------------" -ForegroundColor Yellow
    $sectionStats | Format-Table -AutoSize @(
        @{L='Section';    E={$_.Section};                    W=25}
        @{L='Instances';  E={$_.Instances};                  W=10; A='Right'}
        @{L='Total (B)';  E={'{0:N0}' -f $_.TotalMemory};   W=14; A='Right'}
        @{L='Total (KB)'; E={'{0:N2}' -f ($_.TotalMemory/1KB)}; W=12; A='Right'}
    )
}

# -- Optional CSV export ---------------------------------------------------------------
if ($ExportCsv) {
    $csvPath = [System.IO.Path]::ChangeExtension($LogFile, '.csv')
    $scriptStats | Sort-Object TotalMemory -Descending |
        Export-Csv -Path $csvPath -NoTypeInformation -Encoding UTF8
    Write-Host "CSV exported to: $csvPath" -ForegroundColor Green
}

Write-Host "=== Analysis complete ===`n" -ForegroundColor Cyan
