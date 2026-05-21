# Rename namespace VisionCom -> VisionComm across source files
# Backup each changed file to <file>.bak_TIMESTAMP
# Usage: powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\rename_namespace_visioncom_to_visioncomm.ps1

$timestamp = Get-Date -Format "yyyyMMddHHmmss"
$report = "rename_report_$timestamp.txt"
Write-Output "Rename started at $(Get-Date)" | Tee-Object $report

# File extensions to process
$exts = @('*.h','*.hpp','*.hh','*.c','*.cpp','*.cc','*.filters','*.vcxproj','*.rc')

# Gather files
$files = Get-ChildItem -Recurse -Include $exts -ErrorAction SilentlyContinue | Where-Object { -not $_.PSIsContainer }
Write-Output "Found $($files.Count) files to inspect." | Tee-Object $report -Append

foreach ($f in $files) {
    $path = $f.FullName
    $content = Get-Content -Raw -ErrorAction SilentlyContinue -Path $path
    if ($null -eq $content) { continue }
    $orig = $content
    $modified = $false

    # Backup
    $bak = "$path.bak_$timestamp"
    if (-not (Test-Path $bak)) { Copy-Item -Path $path -Destination $bak -Force }

    # 1) Replace namespace declaration
    $new = [regex]::Replace($content, '\bnamespace\s+VisionCom\b', 'namespace VisionComm')
    if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Replaced namespace VisionCom -> VisionComm in: $path" | Tee-Object $report -Append }

    # 2) Replace qualified references VisionCom:: -> VisionComm::
    $new = [regex]::Replace($content, '\bVisionCom::', 'VisionComm::')
    if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Replaced VisionCom:: -> VisionComm:: in: $path" | Tee-Object $report -Append }

    # 3) Replace closing comment markers: // namespace VisionCom -> // namespace VisionComm
    $new = $content -replace '//\s*namespace\s+VisionCom', '// namespace VisionComm'
    if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Updated closing namespace comments in: $path" | Tee-Object $report -Append }

    # 4) Replace using-directives
    $new = [regex]::Replace($content, '\busing\s+namespace\s+VisionCom\b', 'using namespace VisionComm')
    if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Replaced using namespace VisionCom in: $path" | Tee-Object $report -Append }

    # 5) Write back if changed
    if ($modified) {
        Set-Content -Path $path -Value $content -Encoding UTF8
    }
}

# Add compatibility alias into VisionComAPI.h (if present) so old code using VisionCom still works
$vcApiPaths = Get-ChildItem -Recurse -Include VisionComAPI.h -ErrorAction SilentlyContinue
foreach ($p in $vcApiPaths) {
    $file = $p.FullName
    $content = Get-Content -Raw -Path $file
    if ($content -notmatch 'namespace\s+VisionComm') {
        # ensure alias is present
        $insertion = "\n// Backwards-compatibility: alias old namespace name to new one\nnamespace VisionComm {}\nnamespace VisionCom = VisionComm;\n"
        # append after macro block
        if ($content -match "#endif") {
            # append at end
            $content = $content + $insertion
        } else {
            $content = $content + $insertion
        }
        Copy-Item -Path $file -Destination "$file.bak_$timestamp" -Force
        Set-Content -Path $file -Value $content -Encoding UTF8
        Write-Output "Added compatibility alias to: $file" | Tee-Object $report -Append
    }
}

Write-Output "Rename finished at $(Get-Date). See report: $report" | Tee-Object $report -Append
exit 0
