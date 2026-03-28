Set-Location 'E:\_src\C_Lang\knowledge'
$files = Get-ChildItem -Recurse -Filter '*.md' | Select-Object -ExpandProperty FullName
$results = @()
$count = 0

foreach ($file in $files) {
    $count++
    if ($count % 200 -eq 0) { Write-Host "Processing $count..." }
    
    $lines = Get-Content $file -ErrorAction SilentlyContinue
    $lineCount = ($lines | Measure-Object).Count
    
    # Focus on files with less than 100 lines
    if ($lineCount -lt 100) {
        $hasTOC = $lines | Select-String -Pattern '## 目录' -Quiet
        $hasDeep = $lines | Select-String -Pattern '深入理解|深入探讨|深层理解' -Quiet
        $isPlaceholder = $lines | Select-String -Pattern 'TODO|FIXME|待完善|待补充|占位符|placeholder|内容待补充' -Quiet
        
        $results += [PSCustomObject]@{
            File = $file.Replace((Get-Location).Path + '\', '')
            Lines = $lineCount
            HasTOC = if ($hasTOC) { 'Yes' } else { 'No' }
            HasDeep = if ($hasDeep) { 'Yes' } else { 'No' }
            IsPlaceholder = if ($isPlaceholder) { 'Yes' } else { 'No' }
        }
    }
}

Write-Host "`n=== Files with less than 100 lines ==="
$results | Sort-Object Lines | Select-Object -First 50 | Format-Table -AutoSize

# Export to CSV for further analysis
$results | Sort-Object Lines | Export-Csv -Path 'E:\_src\C_Lang\scripts\short_files.csv' -NoTypeInformation
Write-Host "`nResults exported to scripts\short_files.csv"
