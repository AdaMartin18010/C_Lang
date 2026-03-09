# =============================================
# Knowledge Directory Analysis Script
# =============================================

param(
    [string]$BasePath = "E:\_src\C_Lang\knowledge"
)

# 输出报告文件
$ReportFile = "E:\_src\C_Lang\knowledge_analysis_report.txt"

# 清空或创建报告文件
"=" * 80 | Out-File -FilePath $ReportFile -Encoding UTF8
"  Knowledge Directory Analysis Report" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"  Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"=" * 80 | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"" | Out-File -FilePath $ReportFile -Append -Encoding UTF8

Write-Host "开始分析 Knowledge 目录..." -ForegroundColor Green

# =============================================
# 任务1: 找出所有内容少于100行的.md文件
# =============================================
Write-Host "`n[任务1] 分析内容少于100行的.md文件..." -ForegroundColor Cyan

$shortFiles = @()
$allMdFiles = Get-ChildItem -Path $BasePath -Recurse -Filter "*.md"

foreach ($file in $allMdFiles) {
    $lineCount = (Get-Content -Path $file.FullName | Measure-Object -Line).Lines
    if ($lineCount -lt 100) {
        $shortFiles += [PSCustomObject]@{
            FilePath = $file.FullName.Replace($BasePath, ".")
            Lines = $lineCount
            SizeKB = [math]::Round($file.Length / 1KB, 2)
        }
    }
}

# 输出到报告
"## 任务1: 内容少于100行的.md文件 ($(($shortFiles | Measure-Object).Count) 个)" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"-" * 80 | Out-File -FilePath $ReportFile -Append -Encoding UTF8

if ($shortFiles.Count -eq 0) {
    "✓ 所有.md文件内容均超过100行" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
} else {
    $shortFiles | Sort-Object Lines | Format-Table -AutoSize | Out-String | Out-File -FilePath $ReportFile -Append -Encoding UTF8
}
"" | Out-File -FilePath $ReportFile -Append -Encoding UTF8

Write-Host "  发现 $($shortFiles.Count) 个内容不足的.md文件" -ForegroundColor $(if($shortFiles.Count -eq 0){"Green"}else{"Yellow"})

# =============================================
# 任务2: 检查所有README.md中的内部链接
# =============================================
Write-Host "`n[任务2] 检查README.md中的内部链接..." -ForegroundColor Cyan

$readmeFiles = Get-ChildItem -Path $BasePath -Recurse -Filter "README.md"
$brokenLinks = @()

foreach ($readme in $readmeFiles) {
    $content = Get-Content -Path $readme.FullName -Raw
    $readmeDir = $readme.DirectoryName
    
    # 匹配 ./xxx/xxx.md 格式的内部链接
    $linkPattern = '\]\(\.\/([^)]+\.md)\)'
    $matches = [regex]::Matches($content, $linkPattern)
    
    foreach ($match in $matches) {
        $relativePath = $match.Groups[1].Value
        $fullPath = Join-Path $readmeDir $relativePath
        $fullPath = [System.IO.Path]::GetFullPath($fullPath)
        
        if (-not (Test-Path -Path $fullPath)) {
            $brokenLinks += [PSCustomObject]@{
                SourceReadme = $readme.FullName.Replace($BasePath, ".")
                BrokenLink = "./" + $relativePath
                SuggestedAction = "文件不存在"
            }
        }
    }
}

"## 任务2: README.md中的失效内部链接 ($(($brokenLinks | Measure-Object).Count) 个)" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"-" * 80 | Out-File -FilePath $ReportFile -Append -Encoding UTF8

if ($brokenLinks.Count -eq 0) {
    "✓ 所有内部链接均有效" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
} else {
    $brokenLinks | Format-Table -AutoSize | Out-String | Out-File -FilePath $ReportFile -Append -Encoding UTF8
}
"" | Out-File -FilePath $ReportFile -Append -Encoding UTF8

Write-Host "  发现 $($brokenLinks.Count) 个失效链接" -ForegroundColor $(if($brokenLinks.Count -eq 0){"Green"}else{"Red"})

# =============================================
# 任务3: 统计每个主要目录下的文件数量和内容分布
# =============================================
Write-Host "`n[任务3] 统计主要目录的文件分布..." -ForegroundColor Cyan

$mainDirs = Get-ChildItem -Path $BasePath -Directory | Where-Object { $_.Name -notin @('templates') }

"## 任务3: 主要目录文件统计" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"-" * 80 | Out-File -FilePath $ReportFile -Append -Encoding UTF8

foreach ($dir in $mainDirs) {
    $dirName = $dir.Name
    Write-Host "  分析目录: $dirName" -ForegroundColor Gray
    
    # 获取该目录下的所有.md文件
    $mdFiles = Get-ChildItem -Path $dir.FullName -Recurse -Filter "*.md"
    $totalFiles = ($mdFiles | Measure-Object).Count
    
    if ($totalFiles -eq 0) {
        continue
    }
    
    # 计算总行数和平均行数
    $totalLines = 0
    $fileDetails = @()
    
    foreach ($file in $mdFiles) {
        $lines = (Get-Content -Path $file.FullName | Measure-Object -Line).Lines
        $totalLines += $lines
        $fileDetails += [PSCustomObject]@{
            Name = $file.Name
            RelativePath = $file.FullName.Replace($dir.FullName, "").TrimStart("\")
            Lines = $lines
            SizeKB = [math]::Round($file.Length / 1KB, 2)
        }
    }
    
    $avgLines = if ($totalFiles -gt 0) { [math]::Round($totalLines / $totalFiles, 1) } else { 0 }
    
    # 行数分布统计
    $under50 = ($fileDetails | Where-Object { $_.Lines -lt 50 } | Measure-Object).Count
    $between50_100 = ($fileDetails | Where-Object { $_.Lines -ge 50 -and $_.Lines -lt 100 } | Measure-Object).Count
    $between100_200 = ($fileDetails | Where-Object { $_.Lines -ge 100 -and $_.Lines -lt 200 } | Measure-Object).Count
    $between200_500 = ($fileDetails | Where-Object { $_.Lines -ge 200 -and $_.Lines -lt 500 } | Measure-Object).Count
    $over500 = ($fileDetails | Where-Object { $_.Lines -ge 500 } | Measure-Object).Count
    
    # 子目录统计
    $subDirs = Get-ChildItem -Path $dir.FullName -Directory
    $subDirCount = ($subDirs | Measure-Object).Count
    
    # 输出到报告
    "### $dirName" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "  - 子目录数: $subDirCount" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "  - Markdown文件总数: $totalFiles" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "  - 总行数: $totalLines" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "  - 平均行数/文件: $avgLines" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "  - 行数分布:" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "    * < 50 行:   $under50 个" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "    * 50-100 行: $between50_100 个" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "    * 100-200行: $between100_200 个" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "    * 200-500行: $between200_500 个" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "    * > 500 行:  $over500 个" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "  - 文件列表:" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    $fileDetails | Sort-Object Lines | Select-Object RelativePath, Lines, SizeKB | Format-Table -AutoSize | Out-String | Out-File -FilePath $ReportFile -Append -Encoding UTF8
    "" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
}

# =============================================
# 总结
# =============================================
"## 总结" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"-" * 80 | Out-File -FilePath $ReportFile -Append -Encoding UTF8
$totalMdCount = ($allMdFiles | Measure-Object).Count
$totalLinesAll = 0
foreach ($file in $allMdFiles) {
    $totalLinesAll += (Get-Content -Path $file.FullName | Measure-Object -Line).Lines
}

"- Markdown文件总数: $totalMdCount" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"- 内容不足文件数 (<100行): $($shortFiles.Count)" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"- 失效链接数: $($brokenLinks.Count)" | Out-File -FilePath $ReportFile -Append -Encoding UTF8
"- 总代码行数: $totalLinesAll" | Out-File -FilePath $ReportFile -Append -Encoding UTF8

Write-Host "`n分析完成！报告已保存到: $ReportFile" -ForegroundColor Green
Write-Host "`n========== 摘要 ==========" -ForegroundColor Cyan
Write-Host "Markdown文件总数: $totalMdCount" -ForegroundColor White
Write-Host "内容不足文件数 (<100行): $($shortFiles.Count)" -ForegroundColor $(if($shortFiles.Count -eq 0){"Green"}else{"Yellow"})
Write-Host "失效链接数: $($brokenLinks.Count)" -ForegroundColor $(if($brokenLinks.Count -eq 0){"Green"}else{"Red"})
