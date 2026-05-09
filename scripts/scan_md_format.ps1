cd e:\_src\C_Lang\knowledge
$files = Get-ChildItem -Recurse -Filter "*.md" | Select-Object -First 200 FullName, Name, DirectoryName
foreach ($f in $files) {
    $content = Get-Content $f.FullName -Raw
    $lines = Get-Content $f.FullName
    $issues = @()
    $h1Count = ([regex]::Matches($content, "^# [^#]", "Multiline")).Count
    if ($h1Count -gt 1) { $issues += "多H1($h1Count)" }
    $openFence = 0
    foreach ($line in $lines) {
        if ($line -match "^```") { $openFence++ }
    }
    if ($openFence % 2 -ne 0) { $issues += "代码块未闭合($openFence)" }
    $emptyLinks = ([regex]::Matches($content, "\[([^\]]*)\]\(\s*\)")).Count
    if ($emptyLinks -gt 0) { $issues += "空链接($emptyLinks)" }
    if ($issues.Count -gt 0) {
        $rel = $f.FullName.Replace('e:\_src\C_Lang\', '')
        $iss = $issues -join '; '
        Write-Output "$rel :: $iss"
    }
}
