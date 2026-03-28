Set-Location 'E:\_src\C_Lang\knowledge'
$files = Get-ChildItem -Recurse -Filter '*.md' | Select-Object -ExpandProperty FullName
$results = @()
$count = 0

foreach ($file in $files) {
    $count++
    if ($count % 300 -eq 0) { Write-Host "Processing $count..." }
    
    $content = Get-Content $file -Raw -ErrorAction SilentlyContinue
    $lines = $content -split "`r?`n"
    $lineCount = $lines.Count
    
    # Check for placeholder patterns in "deep understanding" sections
    $hasDeepSection = $content -match '(##\s*深入理解|##\s*深入探讨|##\s*深层理解)'
    $hasPlaceholderInDeep = $content -match '(##\s*深入理解|##\s*深入探讨|##\s*深层理解)[^#]*?(TODO|FIXME|待完善|待补充|占位符|placeholder|内容待补充|\*\*\*)'
    
    # Check if file has minimal content
    $isMinimal = ($lineCount -lt 50) -or (($content -replace '\s', '').Length -lt 500)
    
    # Check for standard structure
    $hasTOC = $content -match '##\s*目录'
    $hasExamples = $content -match '```c'
    $hasSummary = $content -match '(##\s*总结|##\s*小结)'
    
    if ($lineCount -lt 100 -or $hasPlaceholderInDeep -or ($hasDeepSection -and -not $hasExamples)) {
        $results += [PSCustomObject]@{
            File = $file.Replace((Get-Location).Path + '\', '')
            Lines = $lineCount
            HasTOC = if ($hasTOC) { 'Yes' } else { 'No' }
            HasDeepSection = if ($hasDeepSection) { 'Yes' } else { 'No' }
            HasPlaceholderDeep = if ($hasPlaceholderInDeep) { 'Yes' } else { 'No' }
            HasExamples = if ($hasExamples) { 'Yes' } else { 'No' }
            HasSummary = if ($hasSummary) { 'Yes' } else { 'No' }
            IsMinimal = if ($isMinimal) { 'Yes' } else { 'No' }
        }
    }
}

Write-Host "`n=== Files Needing Enhancement (sorted by line count) ==="
$results | Sort-Object Lines | Select-Object -First 40 | Format-Table -AutoSize

# Export results
$results | Sort-Object Lines | Export-Csv -Path 'E:\_src\C_Lang\scripts\enhancement_needed.csv' -NoTypeInformation
Write-Host "`nResults exported to scripts\enhancement_needed.csv"
Write-Host "Total files needing enhancement: $($results.Count)"
